#pragma once

namespace EE
{
    class PlatformDevice
    {
    public:
        virtual ~PlatformDevice() = default;

        // The device is currently powered by a battery?
        virtual bool IsRunningOnBattery() = 0;

        // Returns the battery life, -1 if no information
        virtual float GetBatteryStatus() = 0;

        virtual float GetDeviceTemperature( const int& DeviceIndex ) = 0;
    };

    extern PlatformDevice* GPlatformDevice;

    //* Creates a handler, implemented in each platform
    PlatformDevice* PlatformCreatePlatformDevice();
}