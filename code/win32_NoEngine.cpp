// NoEngine is the game engine I am making to learn c and c++
// Copyright Samuel Nichols 2024

/*
	TODO(Sam): THIS IS NOT A FINAL PLATFORM LAYER!!!!! Below is platform requirements

	- Saved game locations
	- Getting handle to our executable file
	- Asset loading path
	- Threading (how to launch a thread)
	- Raw Input (Support for multiple keyboards)
	- Sleep and Time Begin Period
	- ClipCursor for multimonitor support
	- Fullscreen Support
	- WM_SETCURSOR to control cursor visibility
	- QueryCancelAutoPlay
	- WM_ACTIVATEAPP
	- Blit speed improvements
	- Hardware acceleration (OpenGL or Direct3D)
	- GetKeyboardLayout (International WASD support)

	Just a partial list


*/
#include <stdint.h>

// defining different types of static
#define internal static
#define local_persist static
#define global_variable static

#define Pi32 3.14159265359f

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

#include "NoEngine.cpp"

#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include <Xinput.h>
#include <dsound.h>

//TODO(Sam): Implement Sine ourselves
#include <math.h>

#include "win32_NoEngine.h"



// We took our global variables and turned them into a struct so that now we can actually have as many buffers as we want
/*
	Input code chunk below allows us to add controller state support without linking to xinput.lib
	We are also intializing the global vairable pointers to stub functions so they don't crash the game on startup
*/
// Defining function prototype
// Defining type of function prototype
// Defining stub for the function prototype
// Setting the global variables 

// XInputGetState
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
	return(ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

// XInputSetState
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
	return(ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);

internal void win32LoadXInput(void)
{
	HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
	if(!XInputLibrary)
	{
		// TODO(Sam): Diagnostic
		XInputLibrary = LoadLibraryA("xinput1_3.dll");
	}
	if (!XInputLibrary)
	{
		// TODO(Sam): Diagnostic
		XInputLibrary = LoadLibraryA("xinput9_1_0.dll");
	}
	if(XInputLibrary)
	{
		XInputGetState = (x_input_get_state*)GetProcAddress(XInputLibrary, "XInputGetState");
		XInputSetState = (x_input_set_state*)GetProcAddress(XInputLibrary, "XInputSetState");
	}
}

// TODO(Sam): This is a global for now
global_variable bool32 GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackBuffer; // Fun new thing about c is that you can't define a variable of a struct without defining the struct (unlike c#)
global_variable LPDIRECTSOUNDBUFFER GlobalSecondaryBuffer;


internal void Win32InitDSound(HWND Window, int32 BufferSize, int32 SamplesPerSecond)
{
	// Load the library
	HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");

	if(DSoundLibrary)
	{
		// get a direct sound object! - cooperative mode
		 direct_sound_create *DirectSoundCreate = (direct_sound_create*)GetProcAddress(DSoundLibrary, "DirectSoundCreate");


		// TODO(Sam): Double check compatibility
		 LPDIRECTSOUND DirectSound;
		 if(DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0)))
		 {
			WAVEFORMATEX WaveFormat = {};
  					WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
  					WaveFormat.nChannels = 2;
  					WaveFormat.nSamplesPerSec = SamplesPerSecond;
  					WaveFormat.wBitsPerSample = 16;
  					WaveFormat.nBlockAlign = (WaveFormat.nChannels*WaveFormat.wBitsPerSample) / 8;
  					WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec*WaveFormat.nBlockAlign;
  					WaveFormat.cbSize = 0;
			if(SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY)))
			{
				DSBUFFERDESC BufferDescription = {};
				BufferDescription.dwSize = sizeof(BufferDescription);
				BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

				// "Create" primary buffer - not really a buffer, just a handle to sound card to set the format
				LPDIRECTSOUNDBUFFER PrimaryBuffer;
				if(SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0)))
				{
					HRESULT Error = PrimaryBuffer->SetFormat(&WaveFormat);
					if(SUCCEEDED(Error))
					{
						// Now we have set the format of primary buffer
						OutputDebugStringA("Primary Buffer format was set\n");
					}
					else
					{
						//TODO(Sam): Diagnostic
					}
				}
				else
				{
					//TODO(Sam): Diagnostic
				}

		 	}
		 	else
		 	{
				// TODO(Sam): Diagnostic
		 	}
			// Create secondary buffer - buffer we actually use for audio
				DSBUFFERDESC BufferDescription = {};
				BufferDescription.dwSize = sizeof(BufferDescription);
				BufferDescription.dwFlags = 0;
				BufferDescription.dwBufferBytes = BufferSize;
				BufferDescription.lpwfxFormat = &WaveFormat;
				HRESULT Error = DirectSound->CreateSoundBuffer(&BufferDescription, &GlobalSecondaryBuffer, 0);
				if(SUCCEEDED(Error))
				{		
					OutputDebugStringA("Secondary Buffer was created successfully\n");
				}
		}
		else
		{
			//TODO(Sam): Diagnostic
		}
	}
	
}
internal win32_window_dimensions Win32GetWindowDimensions(HWND Window)
{
	win32_window_dimensions Result;

	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	Result.Width = ClientRect.right - ClientRect.left;
	Result.Height = ClientRect.bottom - ClientRect.top;
	return (Result);
}

