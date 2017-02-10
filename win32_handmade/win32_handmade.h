#/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sebastian Meier zu Biesen $
   $Notice: (C) Copyright 2000-2016 by Joker Solutions, All Rights Reserved. $
   ======================================================================== */
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

#endif
