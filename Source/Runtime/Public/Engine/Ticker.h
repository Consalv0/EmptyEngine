#pragma once

namespace EE
{
	struct Timestamp
	{
	public:
		Timestamp() : _lastEpochTime(), _nowEpochTime() {}

		Timestamp( uint64 last, uint64 now)
			: _lastEpochTime(last), _nowEpochTime(now) { };

	public:
		template<typename T>
		FORCEINLINE typename T::ReturnType GetDeltaTime() const
		{ 
			return (_nowEpochTime - _lastEpochTime) / (typename T::ReturnType)T::GetSizeInNano();
		};

		void Begin();

		void Stop();

		uint64 GetLastEpoch() const { return _lastEpochTime; };
		uint64 GetNowEpoch() const { return _nowEpochTime; };

		Timestamp operator+(const Timestamp& other);

	private:
		uint64 _lastEpochTime;
		uint64 _nowEpochTime;
	};

	class Ticker
	{
	public:
		template<uint64 Size, typename Type>
		struct Duration { static constexpr uint64 GetSizeInNano() { return Size; }; using ReturnType = Type; };

        using Nano   = Duration<1, uint64>;
		using Micro  = Duration<1000, uint64>;
		using Mili   = Duration<1000000, double>;
		using Second = Duration<1000000000, double>;
		using Minute = Duration<166666666667, double>;

		static uint64 sMaxUpdateDeltaNano;
		static uint64 sMaxRenderDeltaNano;

		FORCEINLINE static bool IsSkippingRender() { return _sSkipRender; };

	private:
		friend class Application;

		// Don't use this unless you know what are you doing
		static void Tick();

		// Update the fixed update time
		// Don't use this unless you know what are you doing
		// static void FixedTick();

		// Time since the last tick callback
		static uint64 _sLastUpdateNano;
		static uint64 _sLastDeltaNano;

		static bool _sHasStarted;

		static uint32 _sTickCount;
		static uint64 _sTickAverage;
		static const uint32 _sMaxTickSamples = 25;
		static uint64 _sTickBuffer[_sMaxTickSamples];
		
		static uint64 _sRenderDeltaTimeSum;
		static bool _sSkipRender;

        static uint64 GetEpochTimeNanoNow();

	public:

		static Timestamp GetTimeStamp();

		// Time in seconds since the last frame;
		template<typename T>
		static inline typename T::ReturnType GetDeltaTime()
		{
			return (_sLastDeltaNano) / (typename T::ReturnType)T::GetSizeInNano();
		}

		// Get the application tick average
		template<typename T>
		static inline typename T::ReturnType GetAverageDelta()
		{
			return (typename T::ReturnType)_sTickAverage / (typename T::ReturnType)T::GetSizeInNano();
		}

		// Get the application frame rate
		template<typename T>
		static inline typename T::ReturnType GetFrameRate()
		{
			return (typename T::ReturnType)(1) / ((typename T::ReturnType)_sTickAverage / (typename T::ReturnType)T::GetSizeInNano());
		}

		// Machine Time
		template<typename T>
		static inline typename T::ReturnType GetEpochTimeNow()
		{
			return GetEpochTimeNanoNow() / (typename T::ReturnType)T::GetSizeInNano();
		}
	};

}