/*
	When passing the buffer here, we have 2 options, lets look at them:
	1) Pass by value: this gives us a copy of the buffer, which means we would need to return the buffer at the end of the function (error prone, what if they forget to capture the return)
	2) Pass by pointer: this gives us the raw struct of the buffer, allowing us to edit it directly

	We are choosing option 2
*/

internal void Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height) // Called once every time the user resizes the screen
{
	// TODO(Sam): see if we should free mem first or after

	// TODO(Sam): Free DIBSection
	if(Buffer->Memory){
		VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
	}

	Buffer->Width = Width;
	Buffer->Height = Height;
	Buffer->BytesPerPixel = 4;

	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height; // NOTE(Sam): positive makes it bottom-up, negative makes it top-down
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;
	
	int BufferMemorySize = (Buffer->Width * Buffer->Height)*Buffer->BytesPerPixel;
	Buffer->Memory = VirtualAlloc(0, BufferMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	Buffer->Pitch = Width * Buffer->BytesPerPixel;
}

/*
	uint8 *A = somewhere in memory;
	uint *B = somewhere in memory;

	int Y = *B; // == whatever was there
	*A = 5;
	int X = *B; // == 5

	obviously y and x are the same! The compiler should just load it once.
	It cannot do that because we have not given the compiler enough information to tell the compiler that *A doesn't point to *B

	The compiler is stupid, easiest way to help is to just use as few pointers as possible
*/
// Since we aren't changing the Buffer here, we can just pass it by value (pass a copy of it)
internal void Win32PushBufferToWindow(win32_offscreen_buffer *Buffer, HDC DeviceContext, int WindowWidth, int WindowHeight, int X, int Y, int Width, int Height)
{
	// TODO(Sam): Aspect Ratio Correction
	StretchDIBits(DeviceContext,
					0, 0, WindowWidth, WindowHeight,
					0, 0, Buffer->Width, Buffer->Height,
					Buffer->Memory, 
					&Buffer->Info, 
					DIB_RGB_COLORS, SRCCOPY);
}

internal LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
	LRESULT Result = 0;
	switch(Message)
	{
		case WM_SIZE:
			{
			}break;

		case WM_CLOSE:
			{
				// TODO(Sam): Handle with message
				GlobalRunning = false;
			}break;

		case WM_ACTIVATEAPP:
			{
				OutputDebugStringA("WM_ACTIVATEAPP\n");
			}break;

		case WM_DESTROY:
			{
				// TODO(Sam): Handle as error and recreate window
				GlobalRunning = false;
			}break;

		// Keyboard input
		case WM_SYSKEYDOWN: // because of this, syswindowproc does not handle Alt+F4 meaning we have to do it ourselves
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			uint32 VKCode = WParam; // what key actually went down
			bool32 WasDown = ((LParam & (1 << 30)) != 0); // anding lparam and bit 30 to produce 0 or 1 (bool) to see if the key was the last one pressed
			bool32 IsDown = ((LParam & (1 << 31)) == 0); // this should tell us if the key is currently down, would be 1 if key up was sent
			if(WasDown != IsDown) // keeps repeat input codes (is and was down) from doing anything
			{
				if(VKCode == 'W')
				{
				}
				else if(VKCode == 'A')
				{

				}
				else if(VKCode == 'S')
				{

				}
				else if(VKCode == 'D')
				{

				}
				else if(VKCode == 'Q')
				{

				}
				else if(VKCode == 'E')
				{

				}
				else if(VKCode == VK_UP)
				{

				}
				else if(VKCode == VK_LEFT)
				{

				}
				else if(VKCode == VK_DOWN)
				{

				}
				else if(VKCode == VK_RIGHT)
				{

				}
				else if(VKCode == VK_ESCAPE)
				{
					OutputDebugStringA("Escape: ");
					if(IsDown)
					{
						OutputDebugStringA("is down ");
					}
					if(WasDown)
					{
						OutputDebugStringA("was down ");
					}
					OutputDebugStringA("\n");
				}
				else if(VKCode == VK_SPACE)
				{

				}
			}
			// Alt + F4 control
			bool32 AltKeyWasDown = ((LParam & (1 << 29)) != 0); // checking for 29th bit of LParam (Remember your order of operations here!!)
			if(AltKeyWasDown && VKCode == VK_F4)
			{
				GlobalRunning = false;
			}
			
		}break;
		
		case WM_PAINT:
			{
				PAINTSTRUCT Paint;
				HDC DeviceContext = BeginPaint(Window, &Paint);
				int X = Paint.rcPaint.left;
				int Y = Paint.rcPaint.top;
				int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
				int Width = Paint.rcPaint.right - Paint.rcPaint.left;

				win32_window_dimensions Dimension = Win32GetWindowDimensions(Window);

				Win32PushBufferToWindow(&GlobalBackBuffer, DeviceContext, Dimension.Width, Dimension.Height, X, Y, Width, Height);			
				EndPaint(Window, &Paint); // make sure to tell windows to stop painting so in the event you move part of the window off screen, it doesn't flood your app with paint requests
			}break;

		default:
			{
				Result = DefWindowProcA(Window, Message, WParam, LParam); // tells windows to handle its own stuff
			}break;
	}
	return (Result);


}



