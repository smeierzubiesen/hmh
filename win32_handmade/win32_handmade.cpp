// win32_handmade.cpp
// compiles with -doc -FC -Zi win32_handmade.cpp user32.lib gdi32.lib
// see: build.bat for switches used in compilation
/* ========================================================================
$File: $
$Date: $
$Revision: 0.1.d5 $
$Creator: Sebastian Meier zu Biesen $
$Notice: (C) Copyright 2000-2016 by Joker Solutions, All Rights Reserved. $
======================================================================== */

#include <Windows.h>
#include <stdint.h>
#include <Xinput.h>
#include "win32_handmade.h"

win32_window_dimensions Win32GetWindowDimensions(HWND WindowHandle) {
	win32_window_dimensions Result;
	RECT WindowRect;
	GetClientRect(WindowHandle, &WindowRect);
	Result.Width = WindowRect.right - WindowRect.left;
	Result.Height = WindowRect.bottom - WindowRect.top;
	return Result;
}

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
	Buffer->BytesPerPixel = 4;
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

internal void Win32DisplayBufferInWindow(HDC DeviceContext, int WindowWidth, int WindowHeight, win32_offscreen_buffer Buffer, int X, int Y, int Width, int Height) {
	StretchDIBits(DeviceContext, 0, 0, WindowWidth, WindowHeight, 0, 0, Buffer.Width, Buffer.Height, Buffer.Memory, &Buffer.Info, DIB_RGB_COLORS, SRCCOPY);
}

/// <summary>
/// This guy deals with the messages from the PeekMessage() function in WinMain().
/// </summary>
/// <param name="Window"></param>
/// <param name="Message"></param>
/// <param name="WParam"></param>
/// <param name="LParam"></param>
/// <returns></returns>
LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
	LRESULT Result = 0;
	switch (Message) {
		case WM_SIZE:
		{
			if (Debug) { OutputDebugStringA("WM_SIZE\n"); }
		} break;
		case WM_DESTROY:
		{
			GlobalRunning = false;
			if (Debug) { OutputDebugStringA("WM_DESTROY\n"); }
		} break;
		case WM_CLOSE:
		{
			GlobalRunning = false;
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
			win32_window_dimensions Dimensions = Win32GetWindowDimensions(Window);
			Win32DisplayBufferInWindow(DeviceContext, Dimensions.Width, Dimensions.Height, GlobalBackBuffer , X, Y, Width, Height);
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

/// <summary>
/// This is the main window call for windows as entry point to the app.
/// </summary>
/// <param name="Instance"></param>
/// <param name="hPrevInstance"></param>
/// <param name="CommandLine"></param>
/// <param name="ShowCode"></param>
/// <returns></returns>
int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE hPrevInstance, LPSTR CommandLine, int ShowCode) {
	WNDCLASS WindowClass = {};
	Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);
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
			GlobalRunning = true;
			while(GlobalRunning)
			{
				MSG Message;
				while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) {
					if (Message.message == WM_QUIT) {
						GlobalRunning = false;
					}
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				DWORD dwResult;
				for (DWORD ControllerIndex = 0; ControllerIndex< XUSER_MAX_COUNT; ++ControllerIndex) {
					XINPUT_STATE ControllerState;
					ZeroMemory(&ControllerState, sizeof(XINPUT_STATE));

					// Simply get the state of the controller from XInput.
					dwResult = XInputGetState(ControllerIndex, &ControllerState);

					if (dwResult == ERROR_SUCCESS)
					{
						// Controller is connected 
						XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;
						bool Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
						bool Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
						bool Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
						bool Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
						bool Start = (Pad->wButtons & XINPUT_GAMEPAD_START);
						bool Back = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
						bool LeftShoulder = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
						bool RightShoulder = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
						bool AButton = (Pad->wButtons & XINPUT_GAMEPAD_A);
						bool BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
						bool XButton = (Pad->wButtons & XINPUT_GAMEPAD_X);
						bool YButton = (Pad->wButtons & XINPUT_GAMEPAD_Y);
						int16 StickX = Pad->sThumbLX;
						int16 StickY = Pad->sThumbLY;
					}
					else
					{
						// Controller is not connected 
					}
				}

				Win32RenderGradient(GlobalBackBuffer, XOffset, YOffset);
				
				HDC DeviceContext = GetDC(WindowHandle);
				win32_window_dimensions Dimensions = Win32GetWindowDimensions(WindowHandle);
				Win32DisplayBufferInWindow(DeviceContext, Dimensions.Width, Dimensions.Height, GlobalBackBuffer, 0, 0, Dimensions.Width, Dimensions.Height);
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
