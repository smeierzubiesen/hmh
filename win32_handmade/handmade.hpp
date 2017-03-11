/* ========================================================================
$File: handmade.hpp
$Date: $
$Revision: 0.1.d13 $
$Creator: Sebastian Meier zu Biesen $
$Notice: (C) Copyright 2000-2016 by Joker Solutions, All Rights Reserved. $
======================================================================== */


#include <stdio.h>
#include <math.h>
#include <stdint.h>

/// <summary>
/// Pi as per google.com : pi
/// </summary>
#define Pi32 3.14159265359f

#define internal static
#define local_persist static
#define global_variable static

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

global_variable bool GlobalRunning;
global_variable bool Debug = 0;

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

#if !defined(HANDMADE_H)
#define HANDMADE_H
//NOTE(smzb): Services that the game provides to the platform layer

//NOTE(smzb): Services that the platform provides to the game

/// /// <summary>
/// Width/Height and Memory information for the bitmap buffer
/// </summary>
struct game_offscreen_buffer {
	//BITMAPINFO Info;
	void *Memory;
	int BytesPerPixel;
	int Width;
	int Height;
	int Pitch;
};

struct game_sound_buffer {
	int16 *Samples;
	int SamplesPerSecond;
	int SampleCount;
};

struct game_button_state {
	int HalfTransitionCount;
	bool32 EndedDown;
};

struct game_controller_input {
	bool32 IsAnalog;
	real32 LStartX;
	real32 LStartY;
	real32 LMinX;
	real32 LMinY;
	real32 LMaxX;
	real32 LMaxY;
	real32 LEndX;
	real32 LEndY;
	union {
		game_button_state Buttons[12];
		struct {
			game_button_state Up;
			game_button_state Down;
			game_button_state Left;
			game_button_state Right;
			game_button_state A;
			game_button_state B;
			game_button_state X;
			game_button_state Y;
			game_button_state Start;
			game_button_state Back;
			game_button_state LShoulder;
			game_button_state RShoulder;
		};
	};
};

struct game_input {
	game_controller_input Controllers[4];
};

///Four things here : timing, controller/keyboard input, bitmap_buffer to use, sound_buffer to user
void GameUpdateAndRender(game_input *Input, game_offscreen_buffer *Buffer, game_sound_buffer *SoundBuffer);

#endif
