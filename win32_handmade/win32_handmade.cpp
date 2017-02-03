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

struct win32_offscreen_buffer {
	BITMAPINFO Info;
	void *Memory;
	int BytesPerPixel = 4;
	int Width;
	int Height;
	int Pitch;
};

global_variable bool Running;
global_variable bool Debug = 0;
global_variable win32_offscreen_buffer GlobalBackBuffer;

internal void Win32RenderGradient(win32_offscreen_buffer Buffer, int XOffset, int YOffset) {
	uint8 *Row = (uint8 *)Buffer.Memory;
	for (int Y = 0; Y < Buffer.Height; ++Y) {
		uint32 *Pixel = (uint32 *)Row;
		for (int X = 0; X < Buffer.Width; ++X) {
			uint8 Blue = (X + XOffset);
			uint8 Green = (Y + YOffset);
			uint8 Red = (X + XOffset);
			*Pixel++ = ((Red << 16) | (Green << 8) | (Blue));
		}
		Row += Buffer.Pitch;
	}
}

internal void Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height) {
	if (Buffer->Memory) {
		VirtualFree(Buffer->Memory,0,MEM_RELEASE);
	}
	Buffer->Width = Width;
	Buffer->Height = Height;
	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;

	int BitmapMemorySize = (Buffer->Width*Buffer->Height)*Buffer->BytesPerPixel;
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
	Buffer->Pitch = Buffer->Width*Buffer->BytesPerPixel;
}

internal void Win32DisplayBufferInWindow(HDC DeviceContext, RECT WindowRect, win32_offscreen_buffer Buffer, int X, int Y, int Width, int Height) {
	int WindowWidth = WindowRect.right - WindowRect.left;
	int WindowHeight = WindowRect.bottom - WindowRect.top;
	StretchDIBits(DeviceContext, 0, 0, Buffer.Width, Buffer.Height, 0, 0, WindowWidth, WindowHeight, Buffer.Memory, &Buffer.Info, DIB_RGB_COLORS, SRCCOPY);
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
			Win32ResizeDIBSection(&GlobalBackBuffer, Width, Height);
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
			Win32DisplayBufferInWindow(DeviceContext, WindowRect, GlobalBackBuffer , X, Y, Width, Height);
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
	WindowClass.style = CS_HREDRAW|CS_VREDRAW;
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
				Win32RenderGradient(GlobalBackBuffer, XOffset, YOffset);
				
				HDC DeviceContext = GetDC(WindowHandle);
				RECT WindowRect;
				GetClientRect(WindowHandle, &WindowRect);
				int WindowWidth = WindowRect.right - WindowRect.left;
				int WindowHeight = WindowRect.bottom - WindowRect.top;
				Win32DisplayBufferInWindow(DeviceContext, WindowRect, GlobalBackBuffer, 0, 0, WindowWidth, WindowHeight);
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
