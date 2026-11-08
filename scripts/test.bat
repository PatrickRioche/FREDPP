@echo off
setlocal
cd /d "%~dp0\.." || exit /b 1

rem Use the same external build directory as scripts/build.bat.
if not defined FREDPP_BUILD_DIR (
    if defined LOCALAPPDATA (
        set "FREDPP_BUILD_DIR=%LOCALAPPDATA%\FREDPP\build\x64-Debug"
    ) else (
        echo ERROR: LOCALAPPDATA is missing. Set FREDPP_BUILD_DIR explicitly.
        exit /b 2
    )
)
if not exist "%FREDPP_BUILD_DIR%\CTestTestfile.cmake" (
    echo ERROR: No configured tests found in "%FREDPP_BUILD_DIR%".
    echo Run scripts\build.bat first.
    exit /b 2
)

ctest --test-dir "%FREDPP_BUILD_DIR%" -C Debug --output-on-failure %*
exit /b %errorlevel%
