// win32_handmade.cpp
// compiles with -doc -FC -Zi win32_handmade.cpp user32.lib gdi32.lib
// see: build.bat for switches used in compilation
/* ========================================================================
$File: $
$Date: $
$Revision: 0.1.d9 $
$Creator: Sebastian Meier zu Biesen $
$Notice: (C) Copyright 2000-2016 by Joker Solutions, All Rights Reserved. $
======================================================================== */

#include <Windows.h>
#include <stdint.h>
#include <Xinput.h>
#include <dsound.h>
#include "win32_handmade.h"

/// <summary>
/// Load the XInput library for XBox Xontroller Support. Depending on OS version either 1.3 or 1.4 is loaded.
/// </summary>
/// <see cref="X_INPUT_GET_STATE"/>
/// <see cref="X_INPUT_SET_STATE"/>
/// <returns>void</returns>
internal void Win32LoadXInput(void) {
	HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
	if (!XInputLibrary) {
		HMODULE XInputLibrary = LoadLibraryA("xinput1_3.dll");
	}
	if (XInputLibrary) {
		XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
		XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
	}
}

/// <summary>
/// PROTOTYPE: This function will load and initialize DirectSound to output sound to the default sound output channel.
/// This should however still allow to run the game in the rare event, that DirectSound is not available.
/// </summary>
/// <returns></returns>
internal void Win32InitDirectSound(HWND WindowHandle, int32 SamplesPerSecond, int32 BufferSize) {
	//NOTE(smzb): Load DirectSound
	HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
	if (DSoundLibrary)
	{
		//NOTE(smzb): Get a a DirectSound Object
		direct_sound_create *DirectSoundCreate = (direct_sound_create *)GetProcAddress(DSoundLibrary, "DirectSoundCreate");
		LPDIRECTSOUND DirectSound;
		if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound,0))) {
			WAVEFORMATEX WaveFormat = {};
			WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
			WaveFormat.nChannels = 2;
			WaveFormat.wBitsPerSample = 16;
			WaveFormat.nSamplesPerSec = SamplesPerSecond;
			WaveFormat.nBlockAlign = (WaveFormat.nChannels*WaveFormat.wBitsPerSample) / 8;
			WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec*WaveFormat.nBlockAlign;
			WaveFormat.cbSize = 0;
			if (SUCCEEDED(DirectSound->SetCooperativeLevel(WindowHandle, DSSCL_PRIORITY))) {
				//NOTE(smzb): "Create" a primary buffer
				DSBUFFERDESC BufferDescription = {};
				BufferDescription.dwSize = sizeof(BufferDescription);
				BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
				LPDIRECTSOUNDBUFFER PrimaryBuffer;
				if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0))) {


					HRESULT Result = PrimaryBuffer->SetFormat(&WaveFormat);
					//BufferDescription.dwSize;
					if (SUCCEEDED(Result)) {
						//NOTE(smzb): Finally the format of the sound is set
						if (Debug) { OutputDebugStringA("Primary Buffer created!\n"); }
					}
					else {
						//TODO(smzb): Diagnostic Log here
					}
				}
				else {
					// (TODO(smzb): Diagnostic
				}
				
			}
			else {
				//TODO(smzb): DirectSound Diagnostic
			}
			//NOTE(smzb): "Create" a secondary buffer
			DSBUFFERDESC BufferDescription = {};
			BufferDescription.dwSize = sizeof(BufferDescription);
			BufferDescription.dwFlags = 0;
			BufferDescription.dwBufferBytes = BufferSize;
			BufferDescription.lpwfxFormat = &WaveFormat;
			HRESULT Result = DirectSound->CreateSoundBuffer(&BufferDescription, &GlobalSecondaryBuffer, 0);
			if (SUCCEEDED(Result)) {
				if (Debug) { OutputDebugStringA("Secondary Buffer created!"); }
					//NOTE(smzb): Start playing
			}
			else {
				//TODO(smzb): Diagnostic
			}
		}
		else {
			//TODO(smzb): Diagnostic Feedback
		}
		
	}
}

