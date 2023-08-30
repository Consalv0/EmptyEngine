#pragma once

#include "CoreTypes.h"

namespace EE
{
	enum class EAudioFormat
	{
		NotSupported, Float32
	};

	typedef std::shared_ptr<class AudioSample> AudioSamplePtr;

	class AudioSample
	{
	public:
		AudioSample( unsigned char* Buffer, uint32 SampleSize, uint32 BufferLength, uint32 Frecuency, uint32 ChannelCount );

		~AudioSample();

		template<typename T>
		inline typename T::ReturnType GetDuration() const { return (typename T::ReturnType)Duration / (typename T::ReturnType)T::GetSizeInNano(); }

		template<typename T>
		inline typename T::ReturnType GetDurationAt( uint32 Pos ) const
		{
			return (typename T::ReturnType)(BufferLength - Pos) / BufferLength * Duration / (typename T::ReturnType)T::GetSizeInNano();
		}

		//* The number of channels in the audio.
		inline uint32 GetChannelCount() const { return ChannelCount; }

		//* The sample frequency of the audio in Hertz
		inline uint32 GetFrecuency() const { return Frecuency; }

		//* The length of one audio sample in bytes.
		inline uint32 GetSampleSize() const { return SampleSize; }

		//* The length of the audio in bytes.
		inline uint32 GetBufferLength() const { return BufferLength; }

		//* The length of the audio in samples.
		inline uint32 GetSampleLength() const { return BufferLength / SampleSize; }

		inline EAudioFormat GetAudioFormat() const { return EAudioFormat::Float32; }

		unsigned char* GetBufferAt( uint32 Offset );

		unsigned char* GetBufferCopy( uint32 Offset, uint32 Length ) const;

		bool SetData( unsigned char* InData, uint32 Offset );

		static AudioSamplePtr Create( unsigned char* Buffer, uint32 SampleSize, uint32 BufferLength, uint32 Frecuency, uint32 ChannelCount );

	private:
		unsigned long long Duration;
		uint32 ChannelCount;
		uint32 Frecuency;
		uint32 SampleSize;
		uint32 BufferLength;
		EAudioFormat format;

		unsigned char* Buffer;
	};
}