internal void Win32ClearBuffer(win32_sound_output* SoundOutput)
{
	VOID* Region1;
	DWORD Region1Size;
	VOID* Region2;
	DWORD Region2Size;
	if (SUCCEEDED(GlobalSecondaryBuffer->Lock(0, SoundOutput->SecondaryBufferSize, 
		&Region1, &Region1Size,
		&Region2, &Region2Size,
		0)))
	{
		uint8* DestSample = (uint8*)Region1;
		for (DWORD ByteIndex = 0; ByteIndex < Region1Size; ++ByteIndex)
		{
			*DestSample++ = 0;
		}
		DestSample = (uint8*)Region2;
		for (DWORD ByteIndex = 0; ByteIndex < Region2Size; ++ByteIndex)
		{
			*DestSample++ = 0;
		}
		GlobalSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
	}
}


internal void Win32FillSoundBuffer(win32_sound_output *SoundOutput, DWORD ByteToLock, DWORD BytesToWrite, game_sound_output_buffer *SourceBuffer) {
	VOID* Region1;
	DWORD Region1Size;
	VOID* Region2;
	DWORD Region2Size;

	if (SUCCEEDED(GlobalSecondaryBuffer->Lock(ByteToLock, BytesToWrite,
		&Region1, &Region1Size,
		&Region2, &Region2Size,
		0))) {
		// TODO(Sam): assert that region 1 size is valid & region 2 size is valid
		DWORD Region1SampleCount = Region1Size / SoundOutput->BytesPerSample;
		int16* DestSample = (int16*)Region1;
		int16* SourceSample = SourceBuffer->Samples;

		for (DWORD SampleIndex = 0; SampleIndex < Region1SampleCount; ++SampleIndex) {

			*DestSample++ = *SourceSample++;
			*DestSample++ = *SourceSample++;
			++SoundOutput->RunningSampleIndex;
		}

		DWORD Region2SampleCount = Region2Size / SoundOutput->BytesPerSample;
		DestSample = (int16*)Region2;

		for (DWORD SampleIndex = 0; SampleIndex < Region2SampleCount; ++SampleIndex) {

			*DestSample++ = *SourceSample++;
			*DestSample++ = *SourceSample++;
			++SoundOutput->RunningSampleIndex;
		}

		GlobalSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
	}
}

