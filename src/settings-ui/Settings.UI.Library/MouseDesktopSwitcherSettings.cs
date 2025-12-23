using System.Text.Json;
using System.Text.Json.Serialization;
using Microsoft.PowerToys.Settings.UI.Library.Interfaces;

namespace Microsoft.PowerToys.Settings.UI.Library
{
    public class MouseDesktopSwitcherSettings : BasePTModuleSettings, ISettingsConfig
    {
        public const string ModuleName = "MouseDesktopSwitcher";

        [JsonPropertyName("properties")]
        public MouseDesktopSwitcherProperties Properties { get; set; } = new MouseDesktopSwitcherProperties();

        public MouseDesktopSwitcherSettings()
        {
            Name = ModuleName;
            Version = "1.0";
        }

        public string GetModuleName()
        {
            return Name;
        }

        public bool UpgradeSettingsConfiguration()
        {
            return false;
        }
    }
}
