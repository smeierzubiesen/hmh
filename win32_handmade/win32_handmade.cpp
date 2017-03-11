// win32_handmade.cpp
// compiles with -doc -FC -Zi win32_handmade.cpp user32.lib gdi32.lib
// see: build.bat for switches used in compilation
/* ========================================================================
$File: $
$Date: $
$Revision: 0.1.d13 $
$Creator: Sebastian Meier zu Biesen $
$Notice: (C) Copyright 2000-2016 by Joker Solutions, All Rights Reserved. $
======================================================================== */

/*
TODO(smzb): This is not a final win32 platform layer.
 - Saved Game locations
 - Getting a handle to the executable
 - Asset loading path(s)
 - Threading (how-to launch a thread)
 - Raw Input (support for multiple keyboards)
 - Sleep/time BeginPeriod
 - ClipCursor() (multi monitor support)
 - Full-screen support
 - WM_SETCURSOR (control cursor visibility)
 - QueryCancelAutoplay
 - WM_ACTIVATEAPP (for when we are not in focus)
 - Blit speed improvement
 - Hardware acceleration (OpenGL||Direct3D||both??)
 - GetKeyboardlayout() support (international keyboards)
`part list of stuff to be done`
*/

#include "handmade.hpp"
#include "win32_handmade.hpp"

/// <summary>
/// The Global Bitmap Buffer
/// </summary>
global_variable win32_offscreen_buffer GlobalBitmapBuffer;

/// <summary>
/// The DSound Buffer to which we actually write sound.
/// </summary>
/// <see cref="DIRECT_SOUND_CREATE"/>
global_variable LPDIRECTSOUNDBUFFER GlobalSoundBuffer;

