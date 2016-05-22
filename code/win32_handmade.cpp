// win32_handmade.cpp
// compiles with -doc -FC -Zi win32_handmade.cpp user32.lib gdi32.lib
// see: build.bat for switches used in compilation
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sebastian Meier zu Biesen $
   $Notice: (C) Copyright 2000-2016 by Joker Solutions, All Rights Reserved. $
   ======================================================================== */

// TODO(smzb): Add a better comment template for new .cpp and .h files
// FIXED(smzb): Template has been changed in .emacs file
// NOTE(smzb): Something else that is noteworthy

#include <windows.h>
#include <stdint.h>
#include <xinput.h>
#include <dsound.h>
#include "win32_handmade.h"

global_variable int XOffset;
global_variable int YOffset;

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub) { return(ERROR_DEVICE_NOT_CONNECTED); }
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub) { return(ERROR_DEVICE_NOT_CONNECTED); }
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);
//DIRECT_SOUND_CREATE(DirectSoundCreateStub) { return(0); }
//global_variable direct_sound_create *DirectSoundCreate_ = DirectSoundCreateStub;
//#define DirectSoundCreate DirectSoundCreate_;

/// <summary>Win32LoadXInput loads xinput1_4.dll
/// <para>Here's how you could make a second paragraph in a description. <see cref="System::Console::WriteLine"/> for information about output statements.</para>
/// <seealso cref="MyClass::MyMethod2"/>
/// </summary>
internal void Win32LoadXInput(void) {
    HMODULE XInputLibrary = LoadLibrary("xinput1_4.dll");
    if(!XInputLibrary) {
        HMODULE XInputLibrary = LoadLibrary("xinput1_3.dll");
    }
    if(XInputLibrary)
    {
        XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
    }
}

internal void Win32InitDSound(HWND Window, int32 SamplesPerSecond, int32 BufferSize) {
    // TODO(smzb): Load Lib
    HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
    if(DSoundLibrary)
    {
        // TODO(smzb): Get DSound Object
        direct_sound_create *DirectSoundCreate = (direct_sound_create *)GetProcAddress(DSoundLibrary, "DirectSoundCreate");
        
        LPDIRECTSOUND DirectSound;
        if(DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0))) {
            WAVEFORMATEX WaveFormat = {};
            WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
            WaveFormat.nChannels = 2;
            WaveFormat.nSamplesPerSec = SamplesPerSecond;
            WaveFormat.wBitsPerSample = 16;
            WaveFormat.nBlockAlign = (WaveFormat.nChannels*WaveFormat.wBitsPerSample) / 8;
            WaveFormat.nAvgBytesPerSec = (WaveFormat.nSamplesPerSec*WaveFormat.nBlockAlign);
            WaveFormat.cbSize = 0;
            if(SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY))) {
                DSBUFFERDESC BufferDescription = {};
                BufferDescription.dwSize = sizeof(BufferDescription);
                BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
                BufferDescription.dwBufferBytes = BufferSize;

                // TODO(smzb): Create "primary" buffer
                LPDIRECTSOUNDBUFFER PrimaryBuffer;
                if(SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0))) {

                    HRESULT Error = PrimaryBuffer->SetFormat(&WaveFormat);
                if(SUCCEEDED(Error)) {
                        // We've got a Primary buffer
                        OutputDebugStringA("Primary Buffer format set\n");
                    } else {
                        // TODO(smzb): Primary Buffer Diagnostic
                        OutputDebugStringA("Primary Buffer format NOT set\n");
                    }
                    OutputDebugStringA("Primary Buffer set");
                } else {
                    OutputDebugStringA("Primary Buffer NOT set");
                }
            } else {
                // TODO(smzb): Direct Sound Diagnostic
            }
            
            // TODO(smzb): Create "secondary" buffer
            DSBUFFERDESC BufferDescription = {};
            BufferDescription.dwSize = sizeof(BufferDescription);
            BufferDescription.dwFlags = 0;
            BufferDescription.dwBufferBytes = BufferSize;
            BufferDescription.lpwfxFormat = &WaveFormat;
            LPDIRECTSOUNDBUFFER SecondaryBuffer;
            HRESULT Error = DirectSound->CreateSoundBuffer(&BufferDescription, &SecondaryBuffer, 0);
            if(SUCCEEDED(Error)) {
                OutputDebugStringA("Secondary Buffer set\n");
            } else {
                //Secondary Buffer Diagnostics
                OutputDebugStringA("Secondary Buffer NOT set\n");
            }
        } else {
            // TODO(smzb): Return some diagnostic info for no-sound..
        }

        // TODO(smzb): Start playback.
        
    }
}

