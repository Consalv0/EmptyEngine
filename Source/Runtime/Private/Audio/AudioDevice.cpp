
#include "CoreMinimal.h"
#include "Utils/TextFormatting.h"
#include "Audio/AudioDevice.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>

namespace EE
{
	AudioDevice::AudioDevice() : PlayInfoList()
	{
		volume = 1.F;
		static SDL_AudioSpec sampleSpecs;

		sampleSpecs.channels = 2;
		sampleSpecs.format = SDL_AUDIO_F32LSB;
		sampleSpecs.freq = 48000;

		sampleSpecs.callback = []( void* UserData, Uint8* Stream, int32 Length ) -> void
		{
			SDL_memset( Stream, 0, Length );
			AudioDevice& Device = *(AudioDevice*)UserData;

			for ( auto KeyValue : Device )
			{
				SamplePlayInfo* info = KeyValue.second;
				if ( info->Sample->GetBufferLength() <= 0 )
					return;

				int32 fixLength = ((uint32)Length > info->Sample->GetBufferLength() - info->Pos) ? info->Sample->GetBufferLength() - info->Pos : (uint32)Length;

				if ( !info->bPause )
				{
					SDL_MixAudioFormat( Stream, info->Sample->GetBufferAt( info->Pos ), sampleSpecs.format, fixLength, (int32)(SDL_MIX_MAXVOLUME * Device.volume * info->volume) );
					info->Pos += fixLength;
				}

				if ( info->Sample->GetBufferLength() - info->Pos <= 0 )
				{
					info->Pos = 0;
					info->bPause = !info->bLoop;
				}
			}

			memcpy( Device.CurrentSample, Stream, Length );
			Device.LastAudioUpdate = Ticker::GetEpochTime<Ticker::Micro>();
		};

		sampleSpecs.userdata = this;

		/* Open the audio device */
		if ( deviceID_ = SDL_OpenAudioDevice( SDL_GetAudioDeviceName( 0, 0 ), 0, &sampleSpecs, NULL, 0 ) < 0 )
		{
			EE_CORE_ASSERT( true, L"Couldn't open audio device: {0}\n", Text::NarrowToWide( SDL_GetError() ) );
			bInitialized = false;
		}
		/* Start playing */
		SDL_PauseAudioDevice( deviceID_ );
		bInitialized = true;
	}

	AudioDevice::~AudioDevice()
	{
		if ( bInitialized )
			SDL_CloseAudioDevice( deviceID_ );
	}

	size_t AudioDevice::AddSample( AudioSamplePtr Sample, float volume, bool Loop, bool PlayOnAdd )
	{
		if ( Sample == NULL ) return 0;

		static size_t Increment = 0;
		PlayInfoList.emplace( ++Increment, new SamplePlayInfo( Sample, volume, Loop, !PlayOnAdd, Increment ) );
		return Increment;
	}

	void AudioDevice::RemoveSample( const size_t& Identifier )
	{
		auto PlayInfoIt = PlayInfoList.find( Identifier );
		if ( PlayInfoIt != PlayInfoList.end() )
		{
			delete PlayInfoIt->second;
			PlayInfoList.erase( Identifier );
		}
	}

}