/*
		THE STACK
		*********************
		Function call
		***********************************
		Return from function
		*********************

		Memory usage fluctuates depending functions being run and memory actually being used, it grows and shrinks as needed by your program

*/
/*
/TODO(NOTE): This is how you could abstract the win32 platform layer
struct platform_window
{
	HWND Handle;
};

platform_window* PlatformOpenWindow(char* Title)
{

	platform_window *Result = allocate it here;

	Result->Handle = ResultOfCreateWindow;

	return(Result);
}
void PlatformCloseWindow(platform_window* Window)
{
	If(Window){
	CloseWIndow(Window->Handle);
	}
}



*/






















int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine,	int ShowCode) // In our build file we can use -F(number) to start the app with a specific amount of stack memory
{
	//TODO(Note): This is where we derive our ms/frame and FPS
	LARGE_INTEGER PerfCountFrequencyResult;
	QueryPerformanceFrequency(&PerfCountFrequencyResult);

	uint64 LastCycleCount = __rdtsc();

	int64 PerfCountFrequency = PerfCountFrequencyResult.QuadPart;

	// Load inputs
	win32LoadXInput();
	// Window class
	WNDCLASSA WindowClass = {}; // initializing the window to 0

	Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);

	WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC; // flags to say repaint whole window when resizing instead of just new portion
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = Instance;

	WindowClass.lpszClassName = "NoEngineWindowClass";

	if(RegisterClassA(&WindowClass))
	{
		// Create a window using data in our window class (why we pass the pointer to the WNDCLASSA in memory)
		HWND Window = CreateWindowExA(
					0,
					WindowClass.lpszClassName,
					"NoEngine",
					WS_OVERLAPPEDWINDOW|WS_VISIBLE,
					CW_USEDEFAULT,
					CW_USEDEFAULT,
					CW_USEDEFAULT,
					CW_USEDEFAULT,
					0,
					0,
					Instance,
					0);
		if(Window)
		{
			// NOTE(Sam): we specifiec CS_OWNDC so we just get our device context and use it forever
			HDC DeviceContext = GetDC(Window);


			// NOTE(Sam): Sound Test | Maybe we should make this buffer longer than a second, if the game were to stall for longer than our buffer that would be bad
			win32_sound_output SoundOutput = {};
			SoundOutput.SamplesPerSecond = 48000;
			SoundOutput.RunningSampleIndex = 0;
			SoundOutput.BytesPerSample = sizeof(int16) * 2;
			SoundOutput.SecondaryBufferSize = SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample;
			SoundOutput.LatencySampleCount = SoundOutput.SamplesPerSecond / 15;

			Win32InitDSound(Window, SoundOutput.SecondaryBufferSize, SoundOutput.SamplesPerSecond);
			Win32ClearBuffer(&SoundOutput);
			GlobalSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

			GlobalRunning = true;

			//TODO(Sam): Pool with bitmap alloc
			
			int16 *Samples = (int16*)VirtualAlloc(0, SoundOutput.SecondaryBufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

			LARGE_INTEGER LastCounter;
			QueryPerformanceCounter(&LastCounter);

			while(GlobalRunning) // Game Loop
			{
				// Message is scoped inside while(Running) to keep anyone outside the loop from accidentally messing with it (lexical scoping)
				MSG Message;
			
				game_input Input = {};

				while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) // Looking at all the messages Windows is sending us
				{
					if(Message.message == WM_QUIT) // checking to see if it was a quit message, so that we can tell the application to quit
					{
						GlobalRunning = false;
					}
					TranslateMessage(&Message); // Translates keycodes
					DispatchMessage(&Message); // calls window class as necessary, isn't the only time windows will call you back (it can do that any time you call them)
				}
				// Post message reading game loop

				// TODO(Sam): Should we pull this more frequently
				for(DWORD ControllerIndex = 0; ControllerIndex < XUSER_MAX_COUNT; ++ControllerIndex)
				{
					XINPUT_STATE ControllerState;
					if(XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
					{
						// Controller is plugged in
						// TODO(Sam): See if controllerstate.dwpacketnumber increments too rapidly
						XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;

						bool32 DPadUp = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
						bool32 DPadDown = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
						bool32 DPadLeft = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
						bool32 DPadRight = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
						bool32 Start = (Pad->wButtons & XINPUT_GAMEPAD_START);
						bool32 Back = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
						//bool LeftThumb = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB); // Don't need joystick press for now
						//bool32 RightThumb = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
						bool32 LeftShoulder = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
						bool32 RightShoulder = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
						bool32 GamepadA = (Pad->wButtons & XINPUT_GAMEPAD_A);
						bool32 GamepadB = (Pad->wButtons & XINPUT_GAMEPAD_B);
						bool32 GamepadX = (Pad->wButtons & XINPUT_GAMEPAD_X);
						bool32 GamepadY = (Pad->wButtons & XINPUT_GAMEPAD_Y);

						int16 StickX = Pad->sThumbLX;
						int16 StickY = Pad->sThumbLY;

						//TODO(Sam): Implement proper deadzone handling later using xinput values

					}
					else
					{
						// Controller is not available
					}
				}
				// LOL use the typdef not the struct or the controller will continue to vibrate when you quit the application
				// How to vibrate controller		
				// XINPUT_VIBRATION Vibration;							
				// Vibration.wLeftMotorSpeed = 20000;
				// Vibration.wRightMotorSpeed = 10000;
				// XInputSetState(0, &Vibration);
				DWORD ByteToLock;
				DWORD TargetCursor;
				DWORD BytesToWrite;
				DWORD PlayCursor;
				DWORD WriteCursor;
				bool32 SoundIsValid = false;
				//TODO(Sam): Tighten up sound logic so we know where we need to write to and can anticipate time spent in game update
				if (SUCCEEDED(GlobalSecondaryBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor)))
				{
					ByteToLock = (SoundOutput.RunningSampleIndex * SoundOutput.BytesPerSample) % SoundOutput.SecondaryBufferSize; // Figuring out where we are in the buffer

					TargetCursor = (PlayCursor + (SoundOutput.LatencySampleCount * SoundOutput.BytesPerSample)) % SoundOutput.SecondaryBufferSize;

					if (ByteToLock > TargetCursor)
					{ // Fill two regions
						BytesToWrite = SoundOutput.SecondaryBufferSize - ByteToLock;
						BytesToWrite += TargetCursor;
					}
					else
					{ // fill one region
						BytesToWrite = TargetCursor - ByteToLock;
					}

					SoundIsValid = true;
				}
				game_sound_output_buffer SoundBuffer = {};
				SoundBuffer.SamplesPerSecond = SoundOutput.SamplesPerSecond;
				SoundBuffer.SampleCount = BytesToWrite / SoundOutput.BytesPerSample;
				SoundBuffer.Samples = Samples;

				game_offscreen_buffer Buffer = {};
				Buffer.Memory = GlobalBackBuffer.Memory;
				Buffer.Width = GlobalBackBuffer.Width;
				Buffer.Height = GlobalBackBuffer.Height;
				Buffer.Pitch = GlobalBackBuffer.Pitch;
				GameUpdateAndRender(&Input, &Buffer, &SoundBuffer); //TODO(Sam): This is where we do our update and render logic (right before we actually render)

				if(SoundIsValid)
				{
					Win32FillSoundBuffer(&SoundOutput, ByteToLock, BytesToWrite, &SoundBuffer);
				}

				

				win32_window_dimensions Dimension = Win32GetWindowDimensions(Window);
				Win32PushBufferToWindow(&GlobalBackBuffer, DeviceContext, Dimension.Width, Dimension.Height, 0, 0, Dimension.Width, Dimension.Height);				
				
				
				uint64 EndCycleCount = __rdtsc();
			
				LARGE_INTEGER EndCounter;
				QueryPerformanceCounter(&EndCounter);

				uint64 CyclesElapsed = EndCycleCount - LastCycleCount;
				int64 CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;
				real32 MSPerFrame = ((1000.0f*(real32)CounterElapsed) / (real32)PerfCountFrequency); // Milliseconds per frame
				real32 FPS = ((real32)PerfCountFrequency / (real32)CounterElapsed);
				real32 MCPF = ((real32)CyclesElapsed / (1000.0f * 1000.0f));

#if 0
				// TODO(Note): This form of printing is bad because wsprinf trusts you to not write something longer than the buffer
				// It will just start reading/writing memory that doesn't really belong to it
				char Buffer[256];
				sprintf(Buffer, "%fms/f, %ff/s, %fmc/f\n", MSPerFrame, FPS, MCPF);
				OutputDebugStringA(Buffer);
#endif
				LastCounter = EndCounter;
				LastCycleCount = EndCycleCount;
			}
		}
		else
		{
			// TODO(Sam): Logging
		}
	}
	else
	{
		// TODO(Sam): Logging
	}

	return(0); 
}
