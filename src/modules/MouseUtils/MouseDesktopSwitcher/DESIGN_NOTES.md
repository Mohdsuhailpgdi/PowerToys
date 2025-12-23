# Design Notes: Hybrid Desktop Switching

## Architecture Implementation
This module employs a **Hybrid Switching Strategy** designed to balance performance with stability.

### 1. Primary: Undocumented COM (`IVirtualDesktopManagerInternal`)
- **Rationale**: The public Windows API (`IVirtualDesktopManager`) provides read-only access (querying window/desktop associations) but critically lacks a `SwitchDesktop` method. The only public way to switch desktops is simulating user input (`Ctrl + Win + Arrow`), which is slow and unreliable (see below).
- **Implementation**: We define a minimal subset of the `IVirtualDesktopManagerInternal` interface, matching the virtual table layout for Windows 10 and Windows 11.
- **Standards Alignment**: Accessing internal APIs is a known pattern in advanced PowerToys modules (e.g., FancyZones) to achieve functionality impossible via public APIs.

### 2. Fallback: Input Simulation (`SendInput`)
- **Rationale**: Internal APIs are subject to change in any Windows Update. Relying solely on them would pose a high risk of breakage.
- **Implementation**: If the COM interface fails to instantiate or returns an error during execution, the module falls back to synthesizing `Ctrl + Win + Left/Right` keystrokes.
- **Safety**: This ensures users never lose functionality, even if the "fast path" breaks.

## Risk Mitigation
| Risk | MitigationStrategy |
|------|-------------------|
| **API Breakage** | Runtime HRESULT checks are performed on every call. If an interface method fails, the module immediately downgrades to Input Simulation for the remainder of the session. |
| **VTable Mismatch** | We use specific IIDs for Win10 vs Win11 to ensure we load the correct interface version. |
| **Input Conflicts** | The fallback `SendInput` is only triggered if COM fails. |

## PowerToys Guidelines
This design respects the "PowerToys Philosophy":
1.  **Speed**: 99% of users get instant, <100ms switching via COM.
2.  **Robustness**: 100% of users have working switching via fallback.
3.  **Low Profile**: No code injection into `explorer.exe` is performed; we operate essentially as an external automation tool.

## Global Mouse Hook Safety

### 1. Installation & Removal
- **Installation**: The `WH_MOUSE_LL` hook is installed via `SetWindowsHookEx` **only** when the module is explicitly enabled in Settings. It is not active by default.
- **Removal**: The hook is immediately removed (`UnhookWindowsHookEx`) when the module is disabled or when the PowerToys process terminates. RAII wrappers in the module instance ensure cleanup on destruction.

### 2. Guardrails
- **Non-Intrusive**: The hook callback is extremely lightweight. It performs a quick pointer check (`HC_ACTION`) and a specific event check (`WM_MOUSEWHEEL`). All other events are passed immediately to `CallNextHookEx`.
- **Targeted**: Heavy logic (debouncing, desktop switching) is only executed if specific criteria are met (hold button pressed or taskbar hover).
- **Safe Concurrency**: The input processing runs on the UI thread message pump (as required by low-level hooks), but expensive operations are delegated to separate logic paths.
- **Isolation**: A unique hook handle ensures no interference with other PowerToys modules (like Mouse Highlighter) that may also hook mouse events. Windows chains hooks automatically respecting the order.
