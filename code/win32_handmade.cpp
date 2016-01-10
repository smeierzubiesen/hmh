/*
// TODO(smzb): Add a better comment template for new .cpp and .h files
*/

#include <windows.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t  int64;

struct win32_offscreen_buffer {
    BITMAPINFO Info; // TODO(smzb): This should not be a global in future.
    void *Memory; // TODO(smzb): This should not be a global in future.
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
};

struct win32_window_dimensions
{
    int Width;
    int Height;
};

win32_window_dimensions GetWindowDimensions(HWND Window) {
    win32_window_dimensions Result;
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    return(Result);
}

global_variable bool Running; // TODO(smzb): This should not be a global in future.
global_variable win32_offscreen_buffer GlobalBackBuffer;

internal void RenderGradient(win32_offscreen_buffer Buffer, int XOffset, int YOffset)
{
    int Width = Buffer.Width;
    int height = Buffer.Height;


    uint8 *Row = (uint8 *)Buffer.Memory;
    for(int Y = 0; Y < Buffer.Height; Y++)
    {
        uint32 *Pixel = (uint32 *)Row;
        for(int X = 0; X < Buffer.Width; X++)
        {
            uint8 Blue = (X + XOffset);
            uint8 Green = (Y + YOffset);
            uint8 Red = (YOffset + XOffset);

            *Pixel++ = ((Red << 16) | (Green << 8) | (Blue));
		}

        Row += Buffer.Pitch;
    }
}

internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;
    
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;


    int BitmapMemorySize = (Buffer->Width*Buffer->Height)*Buffer->BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    // TODO(smzb): Might want a blanking routine here.

    Buffer->Pitch = Width*Buffer->BytesPerPixel;
}

internal void Win32UpdateWindow(HDC DeviceContext, win32_offscreen_buffer Buffer, int WindowWidth, int WindowHeight, int X, int Y, int W, int H)
{
    StretchDIBits(DeviceContext,
                  0, 0, Buffer.Width, Buffer.Height,
                  0, 0, WindowWidth, WindowHeight,
                  Buffer.Memory,
                  &Buffer.Info,
                  DIB_RGB_COLORS,
                  SRCCOPY);
}

LRESULT CALLBACK Win32MainWindowCallBack(
    HWND   Window,
    UINT   Message,
    WPARAM WParam,
    LPARAM LParam)
{
    LRESULT Result = 0;
    switch(Message)
    {
        case WM_SIZE:
        {
            win32_window_dimensions Dimensions = GetWindowDimensions(Window);
            Win32ResizeDIBSection(&GlobalBackBuffer, Dimensions.Width, Dimensions.Height);
        } break;
        case WM_DESTROY:
        {
            Running = false; // TODO(smzb): Handle this as error? Re-create Window?
        } break;
        case WM_CLOSE:
        {
            // FIXED(smzb): Create function to close window.
            Running = false; // TODO(smzb): Throw a message to the user to ask for confirmation?
        } break;
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;
        case WM_PAINT:
        {
            //OutputDebugStringA("WM_PAINT"); // FIXED(smzb): This won't be needed anymore - addresses #3
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.top;
			int H = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int W = Paint.rcPaint.right - Paint.rcPaint.left;

            win32_window_dimensions Dim = GetWindowDimensions(Window);
            Win32UpdateWindow(DeviceContext, GlobalBackBuffer, Dim.Width, Dim.Height, X, Y, W, H);
            EndPaint(Window, &Paint);
        } break;
        default:
        {
            OutputDebugStringA("no message\n");
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }
    return(Result);
}

int CALLBACK
WinMain(
	HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR     CommandLine,
	int       ShowCode)
{
    WNDCLASS WindowClass = {};
    //WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW; // FIXED(smzb): Check wether HREDRAW and VREDRAW are still necessary [confirmed] HREDRAW|VREDRAW needed due to force refresh upon resize
    WindowClass.style = CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallBack;
    WindowClass.hInstance = Instance;
//    WindowClass.hIcon; // TODO(smzb): Make an icon and stick it in here.
    WindowClass.lpszClassName = "HandmadeHeroWindowClass";

    if(RegisterClass(&WindowClass))
    {
        HWND Window = CreateWindowEx(0,
                                           WindowClass.lpszClassName,
                                           "Handmade Hero", // TODO(smzb): maybe some version information here?
                                           WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           0,
                                           0,
                                           Instance,
                                           0);
        if (Window)
        {
            int XOffset = 0;
            int YOffset = 0;
            Running = true;
            while(Running)
            {

                MSG Message;
                while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if (Message.message == WM_QUIT)
                    {
                        Running = false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                RenderGradient(GlobalBackBuffer, XOffset, YOffset);
                HDC DeviceContext = GetDC(Window);
                win32_window_dimensions Dim = GetWindowDimensions(Window);
                Win32UpdateWindow(DeviceContext, GlobalBackBuffer, Dim.Width, Dim.Height, 0, 0, Dim.Width, Dim.Height);
                ReleaseDC(Window, DeviceContext);
                ++XOffset;
            }
        } else {
            // TODO(smzb): Log this event !WindowHandle
        }
    }
    else
    {
        // TODO(smzb): Log this event !RegisterClass
    }
    
    return(0);
}