internal win32_window_dimensions Win32GetWindowDimensions(HWND Window) {
    win32_window_dimensions Result;
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    return(Result);
}

internal void RenderGradient(win32_offscreen_buffer *Buffer, int XOffset, int YOffset) {
    int Width = Buffer->Width;
    int height = Buffer->Height;
    uint8 *Row = (uint8 *)Buffer->Memory;
    for(int Y = 0; Y < Buffer->Height; Y++)
    {
        uint32 *Pixel = (uint32 *)Row;
        for(int X = 0; X < Buffer->Width; X++)
        {
            uint8 Blue = (X + XOffset);
            uint8 Green = (Y + YOffset);
            *Pixel++ = ((Green << 8) | (Blue));
		}
        Row += Buffer->Pitch;
    }
}

internal void Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height) {
    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
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
    Buffer->Pitch = Width*Buffer->BytesPerPixel;
}

internal void Win32UpdateWindow(HDC DeviceContext, win32_offscreen_buffer *Buffer, int WindowWidth, int WindowHeight, int X, int Y, int W, int H) {
    // TODO(smzb): Aspect ratio needs looked at.
    // NOTE(smzb): at this point in time we simply render whatever is in the framebuffer without regard to any scaling of source material
    StretchDIBits(DeviceContext,
                  0, 0, WindowWidth, WindowHeight,
                  0, 0, Buffer->Width, Buffer->Height,
                  Buffer->Memory,
                  &Buffer->Info,
                  DIB_RGB_COLORS,
                  SRCCOPY);
}

