#include "pch.h"
#include "trace.h"
#include <common/Telemetry/TraceBase.h>

TRACELOGGING_DEFINE_PROVIDER(
    g_hProvider,
    "Microsoft.PowerToys.MouseDesktopSwitcher",
    // {4A114444-1111-4444-8888-999999999999}
    (0x4a114444, 0x1111, 0x4444, 0x88, 0x88, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99),
    TraceLoggingOptionProjectTelemetry());

void Trace::RegisterProvider() noexcept
{
    TraceLoggingRegister(g_hProvider);
}

void Trace::UnregisterProvider() noexcept
{
    TraceLoggingUnregister(g_hProvider);
}

void Trace::EnableChanged(bool enabled) noexcept
{
    TraceLoggingWrite(
        g_hProvider,
        "MouseDesktopSwitcher_EnableChanged",
        TraceLoggingBoolean(enabled, "Enabled"),
        TraceLoggingKeyword(PROJECT_KEYWORD_MEASURE));
}

void Trace::HookStatusChanged(bool installed, DWORD error) noexcept
{
    TraceLoggingWrite(
        g_hProvider,
        "MouseDesktopSwitcher_HookStatus",
        TraceLoggingBoolean(installed, "Installed"),
        TraceLoggingUInt32(error, "LastError"),
        TraceLoggingKeyword(PROJECT_KEYWORD_MEASURE));
}

void Trace::SettingsChanged() noexcept
{
    TraceLoggingWrite(
        g_hProvider,
        "MouseDesktopSwitcher_SettingsChanged",
        TraceLoggingKeyword(PROJECT_KEYWORD_MEASURE));
}

void Trace::DesktopSwitch(bool success, bool fallbackUsed)
{
    TraceLoggingWrite(
        g_hProvider,
        "MouseDesktopSwitcher_DesktopSwitch",
        TraceLoggingBoolean(success, "Success"),
        TraceLoggingBoolean(fallbackUsed, "FallbackUsed"),
        TraceLoggingKeyword(PROJECT_KEYWORD_MEASURE));
}
