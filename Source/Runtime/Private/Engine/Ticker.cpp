
#include "CoreMinimal.h"
#include "Engine/Ticker.h"
#include <chrono>
#include <ctime>

namespace EE
{
	uint64 Ticker::LastUpdateMicro = GetEpochTimeMicro();
	uint64 Ticker::LastDeltaMicro = 0;

	uint32 Ticker::TickCount = 0;
	uint64 Ticker::TickBuffer[ MaxTickSamples ];
	uint64 Ticker::TickAverage = 30;

	uint64 Ticker::MaxUpdateDeltaMicro = 0;
	uint64 Ticker::MaxRenderDeltaMicro = 0;

	bool Ticker::SkipRender = false;
	uint64 Ticker::RenderDeltaTimeSum = 0;

	void Ticker::Tick()
	{
		uint64 TickTime = GetEpochTimeMicro();
		LastDeltaMicro = TickTime - LastUpdateMicro;

		if ( LastDeltaMicro < MaxUpdateDeltaMicro )
		{
			uint64 Delta = MaxUpdateDeltaMicro - LastDeltaMicro;
			std::this_thread::sleep_for( std::chrono::microseconds( Delta - 1000 ) );
		}

		LastUpdateMicro = GetEpochTimeMicro();
		LastDeltaMicro += LastUpdateMicro - TickTime;

		RenderDeltaTimeSum += LastDeltaMicro;
		SkipRender = RenderDeltaTimeSum < MaxRenderDeltaMicro;
		if ( !SkipRender )
		{
			RenderDeltaTimeSum = 0;
		}

		TickBuffer[ TickCount ] = LastDeltaMicro;

		TickAverage = 0;
		for ( int Count = 0; Count < MaxTickSamples; Count++ )
		{
			TickAverage += TickBuffer[ Count ];
		}
		TickAverage = TickAverage / MaxTickSamples;

		TickCount = (TickCount + 1) % MaxTickSamples;
	}

	Timestamp Ticker::GetTimeStamp()
	{
		return Timestamp( LastUpdateMicro, LastUpdateMicro + LastDeltaMicro );
	}

	uint64 Ticker::GetEpochTimeMicro()
	{
		using namespace std::chrono;
		return time_point_cast<microseconds>(steady_clock::now()).time_since_epoch().count();
		return 0;
	}

	void Timestamp::Begin()
	{
		LastEpochTime = Ticker::GetEpochTime<Ticker::Micro>();
	}

	void Timestamp::Stop()
	{
		NowEpochTime = Ticker::GetEpochTime<Ticker::Micro>();
	}

	Timestamp Timestamp::operator+( const Timestamp& Other )
	{
		return Timestamp( Math::Max( LastEpochTime, Other.LastEpochTime ), Math::Min( NowEpochTime, Other.NowEpochTime ) );
	}
}