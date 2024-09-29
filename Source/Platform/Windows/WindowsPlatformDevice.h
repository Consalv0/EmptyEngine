#pragma once

#include "Platform/PlatformDevice.h"

namespace EE
{
    class WindowsPlatformDevice : public PlatformDevice
    {
    public:
        WindowsPlatformDevice();

        ~WindowsPlatformDevice();

        virtual bool IsRunningOnBattery();

        virtual float GetBatteryStatus();

        virtual float GetDeviceTemperature( const int& deviceIndex );
    };
}