/// <summary>
/// Calculate Width and Height of the main window by subtracting right and left & bottom - top
/// </summary>
/// <param name="WindowHandle">The Window Handle to calculate</param>
/// <returns>struct win32_window_dimensions</returns>
internal win32_window_dimensions Win32GetWindowDimensions(HWND WindowHandle) {
	win32_window_dimensions Result;
	RECT WindowRect;
	GetClientRect(WindowHandle, &WindowRect);
	Result.Width = WindowRect.right - WindowRect.left;
	Result.Height = WindowRect.bottom - WindowRect.top;
	return Result;
}

/// <summary>
/// This is just a dummy function to display something on the screen after we have assigned memory to the bitmap
/// </summary>
/// <param name="Buffer">Bitmap Backbuffer to us (pointer to)</param>
/// <param name="XOffset">X Offset to start Moevement of the bitmap</param>
/// <param name="YOffset">X Offset to start Moevement of the bitmap</param>
/// <returns>void</returns>
internal void Win32RenderGradient(win32_offscreen_buffer *Buffer, int XOffset, int YOffset) {
	uint8 *Row = (uint8 *)Buffer->Memory;
	for (int Y = 0; Y < Buffer->Height; ++Y) {
		uint32 *Pixel = (uint32 *)Row;
		for (int X = 0; X < Buffer->Width; ++X) {
			uint8 Blue = (X + XOffset);
			uint8 Green = (Y + YOffset);
			uint8 Red = (X + XOffset);
			*Pixel++ = ((Red << 16) | (Green << 8) | (Blue));
		}
		Row += Buffer->Pitch;
	}
}

/// <summary>
/// This re/creates a DeviceIndapendentBitmap Bufferand calculates the necessary memory allocation. 
/// </summary>
/// <param name="Buffer">Pointer to the BitmapBuffer</param>
/// <param name="Width">Width of the Bitmap Buffer</param>
/// <param name="Height">Height of the Bitmap Buffer</param>
/// <returns>void</returns>
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

/// <summary>
/// This function will blt the bitmap to the window defined in the below parameters.
/// </summary>
/// <param name="Buffer">Which bitmap buffer are we dealing with.</param>
/// <param name="DeviceContext">Hardware Device Context (passed from WinMain()</param>
/// <param name="WindowWidth">Integer Window Width</param>
/// <param name="WindowHeight">Integer Window Height</param>
/// <param name="X">X Coordinate for origin of bitmap</param>
/// <param name="Y">y Coordinate for origin of bitmap</param>
/// <param name="Width">Bitmap Width</param>
/// <param name="Height">Bitmap Height</param>
/// <returns>void</returns>
internal void Win32DisplayBufferInWindow(win32_offscreen_buffer *Buffer, HDC DeviceContext, int WindowWidth, int WindowHeight, int X, int Y, int Width, int Height) {
	StretchDIBits(DeviceContext, 0, 0, WindowWidth, WindowHeight, 0, 0, Buffer->Width, Buffer->Height, Buffer->Memory, &Buffer->Info, DIB_RGB_COLORS, SRCCOPY);
}

