#pragma once
#include "pch.h"
#include "VirtualDesktopInternal.h"

struct MouseDesktopSwitcherSettings
{
    bool enabled = false;
    int sensitivity = 50;
    int debounceMs = 300;
    bool singleStepMode = true;
    int holdButton = 0; // 0=None, 1=Left, 2=Right, 4=Middle, etc.
    // ...
};

class MouseDesktopSwitcher
{
public:
    MouseDesktopSwitcher();
    ~MouseDesktopSwitcher();

    void Enable();
    void Disable();
    bool IsEnabled() const;
    void UpdateSettings(const MouseDesktopSwitcherSettings& settings);

    // Friend hook proc
    static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

private:
    void AttemptSwitch(int direction);
    bool SwitchDesktop(int direction);
    bool SwitchDesktopFallback(int direction);
    void InputWorker();

    static MouseDesktopSwitcher* s_instance;
    HHOOK m_hook = nullptr;
    MouseDesktopSwitcherSettings m_settings;

    // State
    std::atomic<bool> m_enabled{ false };
    std::mutex m_mutex;

    // Logic
    int m_accumulatedDelta = 0;
    std::chrono::steady_clock::time_point m_lastScrollTime;
    std::chrono::steady_clock::time_point m_lastSwitchTime;

    // COM
    winrt::com_ptr<IVirtualDesktopManager> m_pDesktopManager;
    winrt::com_ptr<IVirtualDesktopManagerInternal> m_pDesktopManagerInternal;
};

void StartMouseDesktopSwitcher();
void StopMouseDesktopSwitcher();
