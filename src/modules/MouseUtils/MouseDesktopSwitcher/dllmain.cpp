#include "pch.h"
#include <interface/powertoy_module_interface.h>
#include "trace.h"
#include "MouseDesktopSwitcher.h"
#include <common/SettingsAPI/settings_objects.h>
#include <common/utils/json.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;

class MouseDesktopSwitcherModule : public PowertoyModuleIface
{
public:
    MouseDesktopSwitcherModule()
    {
        m_app = std::make_unique<MouseDesktopSwitcher>();
    }

    virtual const wchar_t* get_name() override
    {
        static std::wstring name = L"Mouse Virtual Desktop Switcher";
        return name.c_str();
    }

    virtual const wchar_t* get_key() override
    {
        static std::wstring key = L"MouseDesktopSwitcher";
        return key.c_str();
    }

    virtual bool get_config(wchar_t* buffer, int* buffer_size) override
    {
        return true;
    }

    virtual void set_config(const wchar_t* config) override
    {
        try
        {
            auto json = json::JsonValue::Parse(config).GetObjectW();
            if (json.HasKey(L"properties"))
            {
                auto props = json.GetNamedObject(L"properties");
                MouseDesktopSwitcherSettings settings;

                if (props.HasKey(L"enabled"))
                    settings.enabled = props.GetNamedObject(L"enabled").GetNamedBoolean(L"value");
                if (props.HasKey(L"sensitivity"))
                    settings.sensitivity = (int)props.GetNamedObject(L"sensitivity").GetNamedNumber(L"value");
                if (props.HasKey(L"debounceMs"))
                    settings.debounceMs = (int)props.GetNamedObject(L"debounceMs").GetNamedNumber(L"value");
                if (props.HasKey(L"holdButton"))
                    settings.holdButton = (int)props.GetNamedObject(L"holdButton").GetNamedNumber(L"value");

                m_app->UpdateSettings(settings);
                if (settings.enabled)
                    m_app->Enable();
                else
                    m_app->Disable();
            }
        }
        catch (...)
        {
        }
    }

    virtual void enable() override
    {
        m_app->Enable();
    }

    virtual void disable() override
    {
        m_app->Disable();
    }

    virtual bool is_enabled() override
    {
        return m_app->IsEnabled();
    }

    virtual void destroy() override
    {
        delete this;
    }

private:
    std::unique_ptr<MouseDesktopSwitcher> m_app;
};

extern "C" __declspec(dllexport) PowertoyModuleIface* __cdecl powertoy_module_factory(const char* name)
{
    return new MouseDesktopSwitcherModule();
}