#if !defined(WIN32_HANDMADE_X_INPUT)
#define WIN32_HANDMADE_X_INPUT
//NOTE(smzb): XInputGetState
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub) {
	return(ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

//NOTE(smzb): XInputSetState
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub) {
	return(ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_
#endif

//NOTE(smzb): DirectSoundCreate
#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);

/// <summary>
/// Formats the debug output for timing stuff (this is only enabled, when `Debug==true`
/// </summary>
/// <param name="ms">float: Milliseconds per frame.</param>
/// <param name="fps">float: Frames per second.</param>
/// <param name="MCyclesPerFrame">float: Millions of Cycles per second.</param>
/// <see cref="DIRECT_SOUND_CREATE"/>
void PrintDebugTime(real64 ms, real64 fps, real64 MCyclesPerFrame) {
	char Buffer[256];
	sprintf_s(Buffer, "[:: %.03f ms/frame ::][:: %.03f FPS ::][:: %.03f MegaCycles/frame ::]\n", ms, fps, MCyclesPerFrame);
	OutputDebugStringA(Buffer);
}

/// <summary>
/// Load the XInput library for XBox Controller Support. Depending on OS version either 1.3 or 1.4 is loaded.
/// </summary>
/// <see cref="X_INPUT_GET_STATE"/>
/// <see cref="X_INPUT_SET_STATE"/>
/// <returns>void</returns>
internal void Win32LoadXInput(void) {
	HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
	if (!XInputLibrary) {
		HMODULE XInputLibrary = LoadLibraryA("xinput1_3.dll");
	}
	if (!XInputLibrary) {
		HMODULE XInputLibrary = LoadLibraryA("xinput9_1_0.dll");
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
/// <param name="WindowHandle">A Handle to the MainWindow</param>
/// <param name="SamplesPerSecond">How many samples per second? (aka SampleRate)</param>
/// <param name="BufferSize">How big is our buffer? (int32) [default:1 second]</param>
/// <returns>void</returns>
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
					if (SUCCEEDED(Result)) {
						//NOTE(smzb): Finally the format of the sound is set
						if (Debug) { OutputDebugStringA("Primary Buffer created!\n"); }
					}
					else {
						//TODO(smzb): Diagnostic Log here
					}
				}
				else {
					//TODO(smzb): Diagnostic
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
			HRESULT Result = DirectSound->CreateSoundBuffer(&BufferDescription, &GlobalSoundBuffer, 0);
			if (SUCCEEDED(Result)) {
				if (Debug) { OutputDebugStringA("Secondary Buffer created!\n"); }
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


internal void Win32ClearSoundBuffer(win32_sound_output *SoundOutput) {
	VOID *Region1;
	DWORD Region1Size;
	VOID *Region2;
	DWORD Region2Size;
	if (SUCCEEDED(GlobalSoundBuffer->Lock(0, SoundOutput->SecondaryBufferSize, &Region1, &Region1Size, &Region2, &Region2Size, 0)))
	{
		uint8 *DestSample = (uint8 *)Region1;
		for (DWORD ByteIndex = 0; ByteIndex < Region1Size; ++ByteIndex) {
			*DestSample++ = 0;
		}
		DestSample = (uint8 *)Region2;
		for (DWORD ByteIndex = 0; ByteIndex < Region2Size; ++ByteIndex) {
			*DestSample++ = 0;
		}
		GlobalSoundBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
	}
}

/// <summary>
/// Fill the Sound-buffer with a Sine wave
/// </summary>
/// <param name="SoundOutput">Pointer to the struct describing the Sound Output</param>
/// <param name="ByteToLock">Where do we want to apply our direct-sound lock</param>
/// <param name="BytesToWrite">.. and how many bytes do we have to commit to memory?</param>
/// <returns>void</returns>
internal void Win32FillSoundBuffer(win32_sound_output *SoundOutput, game_sound_buffer *SourceBuffer, DWORD ByteToLock, DWORD BytesToWrite) {
	VOID *Region1;
	DWORD Region1Size;
	VOID *Region2;
	DWORD Region2Size;
	if (SUCCEEDED(GlobalSoundBuffer->Lock(ByteToLock, BytesToWrite, &Region1, &Region1Size, &Region2, &Region2Size, 0)))
	{
		//TODO(smzb): assert to ensure that sample (Region1Size/Region2Size) is correct size
		DWORD Region1SampleCount = Region1Size / SoundOutput->BytesPerSample;
		int16 *SourceSample = (int16 *)SourceBuffer->Samples;
		int16 *DestSample = (int16 *)Region1;
		for (DWORD SampleIndex = 0; SampleIndex < Region1SampleCount; ++SampleIndex) {
			*DestSample++ = *SourceSample++;
			*DestSample++ = *SourceSample++;
			++SoundOutput->RunningSampleIndex;
		}
		DWORD Region2SampleCount = Region2Size / SoundOutput->BytesPerSample;
		DestSample = (int16 *)Region2;
		for (DWORD SampleIndex = 0; SampleIndex < Region2SampleCount; ++SampleIndex) {
			*DestSample++ = *SourceSample++;
			*DestSample++ = *SourceSample++;
			++SoundOutput->RunningSampleIndex;
		}
		GlobalSoundBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
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
/// This re/creates a DeviceIndapendentBitmap Buffer and calculates the necessary memory allocation. 
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
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	Buffer->Pitch = Buffer->Width*Buffer->BytesPerPixel;
}

/// <summary>
/// This function will blit the bitmap to the window defined in the below parameters.
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
			bool32 WasDown = ((LParam & (1 << 30)) != 0);
			bool32 IsDown = ((LParam & (1 << 31)) == 0);
			if (WasDown != IsDown) {
				if (VKCode == 'A')
				{
					// Do something
				}
				else if (VKCode == 'S')
				{
					// Do something
				}
				else if (VKCode == 'W')
				{
					// Do something
				}
				else if (VKCode == 'D')
				{
					// Do something
				}
				else if (VKCode == 'Q')
				{
					// Do something
				}
				else if (VKCode == 'E')
				{
					// Do something
				}
				else if (VKCode == VK_UP)
				{
					// Do something
				}
				else if (VKCode == VK_DOWN)
				{
					// Do something
				}
				else if (VKCode == VK_LEFT)
				{
					// Do something
				}
				else if (VKCode == VK_RIGHT)
				{
					// Do something
				}
				else if (VKCode == VK_SPACE)
				{
					// Do something
				}
				else if (VKCode == VK_ESCAPE)
				{
					// Do something
				}
			}
			//NOTE(smzb): Deal with Alt+F4 by setting the running loop to false.
			bool32 AltKeyWasDown = (LParam & (1 << 29));
			if ((VKCode == VK_F4) && AltKeyWasDown) {
				GlobalRunning = false;
			}
		} break;
		case WM_SIZE:
		{
			// Do something
		} break;
		case WM_DESTROY:
		{
			GlobalRunning = false;
		} break;
		case WM_CLOSE:
		{
			GlobalRunning = false;
		} break;
		case WM_QUIT:
		{
			// Do something
		} break;
		case WM_ACTIVATEAPP:
		{
			// Do something
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
			Win32DisplayBufferInWindow(&GlobalBitmapBuffer, DeviceContext, Dimensions.Width, Dimensions.Height, X, Y, Width, Height);
			EndPaint(Window,&Paint);
		} break;
		default:
		{
			Result = DefWindowProcA(Window,Message,WParam,LParam);
		} break;
	}
	return(Result);
}

internal void Win32ProcessXInputButton(DWORD XInputButtonState, game_button_state *OldState, game_button_state *NewState, DWORD ButtonBit) {
	NewState->EndedDown = ((XInputButtonState & ButtonBit) == ButtonBit);
	NewState->HalfTransitionCount = (OldState->EndedDown != NewState->EndedDown) ? 1 : 0;
}

/// <summary>
/// This is the main window call for windows as entry point to the app.
/// </summary>
/// <param name="Instance">A handle to the current instance of the application.</param>
/// <param name="hPrevInstance">A handle to the previous instance of the application. This parameter is always NULL.</param>
/// <param name="CommandLine">The command line for the application, excluding the program name. To retrieve the entire command line, use the GetCommandLine function.</param>
/// <param name="ShowCode">Controls how the window is to be shown.</param>
/// <returns>If the function succeeds, terminating when it receives a WM_QUIT message, it should return the exit value contained in that message's wParam parameter. If the function terminates before entering the message loop, it should return zero.</returns>
int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE hPrevInstance, LPSTR CommandLine, int ShowCode) 
{
	//NOTE(smzb): Timing Stuff
	LARGE_INTEGER PerfCounterFrequencyResult;
	QueryPerformanceFrequency(&PerfCounterFrequencyResult);
	int64 PerfCounterFrequency = PerfCounterFrequencyResult.QuadPart;
	uint64 LastCycleCount = __rdtsc();

	//NOTE(smzb): Init of I/O and window.
	Win32LoadXInput();
	WNDCLASSA WindowClass = {};
	Win32ResizeDIBSection(&GlobalBitmapBuffer, 640, 480);
	WindowClass.style = CS_HREDRAW|CS_VREDRAW;
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = Instance;
	//WindowClass.hIcon = ;
	//WindowClass.hCursor = ;
	WindowClass.lpszClassName = "hmhWindow";
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
			GlobalRunning = true; //NOTE(smzb): This is the state of the game in 0 or 1, simple
			//NOTE(smzb): Timing Variable
			LARGE_INTEGER LastCounter;
			QueryPerformanceCounter(&LastCounter);
			//NOTE(smzb): Sound stuff setup
			win32_sound_output SoundOutput = {};
			SoundOutput.SamplesPerSecond = 44100; //NOTE(smzb): Sample rate of Output
			SoundOutput.ToneVolume = 5000; //NOTE(smzb): The volume of output
			SoundOutput.RunningSampleIndex = 0; //NOTE(smzb): Counter used in Square-wave/Sine-wave functions
			SoundOutput.BytesPerSample = sizeof(int16) * 2; //NOTE(smzb): How many bytes do we need per sample (L/R * 16bytes)
			SoundOutput.SecondaryBufferSize = SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample; // The Buffer size to actually generate sound in.
			SoundOutput.LatencySampleCount = SoundOutput.SamplesPerSecond / 15;
			int16 *Samples = (int16 *)VirtualAlloc(0, SoundOutput.SecondaryBufferSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
			Win32InitDirectSound(WindowHandle, SoundOutput.SamplesPerSecond, SoundOutput.SecondaryBufferSize);
			Win32ClearSoundBuffer(&SoundOutput);
			GlobalSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
			//NOTE(smzb): The actual program loop
			while(GlobalRunning)
			{
				MSG Message;

				//XInput Stuff
				game_input Input[2] = {};
				game_input *NewInput = &Input[0];
				game_input *OldInput = &Input[1];
				while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) {
					if (Message.message == WM_QUIT) {
						GlobalRunning = false;
					}
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				DWORD dwResult;
				int MaxControllerCount = XUSER_MAX_COUNT;
				if (MaxControllerCount > ArrayCount(NewInput->Controllers)) {
					MaxControllerCount = ArrayCount(NewInput->Controllers);
				}
				for (DWORD ControllerIndex = 0;
					ControllerIndex < MaxControllerCount;
					++ControllerIndex) 
				{
					game_controller_input *OldController = &OldInput->Controllers[ControllerIndex];
					game_controller_input *NewController = &NewInput->Controllers[ControllerIndex];
					XINPUT_STATE ControllerState;
					ZeroMemory(&ControllerState, sizeof(XINPUT_STATE));
					//NOTE(smzb): Simply get the state of the controller from XInput.
					dwResult = XInputGetState(ControllerIndex, &ControllerState);
					if (dwResult == ERROR_SUCCESS)
					{
						//NOTE(smzb): Controller is connected 
						XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;

/*						bool32 Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
						bool32 Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
						bool32 Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
						bool32 Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);

						int16 LStickX = (real32)Pad->sThumbLX;
						int16 LStickY = (real32)Pad->sThumbLY;
						int16 RStickX = (real32)Pad->sThumbRX;
						int16 RStickY = (real32)Pad->sThumbRY;
*/
						NewController->IsAnalog = true;
						//TODO(smzb): see what Casey does here, but I am assuming, that we will have to assign IsAnalog to both/all controllers
						//OldController->IsAnalog = true;
						NewController->LStartX = OldController->LEndX;
						NewController->LStartY = OldController->LEndY;
						NewController->RStartX = OldController->REndX;
						NewController->RStartY = OldController->REndY;

						//NOTE(smzb): Left Stick
						real32 lx;
						if (Pad->sThumbLX < 0) {
							lx = (real32)Pad->sThumbLX / 32768.0f;
						}
						else {
							lx = (real32)Pad->sThumbLX / 32767.0f;
						}
						NewController->LMinX = NewController->LMaxX = NewController->LEndX = lx;

						real32 ly;
						if (Pad->sThumbLY < 0) {
							ly = (real32)Pad->sThumbLY / 32768.0f;
						}
						else {
							ly = (real32)Pad->sThumbLY / 32767.0f;
						}
						NewController->LMinY = NewController->LMaxY = NewController->LEndY = ly;

						//NOTE(smzb): Right Stick
						real32 rx;
						if (Pad->sThumbRX < 0) {
							rx = (real32)Pad->sThumbRX / 32768.0f;
						}
						else {
							rx = (real32)Pad->sThumbRX / 32767.0f;
						}
						NewController->RMinX = NewController->RMaxX = NewController->REndX = rx;

						real32 ry;
						if (Pad->sThumbRY < 0) {
							ry = (real32)Pad->sThumbRY / 32768.0f;
						}
						else {
							ry = (real32)Pad->sThumbRY / 32767.0f;
						}
						NewController->RMinY = NewController->RMaxY = NewController->REndY = ry;
						
						Win32ProcessXInputButton(Pad->wButtons, &OldController->A, &NewController->A, XINPUT_GAMEPAD_A);
						Win32ProcessXInputButton(Pad->wButtons, &OldController->B, &NewController->B, XINPUT_GAMEPAD_B);
						Win32ProcessXInputButton(Pad->wButtons, &OldController->X, &NewController->X, XINPUT_GAMEPAD_X);
						Win32ProcessXInputButton(Pad->wButtons, &OldController->Y, &NewController->Y, XINPUT_GAMEPAD_Y);
						Win32ProcessXInputButton(Pad->wButtons, &OldController->Start, &NewController->Start, XINPUT_GAMEPAD_START);
						Win32ProcessXInputButton(Pad->wButtons, &OldController->Back, &NewController->Back, XINPUT_GAMEPAD_BACK);
						Win32ProcessXInputButton(Pad->wButtons, &OldController->LShoulder, &NewController->LShoulder, XINPUT_GAMEPAD_LEFT_SHOULDER);
						Win32ProcessXInputButton(Pad->wButtons, &OldController->RShoulder, &NewController->RShoulder, XINPUT_GAMEPAD_RIGHT_SHOULDER);
						Win32ProcessXInputButton(Pad->wButtons, &OldController->Up, &NewController->Up, XINPUT_GAMEPAD_DPAD_UP);
						Win32ProcessXInputButton(Pad->wButtons, &OldController->Down, &NewController->Down, XINPUT_GAMEPAD_DPAD_DOWN);
						Win32ProcessXInputButton(Pad->wButtons, &OldController->Left, &NewController->Left, XINPUT_GAMEPAD_DPAD_LEFT);
						Win32ProcessXInputButton(Pad->wButtons, &OldController->Right, &NewController->Right, XINPUT_GAMEPAD_DPAD_RIGHT);
					}
					else
					{
						//NOTE(smzb): Controller is not connected 
						//NOTE(smzb): In this case the Stub functions for xinputset/get-state should return the correct value;
					}
				}
				
				//NOTE(smzb): DirectSound output test
				bool32 SoundIsValid = false;
				DWORD PlayCursorPosition;
				DWORD WriteCursorPosition;
				DWORD ByteToLock;
				DWORD TargetCursorPosition;
				DWORD BytesToWrite;
				game_sound_buffer SoundBuffer = {};
				game_offscreen_buffer ScreenBuffer = {};
				if (SUCCEEDED(GlobalSoundBuffer->GetCurrentPosition(&PlayCursorPosition, &WriteCursorPosition)))
				{
					ByteToLock = (SoundOutput.RunningSampleIndex*SoundOutput.BytesPerSample) % SoundOutput.SecondaryBufferSize;
					TargetCursorPosition = (PlayCursorPosition + (SoundOutput.LatencySampleCount*SoundOutput.BytesPerSample)) % SoundOutput.SecondaryBufferSize;
					if (ByteToLock > TargetCursorPosition) {
						BytesToWrite = SoundOutput.SecondaryBufferSize - ByteToLock;
						BytesToWrite += TargetCursorPosition;
					}
					else {
						BytesToWrite = TargetCursorPosition - ByteToLock;
					}
					SoundIsValid = true;
				}
				SoundBuffer.SamplesPerSecond = SoundOutput.SamplesPerSecond;
				SoundBuffer.SampleCount = BytesToWrite / SoundOutput.BytesPerSample;
				SoundBuffer.Samples = Samples;
				ScreenBuffer.Memory = GlobalBitmapBuffer.Memory;
				ScreenBuffer.Width = GlobalBitmapBuffer.Width;
				ScreenBuffer.Height = GlobalBitmapBuffer.Height;;
				ScreenBuffer.Pitch = GlobalBitmapBuffer.Pitch;
				GlobalRunning = GameUpdateAndRender(NewInput, &ScreenBuffer, &SoundBuffer);
				if (SoundIsValid) { Win32FillSoundBuffer(&SoundOutput, &SoundBuffer, ByteToLock, BytesToWrite); }
				HDC DeviceContext = GetDC(WindowHandle);
				win32_window_dimensions Dimensions = Win32GetWindowDimensions(WindowHandle);
				Win32DisplayBufferInWindow(&GlobalBitmapBuffer, DeviceContext, Dimensions.Width, Dimensions.Height, 0, 0, Dimensions.Width, Dimensions.Height);
				ReleaseDC(WindowHandle, DeviceContext);
				//NOTE(smzb): Timing counters are here, but only if in debug mode.
				if (Debug) {
					uint64 EndCycleCount = __rdtsc();
					LARGE_INTEGER EndCounter;
					QueryPerformanceCounter(&EndCounter);
					uint64 CyclesElapsed = EndCycleCount - LastCycleCount;
					int64 CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;
					real64 MsPerFrame = (((1000.0f*(real64)CounterElapsed) / (real64)PerfCounterFrequency));
					real64 FPS = (real64)(PerfCounterFrequency / (real64)CounterElapsed);
					real64 MCyclesPerFrame = ((real64)CyclesElapsed / (1000.0f * 1000.0f));
					PrintDebugTime(MsPerFrame, FPS, MCyclesPerFrame);
					LastCounter = EndCounter;
					LastCycleCount = EndCycleCount;
				}
				game_input *Temp = NewInput;
				NewInput = OldInput;
				OldInput = Temp;
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
