$ErrorActionPreference = 'Stop'
# Inherit FREDPP_BUILD_DIR and test selection from the shared batch entry point.
& (Join-Path $PSScriptRoot 'test.bat') @args
if ($LASTEXITCODE -ne 0) {
    throw "FREDPP Debug tests failed (exit code $LASTEXITCODE)"
}
