
#include "CoreMinimal.h"

#include "Engine/Ticker.h"

#include "Math/MathUtils.h"
#include <chrono>
#include <ctime>

namespace EE
{
    uint64 Ticker::sLastUpdateNano = GetEpochTimeNanoNow();
    uint64 Ticker::sLastDeltaNano = 0;

    uint32 Ticker::sTickCount = 0;
    uint64 Ticker::sTickBuffer[ sMaxTickSamples ];
    uint64 Ticker::sTickAverage = 30;

    uint64 Ticker::sMaxUpdateDeltaMicro = 0;
    uint64 Ticker::sMaxRenderDeltaNano = 0;

    bool Ticker::sSkipRender = false;
    uint64 Ticker::sRenderDeltaTimeSum = 0;

    void Ticker::Tick()
    {
        uint64 tickTime = GetEpochTimeNanoNow();
        sLastDeltaNano = tickTime - sLastUpdateNano;

        if ( sLastDeltaNano < sMaxUpdateDeltaMicro )
        {
            uint64 delta = sMaxUpdateDeltaMicro - sLastDeltaNano;
            std::this_thread::sleep_for( std::chrono::nanoseconds( delta - 1000000 ) );
        }

        sLastUpdateNano = GetEpochTimeNanoNow();
        sLastDeltaNano += sLastUpdateNano - tickTime;

        sRenderDeltaTimeSum += sLastDeltaNano;
        sSkipRender = sRenderDeltaTimeSum < sMaxRenderDeltaNano;
        if ( !sSkipRender )
        {
            sRenderDeltaTimeSum = 0;
        }

        sTickBuffer[ sTickCount ] = sLastDeltaNano;

        sTickAverage = 0;
        for ( int count = 0; count < sMaxTickSamples; count++ )
        {
            sTickAverage += sTickBuffer[ count ];
        }
        sTickAverage = sTickAverage / sMaxTickSamples;

        sTickCount = (sTickCount + 1) % sMaxTickSamples;
    }

    Timestamp Ticker::GetTimeStamp()
    {
        return Timestamp( sLastUpdateNano, sLastUpdateNano + sLastDeltaNano );
    }

    uint64 Ticker::GetEpochTimeNanoNow()
    {
        using namespace std::chrono;
        return time_point_cast<nanoseconds>(steady_clock::now()).time_since_epoch().count();
        return 0;
    }

    void Timestamp::Begin()
    {
        mLastEpochTime = Ticker::GetEpochTimeNow<Ticker::Nano>();
    }

    void Timestamp::Stop()
    {
        mNowEpochTime = Ticker::GetEpochTimeNow<Ticker::Nano>();
    }

    Timestamp Timestamp::operator+( const Timestamp& other )
    {
        return Timestamp( Math::Max( mLastEpochTime, other.mLastEpochTime ), Math::Min( mNowEpochTime, other.mNowEpochTime ) );
    }
}