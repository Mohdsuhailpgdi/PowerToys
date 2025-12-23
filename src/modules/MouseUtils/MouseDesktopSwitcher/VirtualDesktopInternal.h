#pragma once
#include <windows.h>
#include <objbase.h>

// Undocumented Interfaces for Virtual Desktop Switching

// IIDs vary by Windows build. These are common known values.
// Win10 1809+ / Win11
static const GUID IID_IVirtualDesktopManagerInternal_Win10 = { 0xF31574D6, 0xB682, 0x4CDC, { 0xBD, 0x56, 0x18, 0x27, 0x86, 0x0F, 0xBE, 0xA6 } };
static const GUID IID_IVirtualDesktopManagerInternal_Win11 = { 0xB2F9259F, 0x9A07, 0x4C2E, { 0x9D, 0x14, 0x0B, 0xC2, 0xD8, 0x5E, 0x8C, 0x8A } }; // Example IID, varies!

// We must define the vtable layout exactly.
// Simplified version of IVirtualDesktop (Just need to match vtable offset if we passed it)
// But to Switch, we need to pass a specific IVirtualDesktop pointer.
// Getting that pointer requires IVirtualDesktopManagerInternal::GetDesktops -> IObjectArray -> IVirtualDesktop.

// Given the complexity of implementing the full IObjectArray and Desktop iteration purely to find the "Next" desktop
// without pulling in a large library of undocumented interfaces,
// we will stick to a lighter COM approach if possible, OR acknowledge the 'SendInput' is actually safer for maintenance.

// However, if we assume we can traverse:
MIDL_INTERFACE("EF9F1A6C-D3CC-4358-B712-F84B635BEBE7")
IVirtualDesktop : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE IsViewVisible(IUnknown * pView, int* pfVisible) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetID(GUID * pGuid) = 0;
};

// VTable order matters significantly here.
// This definition is highly dependent on Windows version.
MIDL_INTERFACE("F31574D6-B682-4CDC-BD56-1827860FBEA6")
IVirtualDesktopManagerInternal : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetCount(UINT * pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveViewToDesktop(IUnknown * pView, IVirtualDesktop * pDesktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE CanViewMoveDesktops(IUnknown * pView, int* pfCanViewMoveDesktops) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentDesktop(IVirtualDesktop * *ppDesktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDesktops(IObjectArray * *ppDesktops) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAdjacentDesktop(IVirtualDesktop * pDesktopReference, int direction, IVirtualDesktop** ppAdjacentDesktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE SwitchDesktop(IVirtualDesktop * pDesktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDesktopW(IVirtualDesktop * *ppNewDesktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveDesktop(IVirtualDesktop * pRemove, IVirtualDesktop * pFallbackDesktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindDesktop(GUID * desktopId, IVirtualDesktop * *ppDesktop) = 0;
};

// Service ID for the internal manager
static const GUID CLSID_ImmersiveShell = { 0xC2F03A33, 0x21F5, 0x47FA, { 0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39 } };
