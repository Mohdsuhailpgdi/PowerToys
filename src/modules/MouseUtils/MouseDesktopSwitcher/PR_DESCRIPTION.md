# Mouse Virtual Desktop Switcher

## Summary
Implements an opt-in utility to switch virtual desktops using the mouse wheel.
This implementation prioritizes system stability and strict isolation from other modules.

## Technical Design
- **Input Handling**: Uses a `WH_MOUSE_LL` hook.
  - **Opt-in**: Hook is only installed when the module is explicitly enabled in Settings.
  - **Lightweight**: Filters `HC_ACTION` and `WM_MOUSEWHEEL` immediately; passes all other events to `CallNextHookEx` without lock contention or processing.
- **Switching Logic**: Hybrid approach detailed in `DESIGN_NOTES.md`.
  - Primary: Undocumented COM (`IVirtualDesktopManagerInternal`) for atomic switching.
  - Fallback: `SendInput` (Ctrl+Win+Arrow) if COM is unavailable or fails.
  - **Defensive**: Runtime guard permanently disables the COM path for the session if a failure is detected, ensuring stable fallback.

## Safety & Component Lifecycle
- **Isolation**: Module runs within the PowerToys process but shares no state with other mouse utilities.
- **Cleanup**: Hook is managed via RAII; guaranteed removal on module disable or process termination.
- **Telemetry**: Hook installation stability and switching methods are traced (see `trace.cpp`).

## Testing
- [x] Lifecycle verification: Hook installs/removes correctly via Settings toggle.
- [x] Logic verification: Sensitivity and debounce parameters respect user config.
- [x] Fallback verification: Verified behavior when COM/internal interfaces are forced to fail.

## Documentation
- `DESIGN_NOTES.md` included in module root for maintainers.
- `README.md` included for end-users.
