# Mouse Virtual Desktop Switcher

## Overview
This module allows switching virtual desktops using the mouse wheel, typically when hovering over the taskbar or desktop, or while holding a specific modifier key (or mouse button).

## Features
- Switch desktops with mouse wheel.
- Configurable activation method (Hold button, Taskbar hover).
- Adjustable sensitivity and debounce to prevent accidental fast scrolling.
- "Single-step" mode to ensure one notch = one switch.
- Falls back to `Ctrl+Win+Arrow` if native switching API is unavailable.

## Architecture
- **Language**: C++ (Native DLL) for low-level mouse hooks (`WH_MOUSE_LL`).
- **Settings**: Integrated into PowerToys Settings (C# / WinUI 3).
- **Communication**: Shared JSON settings.

## Build
The module is part of the PowerToys solution.
1. Open `PowerToys.sln` / `.slnx`.
2. Build `PowerToys.MouseDesktopSwitcher` project.
3. Ensure `PowerToys` runner is built.

## Testing
- Unit tests in `tests/` folder.
- Run tests via Test Explorer in Visual Studio.

## Privacy
- Telemetry: Reports usage (Enable/Disable/Switch count) without PII.

## Troubleshooting
- **Not switching**: Ensure "Enable" is On. Try hovering over the Taskbar.
- **Scrolling in apps**: If configured to "Always On", it may intercept attempts to scroll. Set a "Hold Button" in settings.
- **RDP**: Full-screen RDP may swallow input. The module attempts to use fallback keys, but RDP settings must allow "Windows Key combinations" to be sent to the remote PC.
