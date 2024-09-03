#pragma once

// Include windows headers
#ifdef EE_PLATFORM_WINDOWS

#include "Platform/Windows/WindowsMinimal.h"
#include <io.h>
#include <conio.h>

#endif // EE_PLATFORM_WINDOWS

#if (defined(__linux__) || defined(__APPLE__))

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#endif