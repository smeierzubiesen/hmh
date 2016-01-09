/*
// TODO(smzb): Add a better comment template for new .cpp and .h files
*/

#include <windows.h>

#define internal static
#define local_persist static
#define global_variable static

global_variable bool Running; // TODO(smzb): This should not be a global in future.
global_variable BITMAPINFO BitmapInfo; // TODO(smzb): This should not be a global in future.
global_variable void *BitmapMemory; // TODO(smzb): This should not be a global in future.
global_variable HBITMAP BitmapHandle; // TODO(smzb): This should not be a global in future.
global_variable HDC BitmapDeviceContext; // TODO(smzb): This should not be a global in future.

internal void
Win32ResizeDIBSection(int Width, int Height)
{
    // TODO(smzb): Bulletproof this : Free after if possible, if not free first
    // Free after if possible, if not free first

    if (BitmapHandle)
    {
        DeleteObject(BitmapHandle);
    }

    if (!BitmapDeviceContext)
    {
        BitmapDeviceContext = CreateCompatibleDC(0);
    }

    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = Width;
    BitmapInfo.bmiHeader.biHeight = Height;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    BitmapHandle = CreateDIBSection(BitmapDeviceContext,
                                    &BitmapInfo,
                                    DIB_RGB_COLORS,
                                    &BitmapMemory,
                                    NULL, NULL);
}

internal void Win32UpdateWindow(HDC DeviceContext, int X, int Y, int W, int H)
{
    StretchDIBits(DeviceContext,
                  X, Y, W, H,
                  X, Y, W, H,
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
			Win32UpdateWindow(DeviceContext, X, Y, W, H);
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

