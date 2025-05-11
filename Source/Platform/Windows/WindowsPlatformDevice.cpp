
#include "CoreMinimal.h"

#include "Platform/PrePlatform.h"
#include "Platform/Platform.h"
#include "Platform/Windows/WindowsPlatformDevice.h"
#include "Platform/PostPlatform.h"

#ifdef EE_PLATFORM_NVML
#include <nvml.h>
#endif

namespace EE
{
    WindowsPlatformDevice::WindowsPlatformDevice() {
#ifdef EE_PLATFORM_NVML
        nvmlReturn_t DeviceResult = nvmlInit();

        if (NVML_SUCCESS != DeviceResult)
            LOG_CORE_ERROR("Failed to initialize NVML: {}", NString(nvmlErrorString(DeviceResult)));
#endif
    }

    WindowsPlatformDevice::~WindowsPlatformDevice() {
#ifdef EE_PLATFORM_NVML
        nvmlReturn_t DeviceResult = nvmlShutdown();

        if (NVML_SUCCESS != DeviceResult)
            LOG_CORE_ERROR("Failed to shutdown NVML: {}", NString(nvmlErrorString(DeviceResult)));
#endif
    }

    bool WindowsPlatformDevice::IsRunningOnBattery()
    {
        SYSTEM_POWER_STATUS Status;
        GetSystemPowerStatus( &Status );

        switch ( Status.ACLineStatus )
        {
        case 0:             // Offline
        case 255:           // Unknown status
            return false;
        case 1:             // Online
        default:
            return true;
        }
    }

    float WindowsPlatformDevice::GetBatteryStatus()
    {
        SYSTEM_POWER_STATUS Status;
        GetSystemPowerStatus( &Status );

        switch ( Status.BatteryFlag )
        {
        case 128:   // No system battery
        case 255:   // Unknown status-unable to read the battery flag information"
        default:
            break;
        }

        return Status.BatteryLifePercent;
    }

    float WindowsPlatformDevice::GetDeviceTemperature( const int& deviceIndex )
    {
        uint32 DeviceTemperature = 0;
#ifdef EE_PLATFORM_NVML
        nvmlDevice_t Device;
        nvmlReturn_t DeviceResult;

        DeviceResult = nvmlDeviceGetHandleByIndex( DeviceIndex, &Device );
        if ( NVML_SUCCESS != DeviceResult )
        {
            LOG_CORE_ERROR( "Failed to get handle of NVIDIA device {0:d}: {1}", DeviceIndex, NString( nvmlErrorString( DeviceResult ) ) );
            return (float)DeviceTemperature;
        }

        DeviceResult = nvmlDeviceGetTemperature( Device, NVML_TEMPERATURE_GPU, &DeviceTemperature );
        if ( NVML_SUCCESS != DeviceResult )
            LOG_CORE_ERROR( "Failed to get temperature of NVIDIA device {0:d}: {1}", DeviceIndex, NString( nvmlErrorString( DeviceResult ) ) );
#endif

        return (float)DeviceTemperature;
    }

    PlatformDevice* GPlatformDevice = NULL;

    PlatformDevice* PlatformCreatePlatformDevice()
    {
        return new WindowsPlatformDevice();
    }
}