// NOTE(smzb): This simply deals with the program loop and handles events based on WindowMessage
LRESULT CALLBACK Win32MainWindowCallBack(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam) {
    LRESULT Result = 0;
    switch(Message)
    {
        case WM_SIZE:
        {
            // TODO(smzb): do we have to deal with resizing or is the on-screen rendering enough?
        } break;
        case WM_DESTROY:
        {
            Running = false; // TODO(smzb): Handle this as error? Re-create Window?
                             // NOTE(smzb): RThis is NOT a close event, this event occurs if the window handle gets destroyed before its time
        } break;
        case WM_CLOSE:
        {
            Running = false; // TODO(smzb): Throw a message to the user to ask for confirmation?
        } break;
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.top;
			int H = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int W = Paint.rcPaint.right - Paint.rcPaint.left;
            win32_window_dimensions Dim = Win32GetWindowDimensions(Window);
            Win32UpdateWindow(DeviceContext, &GlobalBackBuffer, Dim.Width, Dim.Height, X, Y, W, H);
            EndPaint(Window, &Paint);
        } break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32 VKCode = WParam;
            bool WasDown = ((LParam & (1 << 30)) != 0);
            bool IsDown = ((LParam & (1 << 31)) == 0);
            if(VKCode == 'A') {
                OutputDebugStringA("A\n");
            } else if(VKCode == 'S') {
                OutputDebugStringA("S\n");
            } else if(VKCode == 'W') {
                OutputDebugStringA("W\n");
            } else if(VKCode == 'D') {
                OutputDebugStringA("D\n");
            } else if(VKCode == 'Q') {
                OutputDebugStringA("Q\n");
            } else if(VKCode == 'E') {
                OutputDebugStringA("E\n");
            } else if(VKCode == VK_UP) {
                OutputDebugStringA("UP\n");
            } else if(VKCode == VK_DOWN) {
                OutputDebugStringA("DOWN\n");
            } else if(VKCode == VK_LEFT) {
                OutputDebugStringA("LEFT\n");
            } else if(VKCode == VK_RIGHT) {
                OutputDebugStringA("RIGHT\n");
            } else if(VKCode == VK_ESCAPE) {
                OutputDebugStringA("ESCAPE: ");
                if( IsDown)
                {
                    OutputDebugStringA("IsDown");
                }
                if(WasDown)
                {
                    OutputDebugStringA("WasDown");
                }
                OutputDebugStringA("\n");
                //
            } else if(VKCode == VK_SPACE) {
                OutputDebugStringA("SPACE\n");
                if(IsDown) {
                    YOffset += 2;
                }
                if(WasDown) {
                    //
                }
            }
            bool AltWasDown = ((LParam & (1 << 29)) != 0);
            if((VKCode == VK_F4) && AltWasDown) {
                    Running = false; 
                }
        }
        default:
        {
            //OutputDebugStringA("no message\n");
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }
    return(Result);
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine,	int ShowCode) {
    Win32LoadXInput();
    WNDCLASS WindowClass = {};
    Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);
    WindowClass.style = CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallBack;
    WindowClass.hInstance = Instance;
    //WindowClass.hIcon; // TODO(smzb): Make an icon and stick it in here.
    // NOTE(smzb): Possibly as an .exe resource?
    WindowClass.lpszClassName = "HandmadeHeroWindowClass";
    char title[32] = "Handmade Hero (day6)";
    char* WindowTitle = title;
    if(RegisterClass(&WindowClass))
    {
        HWND Window = CreateWindowEx(0, WindowClass.lpszClassName,
                                     WindowTitle, // TODO(smzb): maybe some version information here?
                                     WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     1280, // NOTE(smzb): this is width
                                     720, // NOTE(smzb): this is height
                                     0,
                                     0,
                                     Instance,
                                     0);
        if (Window)
        {
            Win32InitDSound(Window, 48000, 48000*sizeof(int16)*2);

            Running = true;
            while(Running)
            {
                MSG Message;
                while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if (Message.message == WM_QUIT)
                    {
                        Running = false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }

                // TODO(smzb): check whether we want to poll more frequently
                for(DWORD ControllerIndex = 0;
                    ControllerIndex < XUSER_MAX_COUNT; ++ControllerIndex)
                {
                    XINPUT_STATE ControllerState;
                    if(XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
                    {
                        // NOTE(smzb): Controller is plugged in
                        XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;

                        bool Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                        bool Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                        bool Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                        bool Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                        bool Start = (Pad->wButtons & XINPUT_GAMEPAD_START);
                        bool Back = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
                        bool LShoulder = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                        bool RShoulder = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                        bool AButton = (Pad->wButtons & XINPUT_GAMEPAD_A);
                        bool BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
                        bool XButton = (Pad->wButtons & XINPUT_GAMEPAD_X);
                        bool YButton = (Pad->wButtons & XINPUT_GAMEPAD_Y);

                        int16 StickX = Pad->sThumbLX;
                        int16 StickY = Pad->sThumbLY;

                        if(AButton){
                            XINPUT_VIBRATION Vibration;
                            Vibration.wLeftMotorSpeed = 60000;
                            Vibration.wRightMotorSpeed = 60000;
                            XInputSetState(0, &Vibration);
                            YOffset += 2;
                        } else {
                            XINPUT_VIBRATION Vibration;
                            Vibration.wLeftMotorSpeed = 0;
                            Vibration.wRightMotorSpeed = 0;
                            XInputSetState(0, &Vibration);
                        }
                    } else {
                        // NOTE(smzb): Controller is NOT present
                    }
                    
                }
                
                RenderGradient(&GlobalBackBuffer, XOffset, YOffset);
                HDC DeviceContext = GetDC(Window);
                win32_window_dimensions Dim = Win32GetWindowDimensions(Window);
                Win32UpdateWindow(DeviceContext, &GlobalBackBuffer, Dim.Width, Dim.Height, 0, 0, Dim.Width, Dim.Height);
                ReleaseDC(Window, DeviceContext);
                ++XOffset;
                
            }
        } else {
            // TODO(smzb): Log the fact that we couldn't retrieve a Window Handle
            // This is a critical failure.
        }
    }
    else
    {
        // TODO(smzb): Log the fact that we couldn't register a new WindowClass
        // This is a critical failure.
    }
    
    return(0);
}
