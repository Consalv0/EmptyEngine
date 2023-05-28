
#include "CoreMinimal.h"
#include "Audio/AudioDevice.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>

namespace EE
{
	AudioDevice::AudioDevice() : PlayInfoList()
	{
		Volume = 1.F;
		static SDL_AudioSpec SampleSpecs;

		SampleSpecs.channels = 2;
		SampleSpecs.format = SDL_AUDIO_F32LSB;
		SampleSpecs.freq = 48000;

		SampleSpecs.callback = []( void* UserData, Uint8* Stream, int Length ) -> void
		{
			SDL_memset( Stream, 0, Length );
			AudioDevice& Device = *(AudioDevice*)UserData;

			for ( auto KeyValue : Device )
			{
				SamplePlayInfo* Info = KeyValue.second;
				if ( Info->Sample->GetBufferLength() <= 0 )
					return;

				int FixLength = ((uint32_t)Length > Info->Sample->GetBufferLength() - Info->Pos) ? Info->Sample->GetBufferLength() - Info->Pos : (uint32_t)Length;

				if ( !Info->bPause )
				{
					SDL_MixAudioFormat( Stream, Info->Sample->GetBufferAt( Info->Pos ), SampleSpecs.format, FixLength, (int)(SDL_MIX_MAXVOLUME * Device.Volume * Info->Volume) );
					Info->Pos += FixLength;
				}

				if ( Info->Sample->GetBufferLength() - Info->Pos <= 0 )
				{
					Info->Pos = 0;
					Info->bPause = !Info->bLoop;
				}
			}

			memcpy( Device.CurrentSample, Stream, Length );
			Device.LastAudioUpdate = Ticker::GetEpochTime<Ticker::Micro>();
		};

		SampleSpecs.userdata = this;

		/* Open the audio device */
		if ( deviceID_ = SDL_OpenAudioDevice( SDL_GetAudioDeviceName( 0, 0 ), 0, &SampleSpecs, NULL, 0 ) < 0 )
		{
			EE_CORE_ASSERT( true, "Couldn't open audio device: {0}\n", SDL_GetError() );
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

	size_t AudioDevice::AddSample( AudioSamplePtr Sample, float Volume, bool Loop, bool PlayOnAdd )
	{
		if ( Sample == NULL ) return 0;

		static size_t Increment = 0;
		PlayInfoList.emplace( ++Increment, new SamplePlayInfo( Sample, Volume, Loop, !PlayOnAdd, Increment ) );
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