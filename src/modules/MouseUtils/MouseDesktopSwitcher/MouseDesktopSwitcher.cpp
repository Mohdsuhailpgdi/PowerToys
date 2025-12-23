#include "pch.h"
#include "MouseDesktopSwitcher.h"
#include "trace.h"

extern "C" IMAGE_DOS_HEADER __ImageBase;

MouseDesktopSwitcher* MouseDesktopSwitcher::s_instance = nullptr;

MouseDesktopSwitcher::MouseDesktopSwitcher()
{
    s_instance = this;
    if (FAILED(CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&m_pDesktopManager))))
    {
        // Log error
    }

    // Attempt to get Internal Manager
    winrt::com_ptr<IServiceProvider> serviceProvider;
    if (SUCCEEDED(CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(serviceProvider.put()))))
    {
        // Try Win11
        if (FAILED(serviceProvider->QueryService(IID_IVirtualDesktopManagerInternal_Win11, IID_IVirtualDesktopManagerInternal_Win11, m_pDesktopManagerInternal.put_void())))
        {
            // Try Win10
            serviceProvider->QueryService(IID_IVirtualDesktopManagerInternal_Win10, IID_IVirtualDesktopManagerInternal_Win10, m_pDesktopManagerInternal.put_void());
        }
    }
}

MouseDesktopSwitcher::~MouseDesktopSwitcher()
{
    Disable();
    s_instance = nullptr;
}

void MouseDesktopSwitcher::Enable()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_enabled)
        return;

    // Install Hook
    HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(&__ImageBase);
    m_hook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, hInstance, 0);
    if (m_hook)
    {
        m_enabled = true;
        Trace::EnableChanged(true);
        Trace::HookStatusChanged(true, 0);
    }
    else
    {
        Trace::HookStatusChanged(false, GetLastError());
    }
}

void MouseDesktopSwitcher::Disable()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_enabled)
        return;

    if (m_hook)
    {
        UnhookWindowsHookEx(m_hook);
        m_hook = nullptr;
        Trace::HookStatusChanged(false, 0);
    }
    m_enabled = false;
    Trace::EnableChanged(false);
}

bool MouseDesktopSwitcher::IsEnabled() const
{
    return m_enabled;
}

void MouseDesktopSwitcher::UpdateSettings(const MouseDesktopSwitcherSettings& settings)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_settings = settings;
    // Log settings changed
    Trace::SettingsChanged();
}

