#pragma once

#include "CoreMinimal.h"

namespace EE
{
	struct Timestamp
	{
	public:
		Timestamp() : LastEpochTime(), NowEpochTime() {}

		Timestamp( uint64 Last, uint64 Now)
			: LastEpochTime(Last), NowEpochTime(Now) { };

	public:
		template<typename T>
		FORCEINLINE typename T::ReturnType GetDeltaTime() const
		{ 
			return (NowEpochTime - LastEpochTime) / (typename T::ReturnType)T::GetSizeInMicro();
		};

		void Begin();

		void Stop();

		uint64 GetLastEpoch() const { return LastEpochTime; };
		uint64 GetNowEpoch() const { return NowEpochTime; };

		Timestamp operator+(const Timestamp& Other);

	private:
		uint64 LastEpochTime;
		uint64 NowEpochTime;
	};

	class Ticker
	{
	public:
		template<uint64 Size, typename Type>
		struct Duration { static constexpr uint64 GetSizeInMicro() { return Size; }; using ReturnType = Type; };

		using Micro  = Duration<1, uint64>;
		using Mili   = Duration<1000, double>;
		using Second = Duration<1000000, double>;
		using Minute = Duration<166666667, double>;

		static uint64 MaxUpdateDeltaMicro;
		static uint64 MaxRenderDeltaMicro;

		FORCEINLINE static bool IsSkippingRender() { return SkipRender; };

	private:
		friend class Application;

		// Don't use this unless you know what are you doing
		static void Tick();

		// Update the fixed update time
		// Don't use this unless you know what are you doing
		// static void FixedTick();

		// Time since the last tick callback
		static uint64 LastUpdateMicro;
		static uint64 LastDeltaMicro;

		static bool HasStarted;

		static uint32 TickCount;
		static uint64 TickAverage;
		static const uint32 MaxTickSamples = 25;
		static uint64 TickBuffer[MaxTickSamples];
		
		static uint64 RenderDeltaTimeSum;
		static bool SkipRender;

		static uint64 GetEpochTimeMicro();

	public:

		static Timestamp GetTimeStamp();

		// Time in seconds since the last frame;
		template<typename T>
		static inline typename T::ReturnType GetDeltaTime()
		{
			return (LastDeltaMicro) / (typename T::ReturnType)T::GetSizeInMicro();
		}

		// Get the application tick average
		template<typename T>
		static inline typename T::ReturnType GetAverageDelta()
		{
			return (typename T::ReturnType)TickAverage / (typename T::ReturnType)T::GetSizeInMicro();
		}

		// Get the application frame rate
		template<typename T>
		static inline typename T::ReturnType GetFrameRate()
		{
			return (typename T::ReturnType)(1) / ((typename T::ReturnType)TickAverage / (typename T::ReturnType)T::GetSizeInMicro());
		}

		// Machine Time
		template<typename T>
		static inline typename T::ReturnType GetEpochTime()
		{
			return GetEpochTimeMicro() / (typename T::ReturnType)T::GetSizeInMicro();
		}
	};

}