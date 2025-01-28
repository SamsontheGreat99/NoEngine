#if !defined(WIND32_NOENGINE_H)


struct win32_offscreen_buffer
{
 BITMAPINFO Info;
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

struct win32_sound_output {
	int SamplesPerSecond;
	uint32 RunningSampleIndex;
	int BytesPerSample;
	int SecondaryBufferSize;
	real32 tSine;
	int LatencySampleCount;
};

#define WIN32_NOENGINE_H
#endif
