$ErrorActionPreference = 'Stop'
$Root = Split-Path -Parent $PSScriptRoot
Set-Location $Root
cmake -S . -B out/build/x64-Debug
cmake --build out/build/x64-Debug --config Debug --parallel
