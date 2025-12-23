# Build helper for MouseDesktopSwitcher
param (
    [string]$Configuration = "Release"
)

$SolutionDir = "$PSScriptRoot\..\..\..\..\.."
$Project = "$PSScriptRoot\MouseDesktopSwitcher.vcxproj"

Write-Host "Building MouseDesktopSwitcher ($Configuration)..."
msbuild $Project /p:Configuration=$Configuration /p:Platform=x64

if ($LASTEXITCODE -eq 0) {
    Write-Host "Build Success."
} else {
    Write-Host "Build Failed."
}
