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

global_variable bool Running; // TODO(smzb): This should not be a global in future.

global_variable BITMAPINFO BitmapInfo; // TODO(smzb): This should not be a global in future.
global_variable void *BitmapMemory; // TODO(smzb): This should not be a global in future.
global_variable int BitmapWidth;
global_variable int BitmapHeight;


internal void
Win32ResizeDIBSection(int Width, int Height)
{
    if(BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapWidth = Width;
    BitmapHeight = Height;
    
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    int BytesPerPixel = 4;
    int BitmapMemorySize = (BitmapWidth*BitmapHeight)*BytesPerPixel;
    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    int Pitch = Width*BytesPerPixel;
    uint8 *Row = (uint8 *)BitmapMemory;
    for(int Y = 0; Y < BitmapHeight; Y++)
    {
        uint8 *Pixel = (uint8 *)Row;
        for(int X = 0; X < BitmapWidth; X++)
        {
			/*
			00 11 22 33
			BB GG RR xx
			*/
            *Pixel = (uint8)X;
            ++Pixel;

            *Pixel = (uint8)Y;
            ++Pixel;

            *Pixel = 0;
            ++Pixel;

            *Pixel = 0;
            ++Pixel;
        }

        Row += Pitch;
    }
}

internal void Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int X, int Y, int W, int H)
{
    int WindowWidth = WindowRect->right - WindowRect->left;
    int WindowHeight = WindowRect->bottom - WindowRect->top;
    StretchDIBits(DeviceContext,
                  /*X, Y, W, H,
                    X, Y, W, H,*/
                  0, 0, BitmapWidth, BitmapHeight,
                  0, 0, WindowWidth, WindowHeight,
                  BitmapMemory,
                  &BitmapInfo,
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
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            int W = ClientRect.right - ClientRect.left;
            int H = ClientRect.bottom - ClientRect.top;
            Win32ResizeDIBSection(W, H);
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

            RECT ClientRect;
            GetClientRect(Window, &ClientRect);

            Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, W, H);
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
    //WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW; // TODO(smzb): Check wether HREDRAW and VREDRAW are still necessary [confirmed] setting this to 0 didn't work, will try just CS_OWNDC
    WindowClass.style = CS_OWNDC;
    WindowClass.lpfnWndProc = Win32MainWindowCallBack;
    WindowClass.hInstance = Instance;
//    WindowClass.hIcon; // TODO(smzb): Make an icon and stick it in here.
    WindowClass.lpszClassName = "HandmadeHeroWindowClass";

    if(RegisterClass(&WindowClass))
    {
        HWND WindowHandle = CreateWindowEx(0,
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
        if (WindowHandle)
        {
            Running = true;
            while(Running)
            {
                MSG Message;
                BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
                if (MessageResult > 0)
                {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                } else {
                    break;
                }
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

