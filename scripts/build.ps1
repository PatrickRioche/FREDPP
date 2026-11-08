$ErrorActionPreference = 'Stop'
# Keep the PowerShell entry point consistent with the Windows batch wrapper.
& (Join-Path $PSScriptRoot 'build.bat') @args
if ($LASTEXITCODE -ne 0) {
    throw "FREDPP Debug build failed (exit code $LASTEXITCODE)"
}
