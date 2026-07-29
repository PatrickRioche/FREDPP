$ErrorActionPreference = 'Stop'
$Root = Split-Path -Parent $PSScriptRoot
Set-Location $Root
ctest --test-dir out/build/x64-Debug -C Debug --output-on-failure
