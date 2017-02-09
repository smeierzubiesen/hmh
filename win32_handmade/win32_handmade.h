#/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sebastian Meier zu Biesen $
   $Notice: (C) Copyright 2000-2016 by Joker Solutions, All Rights Reserved. $
   ======================================================================== */
#if !defined(WIN32_HANDMADE_H)
#define WIN32_HANDMADE_H

#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <Xinput.h>
#include <dsound.h>
#include <math.h>

#define internal static
#define local_persist static
#define global_variable static

/// <summary>
/// Pi as per google.com : pi
/// </summary>
#define Pi32 3.14159265359f

global_variable bool GlobalRunning;
global_variable bool Debug = 1;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef int32 bool32;

typedef float real32;
typedef double real64;

/// <summary>
/// Width/Height and Memory information for the bitmap buffer
/// </summary>
struct win32_offscreen_buffer {
	BITMAPINFO Info;
	void *Memory;
	int BytesPerPixel;
	int Width;
	int Height;
	int Pitch;
};

/// <summary>
/// Simply the Width and Height of the main window (drawable area only)
/// </summary>
struct win32_window_dimensions {
	int Width;
	int Height;
};

/// <summary>
/// A struct containing the configuration and size of the DirectSound Oject
/// </summary>
/// <see cref="Dsound"/>
struct win32_sound_output {
	//NOTE(smzb): Sound stuff setup
	int SamplesPerSecond; // Samplerate of Output
	int ToneHz; // The tone to generate
	int16 ToneVolume; // The volume of output
	uint32 RunningSampleIndex; // Counter used in Squarewave/Sinewave functions
	int WavePeriod; // The Waveperiod describing the "duration" of one wave phase.
	int BytesPerSample; // How many bytes do we need per sample (L/R * 16bytes)
	int SecondaryBufferSize; // The Buffersize to actually generate sound in.

};

/// <summary>
/// The Global Bitmap Buffer
/// </summary>
global_variable win32_offscreen_buffer GlobalBackBuffer;
/// <summary>
/// The DSound Buffer to which we actually write sound.
/// </summary>
/// <see cref="DSound"/>
global_variable LPDIRECTSOUNDBUFFER GlobalSecondaryBuffer;

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
void PrintDebugTime(real32 ms, real32 fps, real32 MCyclesPerFrame) {
	char Buffer[256];
	sprintf_s(Buffer, "[:: %.03f ms/frame ::][:: %.03f FPS ::][:: %.03f MegaCycles/frame ::]\n", ms, fps, MCyclesPerFrame);
	OutputDebugStringA(Buffer);
}

#endif
