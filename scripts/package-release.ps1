param(
    [string]$BuildDirectory = "out/build/release-msvc",
    [string]$OutputDirectory = "out/release"
)

$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$BuildDirectory = [System.IO.Path]::GetFullPath((Join-Path $ProjectRoot $BuildDirectory))
$OutputDirectory = [System.IO.Path]::GetFullPath((Join-Path $ProjectRoot $OutputDirectory))

$CMakeText = Get-Content (Join-Path $ProjectRoot "CMakeLists.txt") -Raw
if ($CMakeText -notmatch 'project\s*\(FREDPP\s+VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)') {
    throw "Unable to read FREDPP version from CMakeLists.txt"
}
$Version = $Matches[1]

$ExecutableCandidates = @(
    (Join-Path $BuildDirectory "Release/fredpp.exe"),
    (Join-Path $BuildDirectory "fredpp.exe")
)
$Executable = $ExecutableCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $Executable) {
    throw "Release executable not found under $BuildDirectory"
}

$VersionOutput = & $Executable --version | Out-String
if ($VersionOutput -notmatch [regex]::Escape("FREDPP v$Version")) {
    throw "Executable version does not match v$Version"
}

$PackageName = "FREDPP-v$Version-windows-x64"
$StageDirectory = Join-Path $OutputDirectory $PackageName
$ArchivePath = Join-Path $OutputDirectory "$PackageName.zip"

Remove-Item $StageDirectory -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item $ArchivePath -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path $StageDirectory -Force | Out-Null

Copy-Item $Executable (Join-Path $StageDirectory "fredpp.exe")
Copy-Item (Join-Path $ProjectRoot "packaging/LISEZMOI-WINDOWS.txt") (Join-Path $StageDirectory "LISEZMOI.txt")
foreach ($File in @("LICENSE", "NOTICE", "CHANGELOG.md", "ROADMAP.md", "RELEASE_NOTES.md")) {
    Copy-Item (Join-Path $ProjectRoot $File) (Join-Path $StageDirectory $File)
}

$LibraryDirectory = Join-Path $StageDirectory "library"
New-Item -ItemType Directory -Path $LibraryDirectory -Force | Out-Null
foreach ($Procedure in @("aide.fredpp", "hello.fredpp", "index.fredpp", "ouya.fredpp")) {
    $ProcedureSource = Join-Path (Join-Path $ProjectRoot "library") $Procedure
    if (-not (Test-Path $ProcedureSource)) {
        throw "Missing release library procedure: $Procedure"
    }
    Copy-Item $ProcedureSource (Join-Path $LibraryDirectory $Procedure)
}

Compress-Archive -Path $StageDirectory -DestinationPath $ArchivePath -CompressionLevel Optimal
Write-Host "Created: $ArchivePath"
