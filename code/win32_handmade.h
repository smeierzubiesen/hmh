#if !defined(WIN32_HANDMADE_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sebastian Meier zu Biesen $
   $Notice: (C) Copyright 2000-2016 by Joker Solutions, All Rights Reserved. $
   ======================================================================== */

#define WIN32_HANDMADE_H

#define internal static
#define local_persist static
#define global_variable static

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t  int64;

struct win32_offscreen_buffer {
    BITMAPINFO Info; // FIXED(smzb): No more global variables, they exist in structs now
    void *Memory; 
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
};

struct win32_window_dimensions
{
    int Width;
    int Height;
};

global_variable bool Running; // TODO(smzb): This should not be a global in future.
global_variable win32_offscreen_buffer GlobalBackBuffer;

#endif

