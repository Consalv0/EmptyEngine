
#include "CoreMinimal.h"

#include "Engine/Ticker.h"

#include "Math/MathUtils.h"
#include <chrono>
#include <ctime>

namespace EE
{
	uint64 Ticker::LastUpdateNano = GetEpochTimeNanoNow();
	uint64 Ticker::LastDeltaNano = 0;

	uint32 Ticker::TickCount = 0;
	uint64 Ticker::TickBuffer[ MaxTickSamples ];
	uint64 Ticker::TickAverage = 30;

	uint64 Ticker::MaxUpdateDeltaMicro = 0;
	uint64 Ticker::MaxRenderDeltaNano = 0;

	bool Ticker::SkipRender = false;
	uint64 Ticker::RenderDeltaTimeSum = 0;

	void Ticker::Tick()
	{
		uint64 TickTime = GetEpochTimeNanoNow();
		LastDeltaNano = TickTime - LastUpdateNano;

		if ( LastDeltaNano < MaxUpdateDeltaMicro )
		{
			uint64 Delta = MaxUpdateDeltaMicro - LastDeltaNano;
			std::this_thread::sleep_for( std::chrono::nanoseconds( Delta - 1000000 ) );
		}

		LastUpdateNano = GetEpochTimeNanoNow();
		LastDeltaNano += LastUpdateNano - TickTime;

		RenderDeltaTimeSum += LastDeltaNano;
		SkipRender = RenderDeltaTimeSum < MaxRenderDeltaNano;
		if ( !SkipRender )
		{
			RenderDeltaTimeSum = 0;
		}

		TickBuffer[ TickCount ] = LastDeltaNano;

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
		return Timestamp( LastUpdateNano, LastUpdateNano + LastDeltaNano );
	}

	uint64 Ticker::GetEpochTimeNanoNow()
	{
		using namespace std::chrono;
		return time_point_cast<nanoseconds>(steady_clock::now()).time_since_epoch().count();
		return 0;
	}

	void Timestamp::Begin()
	{
		LastEpochTime = Ticker::GetEpochTimeNow<Ticker::Nano>();
	}

	void Timestamp::Stop()
	{
		NowEpochTime = Ticker::GetEpochTimeNow<Ticker::Nano>();
	}

	Timestamp Timestamp::operator+( const Timestamp& Other )
	{
		return Timestamp( Math::Max( LastEpochTime, Other.LastEpochTime ), Math::Min( NowEpochTime, Other.NowEpochTime ) );
	}
}