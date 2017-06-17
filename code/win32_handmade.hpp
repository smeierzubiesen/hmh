/* ========================================================================
   $File: win32_handmade.hpp
   $Date: $
   $Revision: 0.1.d13 $
   $Creator: Sebastian Meier zu Biesen $
   $Notice: (C) Copyright 2000-2016 by Joker Solutions, All Rights Reserved. $
   ======================================================================== */

#include <stdint.h>
#include <DSound.h>
#include <Xinput.h>
#include <Windows.h>

#include "handmade.cpp"

#if !defined(WIN32_HANDMADE_H)
#define WIN32_HANDMADE_H

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
/// A struct containing the configuration and size of the DirectSound Object
/// </summary>
/// <see cref="DIRECT_SOUND_CREATE"/>
struct win32_sound_output {
	//NOTE(smzb): Sound stuff setup
	int SamplesPerSecond; // Sample-rate of Output
	int ToneHz; // The tone to generate
	int16 ToneVolume; // The volume of output
	uint32 RunningSampleIndex; // Counter used in Square-wave/Sine-wave functions
	int BytesPerSample; // How many bytes do we need per sample (L/R * 16bytes)
	int SecondaryBufferSize; // The Buffer-size to actually generate sound in.
	real32 tSine; // The calculated sine value according to the wave-period
	int LatencySampleCount;
};

/// <summary>
/// Simply the Width and Height of the main window (drawable area only)
/// </summary>
struct win32_window_dimensions {
	int Width;
	int Height;
};

#endif