/// <summary>
/// This guy deals with the messages from the PeekMessage() function in WinMain().
/// </summary>
/// <param name="Window">Window Handle to the messenger</param>
/// <param name="Message">Content of the message passed from windows to us</param>
/// <param name="WParam">Parameter/function passed to us</param>
/// <param name="LParam">Low Parameter for above function call</param>
/// <returns>LResult integer</returns>
internal LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
	LRESULT Result = 0;
	switch (Message) {
		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_KEYDOWN:
		{
			uint32 VKCode = WParam;
			bool WasDown = ((LParam & (1 << 30)) != 0);
			bool IsDown = ((LParam & (1 << 31)) == 0);
			if (WasDown != IsDown) {
				if (VKCode == 'A')
				{
					if (Debug) { 
						OutputDebugStringA("A: "); 
						if (IsDown) {
							OutputDebugStringA("is down");
						}
						if (WasDown) {
							OutputDebugStringA("was down");
						}
						OutputDebugStringA("\n");
					}
				}
				else if (VKCode == 'S')
				{
					if (Debug) {
						OutputDebugStringA("S: ");
						if (IsDown) {
							OutputDebugStringA("is down");
						}
						if (WasDown) {
							OutputDebugStringA("was down");
						}
						OutputDebugStringA("\n");
					}
				}
				else if (VKCode == 'W')
				{
					if (Debug) {
						OutputDebugStringA("W: ");
						if (IsDown) {
							OutputDebugStringA("is down");
						}
						if (WasDown) {
							OutputDebugStringA("was down");
						}
						OutputDebugStringA("\n");
					}
				}
				else if (VKCode == 'D')
				{
					if (Debug) {
						OutputDebugStringA("D: ");
						if (IsDown) {
							OutputDebugStringA("is down");
						}
						if (WasDown) {
							OutputDebugStringA("was down");
						}
						OutputDebugStringA("\n");
					}
				}
				else if (VKCode == 'Q')
				{
					if (Debug) {
						OutputDebugStringA("Q: ");
						if (IsDown) {
							OutputDebugStringA("is down");
						}
						if (WasDown) {
							OutputDebugStringA("was down");
						}
						OutputDebugStringA("\n");
					}
				}
				else if (VKCode == 'E')
				{
					if (Debug) {
						OutputDebugStringA("E: ");
						if (IsDown) {
							OutputDebugStringA("is down");
						}
						if (WasDown) {
							OutputDebugStringA("was down");
						}
						OutputDebugStringA("\n");
					}
				}
				else if (VKCode == VK_UP)
				{
					if (Debug) {
						OutputDebugStringA("Up: ");
						if (IsDown) {
							OutputDebugStringA("is down");
						}
						if (WasDown) {
							OutputDebugStringA("was down");
						}
						OutputDebugStringA("\n");
					}
				}
				else if (VKCode == VK_DOWN)
				{
					if (Debug) {
						OutputDebugStringA("Down: ");
						if (IsDown) {
							OutputDebugStringA("is down");
						}
						if (WasDown) {
							OutputDebugStringA("was down");
						}
						OutputDebugStringA("\n");
					}
				}
				else if (VKCode == VK_LEFT)
				{
					if (Debug) {
						OutputDebugStringA("Left: ");
						if (IsDown) {
							OutputDebugStringA("is down");
						}
						if (WasDown) {
							OutputDebugStringA("was down");
						}
						OutputDebugStringA("\n");
					}
				}
				else if (VKCode == VK_RIGHT)
				{
					if (Debug) {
						OutputDebugStringA("Right: ");
						if (IsDown) {
							OutputDebugStringA("is down");
						}
						if (WasDown) {
							OutputDebugStringA("was down");
						}
						OutputDebugStringA("\n");
					}
				}
				else if (VKCode == VK_SPACE)
				{
					if (Debug) {
						OutputDebugStringA("Space: ");
						if (IsDown) {
							OutputDebugStringA("is down");
						}
						if (WasDown) {
							OutputDebugStringA("was down");
						}
						OutputDebugStringA("\n");
					}
				}
				else if (VKCode == VK_ESCAPE)
				{
					if (Debug) {
						OutputDebugStringA("Escape: ");
						if (IsDown) {
							OutputDebugStringA("is down");
						}
						if (WasDown) {
							OutputDebugStringA("was down");
						}
						OutputDebugStringA("\n");
					}
				}
			}
			bool32 AltKeyWasDown = (LParam & (1 << 29));
			if ((VKCode == VK_F4) && AltKeyWasDown) {
				GlobalRunning = false;
			}
		} break;
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
			Win32DisplayBufferInWindow(&GlobalBackBuffer, DeviceContext, Dimensions.Width, Dimensions.Height, X, Y, Width, Height);
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
/// <param name="Instance">A handle to the current instance of the application.</param>
/// <param name="hPrevInstance">A handle to the previous instance of the application. This parameter is always NULL.</param>
/// <param name="CommandLine">The command line for the application, excluding the program name. To retrieve the entire command line, use the GetCommandLine function.</param>
/// <param name="ShowCode">Controls how the window is to be shown.</param>
/// <returns>If the function succeeds, terminating when it receives a WM_QUIT message, it should return the exit value contained in that message's wParam parameter. If the function terminates before entering the message loop, it should return zero.</returns>
internal int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE hPrevInstance, LPSTR CommandLine, int ShowCode) {
	Win32LoadXInput();
	WNDCLASSA WindowClass = {};
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
			//NOTE(smzb): Graphics Test stuff
			int XOffset = 0;
			int YOffset = 0;

			//NOTE(smzb): Sound stuff setup
			int SamplesPerSecond = 48000;
			int ToneHz = 256;
			int16 ToneVolume = 500;
			uint32 RunningSampleIndex = 0;
			int SquareWavePeriod =SamplesPerSecond/ToneHz;
			int HalfSquareWavePeriod = SquareWavePeriod / 2;
			int BytesPerSample = sizeof(int16) * 2;
			int SecondaryBufferSize = SamplesPerSecond * BytesPerSample;
			Win32InitDirectSound(WindowHandle, SamplesPerSecond, SecondaryBufferSize);
			GlobalSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
			//NOTE(smzb): The actual program loop
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

						XINPUT_VIBRATION Vibration;
						Vibration.wLeftMotorSpeed = 0;
						Vibration.wRightMotorSpeed = 0;
						if (AButton) {
							YOffset += 2;
							Vibration.wLeftMotorSpeed = 60000;
							XInputSetState(ControllerIndex, &Vibration);
						}
						if (BButton) {
							YOffset -= 2;
							Vibration.wRightMotorSpeed = 60000;
							XInputSetState(ControllerIndex, &Vibration);
						}
						XInputSetState(ControllerIndex, &Vibration);

					}
					else
					{
						// Controller is not connected 
						//NOTE(smzb): In this case the Stub functions for xinputset/getstate should return the correct value;
					}
				}

				Win32RenderGradient(&GlobalBackBuffer, XOffset, YOffset);
				
				DWORD PlayCursorPosition;
				DWORD WriteCursorPosition;
				if (SUCCEEDED(GlobalSecondaryBuffer->GetCurrentPosition(&PlayCursorPosition,&WriteCursorPosition)))
				{
					//NOTE(smzb): DirectSound output test
					DWORD ByteToLock = RunningSampleIndex*BytesPerSample % SecondaryBufferSize;
					DWORD BytesToWrite;
					if (ByteToLock > PlayCursorPosition) {
						BytesToWrite = SecondaryBufferSize - ByteToLock;
						BytesToWrite += PlayCursorPosition;
					}
					else {
						BytesToWrite = PlayCursorPosition - ByteToLock;
					}
					VOID *Region1;
					DWORD Region1Size;
					VOID *Region2;
					DWORD Region2Size;
					
					if (SUCCEEDED(GlobalSecondaryBuffer->Lock(ByteToLock, BytesToWrite, &Region1, &Region1Size, &Region2, &Region2Size, 0)))
					{
						//TODO(smzb): assert to ensure that sample (Region1Size/Region2Size) is correct size
						DWORD Region1SampleCount = Region1Size / BytesPerSample;
						int16 *SampleOut = (int16 *)Region1;
						for (DWORD SampleIndex = 0; SampleIndex < Region1SampleCount; ++SampleIndex) {
							int16 SampleValue = ((RunningSampleIndex++ / HalfSquareWavePeriod) % 2) ? ToneVolume : -ToneVolume;
							*SampleOut++ = SampleValue;
							*SampleOut++ = SampleValue;
						}
						DWORD Region2SampleCount = Region2Size / BytesPerSample;
						SampleOut = (int16 *)Region2;
						for (DWORD SampleIndex = 0; SampleIndex < Region2SampleCount; ++SampleIndex) {
							int16 SampleValue = ((RunningSampleIndex++ / HalfSquareWavePeriod) % 2) ? ToneVolume : -ToneVolume;
							*SampleOut++ = SampleValue;
							*SampleOut++ = SampleValue;
						}
						GlobalSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
					}
				}

				HDC DeviceContext = GetDC(WindowHandle);
				win32_window_dimensions Dimensions = Win32GetWindowDimensions(WindowHandle);
				Win32DisplayBufferInWindow(&GlobalBackBuffer, DeviceContext, Dimensions.Width, Dimensions.Height, 0, 0, Dimensions.Width, Dimensions.Height);
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
