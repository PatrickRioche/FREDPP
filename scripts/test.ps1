[CmdletBinding()]
param(
    [Alias('R')]
    [string]$TestRegex
)

$ErrorActionPreference = 'Stop'

# Run CTest natively: cmd.exe can interpret regex pipes before a .bat starts.
$buildDir = if ($env:FREDPP_BUILD_DIR) {
    $env:FREDPP_BUILD_DIR
} elseif ($env:LOCALAPPDATA) {
    Join-Path $env:LOCALAPPDATA 'FREDPP\build\x64-Debug'
} else {
    throw 'LOCALAPPDATA is missing. Set FREDPP_BUILD_DIR explicitly.'
}

if (-not (Test-Path -LiteralPath (Join-Path $buildDir 'CTestTestfile.cmake') -PathType Leaf)) {
    throw "No configured tests found in '$buildDir'. Run scripts/build.bat first."
}

$ctestArgs = @('--test-dir', $buildDir, '-C', 'Debug', '--output-on-failure')
if ($PSBoundParameters.ContainsKey('TestRegex')) {
    $ctestArgs += @('-R', $TestRegex)
}

& ctest @ctestArgs
if ($LASTEXITCODE -ne 0) {
    throw "FREDPP Debug tests failed (exit code $LASTEXITCODE)"
}
