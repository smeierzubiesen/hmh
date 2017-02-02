// win32_handmade.cpp
// compiles with -doc -FC -Zi win32_handmade.cpp user32.lib gdi32.lib
// see: build.bat for switches used in compilation
/* ========================================================================
$File: $
$Date: $
$Revision: 0.1.d2.b(build#) $
$Creator: Sebastian Meier zu Biesen $
$Notice: (C) Copyright 2000-2016 by Joker Solutions, All Rights Reserved. $
======================================================================== */

#include "win32_handmade.h"
#include <Windows.h>

LRESULT CALLBACK MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
	LRESULT Result = 0;
	switch (Message) {
		case WM_SIZE:
		{
			OutputDebugStringA("WM_SIZE\n");
		} break;
		case WM_DESTROY:
		{
			OutputDebugStringA("WM_DESTROY\n");
		} break;
		case WM_CLOSE:
		{
			OutputDebugStringA("WM_CLOSE\n");
			//PostQuitMessage(0);
			//TODO(smzb): this is probably not the way to do it, but it works.
		} break;
		case WM_QUIT:
		{
			OutputDebugStringA("WM_QUIT\n");
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
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;
			int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			static DWORD Operation = WHITENESS;
			PatBlt(DeviceContext, X, Y, Width, Height, Operation);
			if (Operation == WHITENESS) {
				Operation = BLACKNESS;
			}
			else {
				Operation = WHITENESS;
			}

			EndPaint(Window,&Paint);
			OutputDebugStringA("WM_PAINT\n");
		}
		default:
		{
			Result = DefWindowProc(Window,Message,WParam,LParam);
			//OutputDebugStringA("default\n");
		} break;
	}
	
	return(Result);
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE hPrevInstance, LPSTR CommandLine, int ShowCode) {
	WNDCLASS WindowClass = {};
	WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
	WindowClass.lpfnWndProc = MainWindowCallback;
	WindowClass.hInstance = Instance;
	//WindowClass.hIcon = ;
	//WindowClass.hCursor = ;
	WindowClass.lpszClassName = "hmhWindowClass";
	if (RegisterClass(&WindowClass))
	{
		HWND WindowHandle = CreateWindowEx(
			0,
			WindowClass.lpszClassName,
			"Handmade Hero v0.1",
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
			for (;;)
			{
				MSG Message;
				BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
				if (MessageResult > 0) {
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				else {
					break;
				}
			}
			
		}
		else
		{
			//TODO(smzb): Log this event
		}
	}
	else {
		//TODO(smzb): log this event
	}
	MessageBox(0, "This is Handmade Hero", "Handmade Hero v0.1", MB_OK | MB_ICONINFORMATION);
	return 0;
}
