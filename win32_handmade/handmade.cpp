/* ========================================================================
$File: handmade.cpp
$Date: $
$Revision: 0.1.d13 $
$Creator: Sebastian Meier zu Biesen $
$Notice: (C) Copyright 2000-2016 by Joker Solutions, All Rights Reserved. $
======================================================================== */

#include "handmade.hpp"

/// <summary>
/// This is just a dummy function to display something on the screen after we have assigned memory to the bitmap
/// </summary>
/// <param name="Buffer">Bitmap Back-buffer to us (pointer to)</param>
/// <param name="XOffset">X Offset to start Movement of the bitmap</param>
/// <param name="YOffset">X Offset to start Movement of the bitmap</param>
/// <returns>void</returns>
internal void RenderGradient(game_offscreen_buffer *Buffer, int XOffset, int YOffset) {
	uint8 *Row = (uint8 *)Buffer->Memory;
	for (int Y = 0; Y < Buffer->Height; ++Y) {
		uint32 *Pixel = (uint32 *)Row;
		for (int X = 0; X < Buffer->Width; ++X) {
			uint8 Blue = (X + XOffset);
			uint8 Green = (Y + YOffset);
			uint8 Red = (Y + X);
			*Pixel++ = ((Red << 16) | (Green << 8) | (Blue));
		}
		Row += Buffer->Pitch;
	}
}

internal void OutputGameSound(game_sound_buffer *SoundBuffer, int ToneHz) {
	local_persist real32 tSine;
	int16 ToneVolume = 5000; //NOTE(smzb): The volume of output
	int WavePeriod = SoundBuffer->SamplesPerSecond / ToneHz; //NOTE(smzb): The Wave-period describing the "duration" of one wave phase.
	int16 *SampleOut = SoundBuffer->Samples;
	for (int SampleIndex = 0; SampleIndex < SoundBuffer->SampleCount; ++SampleIndex) {
		real32 SineValue = sinf(tSine);
		int16 SampleValue = (int16)(SineValue*ToneVolume);
		*SampleOut++ = SampleValue;
		*SampleOut++ = SampleValue;
		tSine += 2.0f*Pi32*1.0f / (real32)WavePeriod;
	}
}

/// <summary>
/// The actual game loop
/// I've added a return of bool type here so we can interrupt/stop the game from inside the actual game running function
/// </summary>
bool GameUpdateAndRender(game_input * Input, game_offscreen_buffer * Buffer, game_sound_buffer * SoundBuffer) {
	local_persist int XOffset = 0;
	local_persist int YOffset = 0;
	local_persist int ToneHz = 440;

	game_controller_input *Input0 = &Input->Controllers[0];

	if (Input0->IsAnalog) {
		// Use analog input tuning
		ToneHz = 440 + (int)(330.0f*(Input0->LEndY));
		XOffset -= (int)4.0f*(Input0->LEndX);
		YOffset += (int)4.0f*(Input0->LEndY);
	}
	else {
		// Use digital input tuning
	}
	if (Input0->A.EndedDown) {
		YOffset += 2;
	}
	if (Input0->B.EndedDown) {
		XOffset += 2;
	}
	if (Input0->Back.EndedDown) {
		return false;
	}
	//TODO(smzb): Allow sample offset here for more robust platform handling
	OutputGameSound(SoundBuffer, ToneHz);
	RenderGradient(Buffer, XOffset, YOffset);
	return true;
}