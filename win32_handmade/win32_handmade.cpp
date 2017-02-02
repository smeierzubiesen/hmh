// win32_handmade.cpp
// compiles with -doc -FC -Zi win32_handmade.cpp user32.lib gdi32.lib
// see: build.bat for switches used in compilation
/* ========================================================================
$File: $
$Date: $
$Revision: 0.1.d4.b(build#) $
$Creator: Sebastian Meier zu Biesen $
$Notice: (C) Copyright 2000-2016 by Joker Solutions, All Rights Reserved. $
======================================================================== */

#include <Windows.h>
#include <stdint.h>
#include "win32_handmade.h"

global_variable bool Running;
global_variable bool Debug = 0;
global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;

global_variable int BytesPerPixel = 4;
global_variable int BitmapWidth;
global_variable int BitmapHeight;

internal void Win32RenderGradient(int XOffset, int YOffset) {
	int Pitch = BitmapWidth*BytesPerPixel;
	uint8 *Row = (uint8 *)BitmapMemory;
	for (int Y = 0; Y < BitmapHeight; ++Y) {
		uint8 *Pixel = (uint8 *)Row;
		for (int X = 0; X < BitmapWidth; ++X) {
			*Pixel = (uint8)(Y + YOffset);
			++Pixel;
			*Pixel = XOffset * YOffset;
			++Pixel;
			*Pixel = (uint8)(X + XOffset);
			++Pixel;
			*Pixel = 0;
			++Pixel;
		}
		Row += Pitch;
	}
}

internal void Win32ResizeDIBSection(int Width, int Height) {
	if (BitmapMemory) {
		VirtualFree(BitmapMemory,0,MEM_RELEASE);
	}
	BitmapWidth = Width;
	BitmapHeight = Height;
	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = BitmapWidth;
	BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

	int BitmapMemorySize = (BitmapWidth*BitmapHeight)*BytesPerPixel;
	BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal void Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int X, int Y, int Width, int Height) {
	int WindowWidth = WindowRect->right - WindowRect->left;
	int WindowHeight = WindowRect->bottom - WindowRect->top;
	StretchDIBits(DeviceContext, 0, 0, BitmapWidth, BitmapHeight, 0, 0, WindowWidth, WindowHeight, BitmapMemory, &BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
	LRESULT Result = 0;
	switch (Message) {
		case WM_SIZE:
		{
			RECT WindowRect;
			GetClientRect(Window,&WindowRect);
			int Width = WindowRect.right - WindowRect.left;
			int Height = WindowRect.bottom - WindowRect.top;
			Win32ResizeDIBSection(Width, Height);
			if (Debug) { OutputDebugStringA("WM_SIZE\n"); }
		} break;
		case WM_DESTROY:
		{
			Running = false;
			if (Debug) { OutputDebugStringA("WM_DESTROY\n"); }
		} break;
		case WM_CLOSE:
		{
			Running = false;
			if (Debug) { OutputDebugStringA("WM_CLOSE\n"); }
		} break;
		case WM_QUIT:
		{
			if (Debug) { OutputDebugStringA("WM_QUIT\n"); }
		} break;
		case WM_ACTIVATEAPP:
		{
			if (Debug) { OutputDebugStringA("WM_ACTIVATEAPP\n"); }
		} break;
		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.top;
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;
			int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			RECT WindowRect;
			GetClientRect(Window, &WindowRect);
			Win32UpdateWindow(DeviceContext, &WindowRect, X, Y, Width, Height);
			EndPaint(Window,&Paint);
			if (Debug) { OutputDebugStringA("WM_PAINT\n"); }
		} break;
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
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
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
			int XOffset = 0;
			int YOffset = 0;
			Running = true;
			while(Running)
			{
				MSG Message;
				while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) {
					if (Message.message == WM_QUIT) {
						Running = false;
					}
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				Win32RenderGradient(XOffset, YOffset);
				
				HDC DeviceContext = GetDC(WindowHandle);
				RECT WindowRect;
				GetClientRect(WindowHandle, &WindowRect);
				int WindowWidth = WindowRect.right - WindowRect.left;
				int WindowHeight = WindowRect.bottom - WindowRect.top;
				Win32UpdateWindow(DeviceContext,&WindowRect,0,0,WindowWidth,WindowHeight);
				ReleaseDC(WindowHandle, DeviceContext);
				++XOffset;
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
	if (Debug) { MessageBox(0, "This is Handmade Hero", "Handmade Hero v0.1", MB_OK | MB_ICONINFORMATION); }
	return 0;
}
