@echo off
setlocal
cd /d "%~dp0\.." || exit /b 1

rem Keep generated MSVC artifacts outside synchronized source directories.
rem Callers may override the stable default with FREDPP_BUILD_DIR.
if not defined FREDPP_BUILD_DIR (
    if defined LOCALAPPDATA (
        set "FREDPP_BUILD_DIR=%LOCALAPPDATA%\FREDPP\build\x64-Debug"
    ) else (
        echo ERROR: LOCALAPPDATA is missing. Set FREDPP_BUILD_DIR explicitly.
        exit /b 2
    )
)
rem Visual Studio 17 users can set FREDPP_CMAKE_GENERATOR before building.
if not defined FREDPP_CMAKE_GENERATOR (
    set "FREDPP_CMAKE_GENERATOR=Visual Studio 18 2026"
)

set "FREDPP_BUILD_EXTRA="
if /I "%~1"=="--clean-first" set "FREDPP_BUILD_EXTRA=--clean-first"
if not "%~1"=="" if not defined FREDPP_BUILD_EXTRA (
    echo Usage: build.bat [--clean-first]
    exit /b 2
)

echo FREDPP build directory: "%FREDPP_BUILD_DIR%"
cmake -S . -B "%FREDPP_BUILD_DIR%" -G "%FREDPP_CMAKE_GENERATOR%" -A x64 -DFREDPP_BUILD_TESTS=ON
if errorlevel 1 exit /b 1

rem Serialize compilation to minimize conflicts with file synchronization.
cmake --build "%FREDPP_BUILD_DIR%" --config Debug --parallel 1 %FREDPP_BUILD_EXTRA%
if errorlevel 1 exit /b 1

echo FREDPP Debug executable: "%FREDPP_BUILD_DIR%\Debug\fredpp.exe"
exit /b 0
