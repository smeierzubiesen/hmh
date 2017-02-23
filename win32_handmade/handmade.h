#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <Xinput.h>
#include <dsound.h>
#include <math.h>
#include "win32_handmade.h"

/// <summary>
/// Pi as per google.com : pi
/// </summary>
#define Pi32 3.14159265359f

#define internal static
#define local_persist static
#define global_variable static

global_variable bool GlobalRunning;
global_variable bool Debug = 0;

#if !defined(HANDMADE_H)
#define HANDMADE_H

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


///Four things here : timing, controller/keyboard input, bitmap_buffer to use, sound_buffer to user
void GameUpdateAndRender(game_offscreen_buffer *Buffer, int XOffset, int YOffset);

internal void RenderGradient(game_offscreen_buffer *Buffer, int XOffset, int YOffset);

#endif
