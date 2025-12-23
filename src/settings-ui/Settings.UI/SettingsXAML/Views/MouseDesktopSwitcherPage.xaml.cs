using Microsoft.PowerToys.Settings.UI.ViewModels;
using Microsoft.UI.Xaml.Controls;

namespace Microsoft.PowerToys.Settings.UI.Views
{
    public sealed partial class MouseDesktopSwitcherPage : Page
    {
        public MouseDesktopSwitcherViewModel ViewModel { get; set; }

        public MouseDesktopSwitcherPage()
        {
            this.InitializeComponent();
            // ViewModel injection usually handled by Shell or Ioc
            // ViewModel = ...
        }
    }
}
