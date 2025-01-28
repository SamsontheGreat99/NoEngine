#include "NoEngine.h"

#include <math.h>


internal void GameOutputSound(game_sound_output_buffer* SoundBuffer, int ToneHz)
{
	local_persist real32 tSine;
	int16 ToneVolume = 3000;

	int WavePeriod = SoundBuffer->SamplesPerSecond / ToneHz;

	int16* SampleOut = SoundBuffer->Samples;

	for (int SampleIndex = 0; SampleIndex < SoundBuffer->SampleCount; ++SampleIndex)
	{
		real32 SineValue = sinf(tSine);
		int16 SampleValue = (int16)(SineValue * ToneVolume);
		*SampleOut++ = SampleValue;
		*SampleOut++ = SampleValue;

		tSine += 2.0f * Pi32 * 1.0f / (real32)WavePeriod;
	}
}

internal void RenderWeirdGradient(game_offscreen_buffer* Buffer, int XOffset, int YOffset)
{
	// TODO(Sam): Check and see if passing the buffer by pointer or by value is more efficient
	int Width = Buffer->Width;
	int Height = Buffer->Height;


	uint8* Row = (uint8*)Buffer->Memory;

	for (int Y = 0; Y < Buffer->Height; ++Y)
	{
		uint32* Pixel = (uint32*)Row;
		for (int X = 0; X < Buffer->Width; ++X)
		{
			/*
				Pixel in Memory: 00 00 00 00
				Big Endian:		 RR GG BB XX
				Little Endian:   BB GG RR XX  THIS IS HOW MICROSOFT STORES THEIR BYTES

				++X != X++ (in fact they are quite different and could have real side effects if used improperly)
				++X increments and then returns the value of X
				X++ returns the value of X and then increments it
			*/
			uint8 Blue = (X + XOffset);
			uint8 Green = (Y + YOffset);

			*Pixel++ = ((Green << 8) | Blue);
			/*
				*Pixel = : Dereferences the pointer, talking about the data at that point (writing to it)
				= *Pixel : Referenced the pointer, reading the data that it is pointing to
			*/
		}
		Row += Buffer->Pitch;
	}
}



internal void GameUpdateAndRender(game_input *Input, game_offscreen_buffer* Buffer, game_sound_output_buffer *SoundBuffer)
{
	local_persist int XOffset = 0;
	local_persist int YOffset = 0;
	local_persist int ToneHz = 256;
	
	game_controller_input *Input0 = &Input->Controllers[0];
	if(Input0->IsAnalog)
	{
		//TODO(Sam): Use Analog movement tuning
		ToneHz = 256 + (int)(128.0f*(Input0->EndX));
		XOffset += (int)4.0f*(Input0->EndY);
	}
	else
	{
		//TODO(Sam): Use Digital movement tuning
	}

	//Input.AButtonEndedDown;
	//Input.AButtonHalfTransitionCount;
	if(Input0->Down.EndedDown)
	{
		YOffset += 1;
	}



	//TODO(Sam):  Allow Sample offsets here for more robust platform options
	GameOutputSound(SoundBuffer, ToneHz);
	RenderWeirdGradient(Buffer, XOffset, YOffset);
}
