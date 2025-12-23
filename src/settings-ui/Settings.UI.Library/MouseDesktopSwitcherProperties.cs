using System.Text.Json.Serialization;

namespace Microsoft.PowerToys.Settings.UI.Library
{
    public class MouseDesktopSwitcherProperties
    {
        [JsonPropertyName("enabled")]
        public BoolProperty Enabled { get; set; } = new BoolProperty();

        [JsonPropertyName("sensitivity")]
        public IntProperty Sensitivity { get; set; } = new IntProperty(50);

        [JsonPropertyName("debounceMs")]
        public IntProperty DebounceMs { get; set; } = new IntProperty(300);

        [JsonPropertyName("holdButton")]
        public IntProperty HoldButton { get; set; } = new IntProperty(0);

        [JsonPropertyName("singleStepMode")]
        public BoolProperty SingleStepMode { get; set; } = new BoolProperty(true);
    }
}