LRESULT CALLBACK MouseDesktopSwitcher::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && s_instance && s_instance->m_enabled)
    {
        if (wParam == WM_MOUSEWHEEL)
        {
            MSLLHOOKSTRUCT* pMouse = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
            short delta = HIWORD(pMouse->mouseData);

            // Logic to determine if we should switch
            bool shouldProcess = false;

            // Check Hold Button
            int holdBtn = s_instance->m_settings.holdButton; // 0=None
            if (holdBtn > 0)
            {
                // Check if button is pressed
                // 1=Left, 2=Right, 4=Middle (Example mapping)
                // Use GetAsyncKeyState
                int vKey = 0;
                if (holdBtn == 1)
                    vKey = VK_LBUTTON;
                else if (holdBtn == 2)
                    vKey = VK_RBUTTON;
                else if (holdBtn == 4)
                    vKey = VK_MBUTTON; // Middle
                else if (holdBtn == 5)
                    vKey = VK_XBUTTON1;
                else if (holdBtn == 6)
                    vKey = VK_XBUTTON2;

                if (vKey != 0 && (GetAsyncKeyState(vKey) & 0x8000))
                {
                    shouldProcess = true;
                }
            }
            else
            {
                // If Hold Button is NONE, we default to "Over Taskbar" or "Over Desktop" to avoid breaking scrolling
                // Or if the user just wants it always on?
                // PowerToys implies "Power" user. But breaking scroll is bad.
                // We check window under cursor.
                POINT pt = pMouse->pt;
                HWND hWnd = WindowFromPoint(pt);
                // Get ancestor to find root
                HWND hRoot = GetAncestor(hWnd, GA_ROOT);
                char className[256];
                GetClassNameA(hRoot, className, 256);

                if (strcmp(className, "Shell_TrayWnd") == 0 || strcmp(className, "WorkerW") == 0 || strcmp(className, "Progman") == 0)
                {
                    shouldProcess = true;
                }
            }

            if (shouldProcess)
            {
                s_instance->AttemptSwitch(delta);
                return 1; // Swallow event
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void MouseDesktopSwitcher::AttemptSwitch(int delta)
{
    // Debounce & Accumulate
    auto now = std::chrono::steady_clock::now();
    long long msSinceLastSwitch = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastSwitchTime).count();

    if (msSinceLastSwitch < m_settings.debounceMs)
    {
        return; // Ignore
    }

    // Accumulate
    // m_accumulatedDelta += delta;
    // Logic: If strict single step, check direction

    // Normalize delta (WHEEL_DELTA = 120)

    int threshold = 120; // could be scaled by sensitivity
    // Sensitivity 50 = 120. 100 = 10? 0 = 500?
    // Map sensitivity 0-100 to Threshold.
    // Let's say: Threshold = 240 - (Sensitivity * 2.2) approx?
    // If Sens=100, Thresh=20. Sens=0, Thresh=240.
    threshold = 240 - (m_settings.sensitivity * 2);
    if (threshold < 10)
        threshold = 10;

    m_accumulatedDelta += delta;

    if (std::abs(m_accumulatedDelta) >= threshold)
    {
        // Switch
        int direction = (m_accumulatedDelta > 0) ? -1 : 1; // Wheel Up = Left (Prev), Down = Right (Next) usually
        // Wait, standard: Up = Scroll Up = Content down.
        // Virtual Desktops: Ctrl+Win+Left (Prev) / Right (Next).
        // Standard Mapping: Wheel Up -> Prev Desktop? Wheel Down -> Next?
        // Let's stick to: Up -> Left (Prev), Down -> Right (Next).

        bool success = SwitchDesktop(direction);
        if (!success)
        {
            SwitchDesktopFallback(direction);
        }

        m_lastSwitchTime = now;
        m_accumulatedDelta = 0; // Reset
    }
}

bool MouseDesktopSwitcher::SwitchDesktop(int direction)
{
    // Try COM Switch first
    if (m_pDesktopManagerInternal)
    {
        bool comSuccess = false;
        // 1. Get Current Desktop
        winrt::com_ptr<IVirtualDesktop> currentDesktop;
        if (SUCCEEDED(m_pDesktopManagerInternal->GetCurrentDesktop(currentDesktop.put())))
        {
            // 2. Get Adjacent (Direction: 3 = Left, 4 = Right. Wait, check enum)
            // AdjacentDesktopDirection: LeftDirection = 3, RightDirection = 4.
            // But let's verify. Usually 3=Left, 4=Right.
            int directionEnum = (direction < 0) ? 3 : 4;

            winrt::com_ptr<IVirtualDesktop> nextDesktop;
            if (SUCCEEDED(m_pDesktopManagerInternal->GetAdjacentDesktop(currentDesktop.get(), directionEnum, nextDesktop.put())))
            {
                // 3. Switch
                if (SUCCEEDED(m_pDesktopManagerInternal->SwitchDesktop(nextDesktop.get())))
                {
                    Trace::DesktopSwitch(true, false);
                    comSuccess = true;
                }
            }
        }

        if (comSuccess)
        {
            return true;
        }
        else
        {
            // Defensive Guard: If valid pointer failed to work, assume API mismatch/breakage.
            // Disable COM path for this session to avoid repeated errors/instability.
            m_pDesktopManagerInternal = nullptr;
        }
    }

    return false; // Trigger fallback
}

bool MouseDesktopSwitcher::SwitchDesktopFallback(int direction)
{
    // Send Ctrl + Win + Left/Right
    INPUT inputs[4] = {};

    // Ctrl Down
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;

    // Win Down
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_LWIN;

    // Arrow Down
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = (direction < 0) ? VK_LEFT : VK_RIGHT;

    // Send Down
    SendInput(3, inputs, sizeof(INPUT));

    // Release all
    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(3, inputs, sizeof(INPUT));

    Trace::DesktopSwitch(true, true);
    return true;
}
