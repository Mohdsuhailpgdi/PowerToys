#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <ProjectTelemetry.h>
#include <shobjidl.h> // For IVirtualDesktopManager
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <chrono>

namespace winrt
{
    using namespace Windows::Foundation; 
}
