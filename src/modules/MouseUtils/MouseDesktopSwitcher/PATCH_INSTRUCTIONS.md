# Build Integration Instructions

## 1. Add Project to Solution (PowerToys.slnx)
Add the following line to the `FOLDER(path: "src\modules\MouseUtils")` section or where `MouseUtils` projects are listed:

```xml
PROJECT(path: "src\modules\MouseUtils\MouseDesktopSwitcher\MouseDesktopSwitcher.vcxproj")
```

## 2. Add Settings Files to Settings.UI.Library (Settings.UI.Library.csproj)
Include the new C# files:

```xml
<Compile Include="MouseDesktopSwitcherProperties.cs" />
<Compile Include="MouseDesktopSwitcherSettings.cs" />
```

## 3. Add Page to Settings.UI (PowerToys.Settings.UI.csproj)
Include the View and ViewModel:

```xml
<Page Include="SettingsXAML\Views\MouseDesktopSwitcherPage.xaml">
  <SubType>Designer</SubType>
  <Generator>MSBuild:Compile</Generator>
</Page>
<Compile Include="SettingsXAML\Views\MouseDesktopSwitcherPage.xaml.cs" />
<Compile Include="ViewModels\MouseDesktopSwitcherViewModel.cs" />
```

## 4. Register Module in Settings General list
Update `src\settings-ui\Settings.UI\ViewModels\GeneralSettingsViewModel.cs` (or equivalent) to include `MouseDesktopSwitcher` in the list of modules if hardcoded. Most modern modules are auto-discovered if they follow the naming convention and appear in the settings directory, or need manual entry in `EnabledModules`.

## 5. Add to Installer (installer\PowerToysSetup\PowerToys.wxs)
Add the component for `PowerToys.MouseDesktopSwitcher.dll`:

```xml
<ComponentGroup Id="MouseDesktopSwitcherComponentGroup">
    <Component Id="cmp_MouseDesktopSwitcher_dll" Guid="*">
        <File Id="fil_MouseDesktopSwitcher_dll" KeyPath="yes" Source="$(var.BinDir)Modules\MouseUtils\MouseDesktopSwitcher\PowerToys.MouseDesktopSwitcher.dll" />
    </Component>
</ComponentGroup>
```
And reference `MouseDesktopSwitcherComponentGroup` in the main Feature.
