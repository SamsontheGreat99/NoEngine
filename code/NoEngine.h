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

internal void GameUpdateAndRender(game_offscreen_buffer *Buffer, int xOffset, int yOffset, game_sound_output_buffer *SoundBuffer);


#define NoEngine_H
#endif