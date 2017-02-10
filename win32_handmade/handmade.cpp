// handmade.cpp
// compiles with -doc -FC -Zi win32_handmade.cpp handmade.cpp user32.lib gdi32.lib
// see: build.bat for switches used in compilation
/* ========================================================================
$File: $
$Date: $
$Revision: 0.1.d11 $
$Creator: Sebastian Meier zu Biesen $
$Notice: (C) Copyright 2000-2016 by Joker Solutions, All Rights Reserved. $
======================================================================== */

#include "handmade.h"

/// <summary>
/// This is just a dummy function to display something on the screen after we have assigned memory to the bitmap
/// </summary>
/// <param name="Buffer">Bitmap Backbuffer to us (pointer to)</param>
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
			uint8 Red = (X + XOffset);
			*Pixel++ = ((Red << 16) | (Green << 8) | (Blue));
		}
		Row += Buffer->Pitch;
	}
}

void GameUpdateAndRender(game_offscreen_buffer *Buffer, int XOffset, int YOffset) {
	RenderGradient(Buffer, XOffset, YOffset);
}