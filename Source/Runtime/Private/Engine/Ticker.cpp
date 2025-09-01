
#include "CoreMinimal.h"

#include "Engine/Ticker.h"

#include "Math/MathUtils.h"
#include <chrono>
#include <ctime>

namespace EE
{
    uint64 Ticker::_sLastUpdateNano = GetEpochTimeNanoNow();
    uint64 Ticker::_sLastDeltaNano = 0;

    uint32 Ticker::_sTickCount = 0;
    uint64 Ticker::_sTickBuffer[ _sMaxTickSamples ];
    uint64 Ticker::_sTickAverage = 30;

    uint64 Ticker::sMaxUpdateDeltaNano = 0;
    uint64 Ticker::sMaxRenderDeltaNano = 0;

    bool Ticker::_sSkipRender = false;
    uint64 Ticker::_sRenderDeltaTimeSum = 0;

    void Ticker::Tick()
    {
        uint64 tickTime = GetEpochTimeNanoNow();
        _sLastDeltaNano = tickTime - _sLastUpdateNano;

        if ( _sLastDeltaNano < sMaxUpdateDeltaNano )
        {
            uint64 delta = sMaxUpdateDeltaNano - _sLastDeltaNano;
            std::this_thread::sleep_for( std::chrono::nanoseconds( delta ) );
        }

        _sLastUpdateNano = GetEpochTimeNanoNow();
        _sLastDeltaNano += _sLastUpdateNano - tickTime;

        _sRenderDeltaTimeSum += _sLastDeltaNano;
        _sSkipRender = _sRenderDeltaTimeSum < sMaxRenderDeltaNano;
        if ( !_sSkipRender )
        {
            _sRenderDeltaTimeSum = 0;
        }

        _sTickBuffer[ _sTickCount ] = _sLastDeltaNano;

        _sTickAverage = 0;
        for ( int count = 0; count < _sMaxTickSamples; count++ )
        {
            _sTickAverage += _sTickBuffer[ count ];
        }
        _sTickAverage = _sTickAverage / _sMaxTickSamples;

        _sTickCount = (_sTickCount + 1) % _sMaxTickSamples;
    }

    Timestamp Ticker::GetTimeStamp()
    {
        return Timestamp( _sLastUpdateNano, _sLastUpdateNano + _sLastDeltaNano );
    }

    uint64 Ticker::GetEpochTimeNanoNow()
    {
        using namespace std::chrono;
        return time_point_cast<nanoseconds>(steady_clock::now()).time_since_epoch().count();
        return 0;
    }

    void Timestamp::Begin()
    {
        _lastEpochTime = Ticker::GetEpochTimeNow<Ticker::Nano>();
    }

    void Timestamp::Stop()
    {
        _nowEpochTime = Ticker::GetEpochTimeNow<Ticker::Nano>();
    }

    Timestamp Timestamp::operator+( const Timestamp& other )
    {
        return Timestamp( Math::Max( _lastEpochTime, other._lastEpochTime ), Math::Min( _nowEpochTime, other._nowEpochTime ) );
    }
}