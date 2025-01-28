#if !defined(NoEngine_H) // TODO(NOTE): Include Guard | Keeeps you from trying to redefine a header


/*
/TODO(NOTE): This is how you could abstract the platform layer through a header like this

struct platform_window;
platform_window* PlatformOpenWindow(char* Title);
void PlatformCloseWindow(platform_window* Window);

*/

/*
	TODO(Sam): Services that the game provides to the platform layer
*/



/*
	TODO(Sam): Services that the game provides to the platform layer
	(this may expand in the future, sound on seperate thread etc.)
*/


// FOUR THINGS - timing, controller/keyboard input, bitmap buffer to use, sound buffer to use

// TODO(Sam): In the future, rendering specifically will become a three-tiered abstraction!!!
struct game_offscreen_buffer
{
	void* Memory;
	int Width;
	int Height;
	int Pitch;
};

struct game_sound_output_buffer
{
	int SamplesPerSecond;
	int SampleCount;
	int16* Samples;
};

struct game_button_state
{
	int HalfTransitionDown;
	bool32 EndedDown;
};
struct game_controller_input
{
	bool32 IsAnalog;

	real32 StartX;
	real32 StartY;

	real32 MinX;
	real32 MinY;

	real32 MaxX;
	real32 MaxY;

	real32 EndX;
	real32 EndY;
	
	union
	{
		game_button_state Buttons[6];
		struct
		{
			game_button_state Up;
			game_button_state Down;
			game_button_state Left;
			game_button_state Right;
			game_button_state LeftShoulder;
			game_button_state RightShoulder;
		};
    	};
};
struct game_input
{
	game_controller_input Controllers[4];
};

internal void GameUpdateAndRender(game_input *Input, game_offscreen_buffer *Buffer, 
				  game_sound_output_buffer *SoundBuffer);


#define NoEngine_H
#endif
