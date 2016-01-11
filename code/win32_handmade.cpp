/*
// TODO(smzb): Add a better comment template for new .cpp and .h files
*/

#include <windows.h>
#include <stdint.h>
#include "win32_handmade.h"

win32_window_dimensions Win32GetWindowDimensions(HWND Window) {
    win32_window_dimensions Result;
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    return(Result);
}

internal void RenderGradient(win32_offscreen_buffer Buffer, int XOffset, int YOffset) {
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
            //uint8 Red = (XOffset|YOffset);
            *Pixel++ = (/*(Red << 16) |*/ (Green << 8) | (Blue));
		}
        Row += Buffer.Pitch;
    }
}

internal void Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height) {
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
    Buffer->Pitch = Width*Buffer->BytesPerPixel;
}



internal void Win32UpdateWindow(HDC DeviceContext, win32_offscreen_buffer Buffer, int WindowWidth, int WindowHeight, int X, int Y, int W, int H) {
    // TODO(smzb): Aspect ratio needs looked at.
    StretchDIBits(DeviceContext,
                  0, 0, WindowWidth, WindowHeight,
                  0, 0, Buffer.Width, Buffer.Height,
                  Buffer.Memory,
                  &Buffer.Info,
                  DIB_RGB_COLORS,
                  SRCCOPY);
}

LRESULT CALLBACK Win32MainWindowCallBack(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam) {
    LRESULT Result = 0;
    switch(Message)
    {
        case WM_SIZE:
        {
        } break;
        case WM_DESTROY:
        {
            Running = false; // TODO(smzb): Handle this as error? Re-create Window?
        } break;
        case WM_CLOSE:
        {
            Running = false; // TODO(smzb): Throw a message to the user to ask for confirmation?
        } break;
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.top;
			int H = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int W = Paint.rcPaint.right - Paint.rcPaint.left;
            win32_window_dimensions Dim = Win32GetWindowDimensions(Window);
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

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine,	int ShowCode) {
    WNDCLASS WindowClass = {};
    Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);
    WindowClass.style = CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallBack;
    WindowClass.hInstance = Instance;
//    WindowClass.hIcon; // TODO(smzb): Make an icon and stick it in here.
    WindowClass.lpszClassName = "HandmadeHeroWindowClass";
    char title[32] = "Handmade Hero (day5)";
    char* WindowTitle = title;

    if(RegisterClass(&WindowClass))
    {
        HWND Window = CreateWindowEx(0, WindowClass.lpszClassName,
                                     WindowTitle, // TODO(smzb): maybe some version information here?
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
                win32_window_dimensions Dim = Win32GetWindowDimensions(Window);
                Win32UpdateWindow(DeviceContext, GlobalBackBuffer, Dim.Width, Dim.Height, 0, 0, Dim.Width, Dim.Height);
                ReleaseDC(Window, DeviceContext);
                ++XOffset;
                YOffset += 2;
            }
        } else {
            // TODO(smzb): Log the fact that we couldn't retrieve a Window Handle
            // This is a critical failure.
        }
    }
    else
    {
        // TODO(smzb): Log the fact that we couldn't register a new WindowClass
        // This is a critical failure.
    }
    
    return(0);
}

