#pragma once
#include "pch.h"

class Trace
{
public:
    static void RegisterProvider() noexcept;
    static void UnregisterProvider() noexcept;
    static void EnableChanged(bool enabled) noexcept;
    static void HookStatusChanged(bool installed, DWORD error) noexcept;
    static void SettingsChanged() noexcept;
    static void DesktopSwitch(bool success, bool fallbackUsed);
};
