
#include "CoreMinimal.h"

#include "Engine/Ticker.h"
#include "Audio/AudioSample.h"

namespace EE
{
	AudioSample::AudioSample( unsigned char* Buffer, uint32 SampleSize, uint32 BufferLength, uint32 Frecuency, uint32 ChannelCount )
		: SampleSize( SampleSize ), BufferLength( BufferLength ), Frecuency( Frecuency ), ChannelCount( ChannelCount ), format( EAudioFormat::Float32 )
	{
		this->Buffer = new unsigned char[ BufferLength ];
		memcpy( this->Buffer, Buffer, BufferLength );
		Duration = (int64)(((BufferLength * 8u / (SampleSize * ChannelCount)) / (float)Frecuency) * Ticker::Mili::GetSizeInNano());
	}

	AudioSample::~AudioSample()
	{
		delete Buffer;
	}

	unsigned char* AudioSample::GetBufferAt( uint32 Offset )
	{
		EE_ASSERT( BufferLength - Offset >= 0, "Trying to copy outside range of audio sample" );
		return &Buffer[ Offset ];
	}

	unsigned char* AudioSample::GetBufferCopy( uint32 Offset, uint32 Length ) const
	{
		EE_ASSERT( BufferLength - Offset > 0 || Length > BufferLength - Offset, "Trying to copy outside range of audio sample" );
		unsigned char* ReturnVal = new unsigned char[ Length ];
		return (unsigned char*)memcpy( ReturnVal, &Buffer[ Offset ], Length );
	}

	bool AudioSample::SetData( unsigned char* InData, uint32 Offset )
	{
		return false;
	}

	AudioSamplePtr AudioSample::Create( unsigned char* Buffer, uint32 SampleSize, uint32 BufferLength, uint32 Frecuency, uint32 ChannelCount )
	{
		return std::make_shared<AudioSample>( Buffer, SampleSize, BufferLength, Frecuency, ChannelCount );
	}
}

