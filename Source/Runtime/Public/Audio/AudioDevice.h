#pragma once

#include "Core/Collections.h"
#include "Audio/AudioSample.h"

namespace EE
{
	class AudioDevice
	{
	public:
		struct SamplePlayInfo
		{
			AudioSamplePtr Sample;
			bool bPause;
			bool bLoop;
			float volume;
			uint32 Pos;
			const size_t Identifier;
			
			SamplePlayInfo(AudioSamplePtr Sample, float volume, bool Loop, bool Pause, const size_t & id) :
				Sample(Sample), volume(volume), bLoop(Loop), bPause(Pause), Pos(0), Identifier(id) {
			}
		};

		AudioDevice();

		~AudioDevice();

		size_t AddSample(AudioSamplePtr Sample, float volume, bool Loop, bool PlayOnAdd);

		inline uint32 GetFrecuency() const { return 48000; };

		inline int32 GetChannelCount() const { return 2; }

		inline uint32 SampleSize() { return 4 * 8; }

		void RemoveSample(const size_t& Identifier);

		inline TMap<size_t, SamplePlayInfo *>::iterator begin() { return PlayInfoList.begin(); }
		inline TMap<size_t, SamplePlayInfo *>::iterator end() { return PlayInfoList.end(); }

	public:
		float volume;

		unsigned char CurrentSample[32768];
		unsigned long long LastAudioUpdate;

	private:
		bool bInitialized;

		uint32 deviceID_;

		TMap<size_t, SamplePlayInfo *> PlayInfoList;
	};

}