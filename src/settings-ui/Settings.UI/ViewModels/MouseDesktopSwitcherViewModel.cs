using System;
using Microsoft.PowerToys.Settings.UI.Library;
using Microsoft.PowerToys.Settings.UI.Library.Helpers;
using Microsoft.PowerToys.Settings.UI.Library.ViewModels.Commands;
using System.Windows.Input;

namespace Microsoft.PowerToys.Settings.UI.ViewModels
{
    public class MouseDesktopSwitcherViewModel : Observable
    {
        private MouseDesktopSwitcherSettings _settings;
        private ISettingsUtils _settingsUtils;

        public MouseDesktopSwitcherViewModel(ISettingsUtils settingsUtils, ISettingsRepository<MouseDesktopSwitcherSettings> settingsRepository)
        {
            _settingsUtils = settingsUtils;
            _settings = settingsRepository.SettingsConfig;
        }

        public bool Enabled
        {
            get => _settings.Properties.Enabled.Value;
            set
            {
                if (_settings.Properties.Enabled.Value != value)
                {
                    _settings.Properties.Enabled.Value = value;
                    OnPropertyChanged(nameof(Enabled));
                    NotifySettingsChanged();
                }
            }
        }

        public int Sensitivity
        {
            get => _settings.Properties.Sensitivity.Value;
            set
            {
                if (_settings.Properties.Sensitivity.Value != value)
                {
                    _settings.Properties.Sensitivity.Value = value;
                    OnPropertyChanged(nameof(Sensitivity));
                    NotifySettingsChanged();
                }
            }
        }

        public int DebounceMs
        {
            get => _settings.Properties.DebounceMs.Value;
            set
            {
                if (_settings.Properties.DebounceMs.Value != value)
                {
                    _settings.Properties.DebounceMs.Value = value;
                    OnPropertyChanged(nameof(DebounceMs));
                    NotifySettingsChanged();
                }
            }
        }

        private void NotifySettingsChanged()
        {
            // Save logic here (via SettingsUtils)
            // _settingsUtils.SaveSettings(_settings.ToJsonString(), "MouseDesktopSwitcher");
        }
    }
}
