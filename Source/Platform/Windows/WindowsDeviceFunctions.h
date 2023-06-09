#pragma once

#include "Platform/DeviceFunctions.h"

namespace EE 
{
	class WindowsDeviceFunctions : public DeviceFunctions {
	public:
		WindowsDeviceFunctions();

		~WindowsDeviceFunctions();

		virtual bool IsRunningOnBattery();

		virtual float GetBatteryStatus();

		virtual float GetDeviceTemperature(const int& DeviceIndex);
	};

}