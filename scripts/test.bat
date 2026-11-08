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

rem PowerShell may strip argument quotes when calling .bat files.
rem Never forward %*: unquoted regex alternation would become a cmd pipe.
if "%~1"=="" goto run_all
if /I "%~1"=="-R" goto run_regex
goto usage

:run_regex
if "%~2"=="" goto usage
if not "%~3"=="" goto usage
ctest --test-dir "%FREDPP_BUILD_DIR%" -C Debug --output-on-failure -R "%~2"
exit /b %errorlevel%

:run_all
ctest --test-dir "%FREDPP_BUILD_DIR%" -C Debug --output-on-failure
exit /b %errorlevel%

:usage
echo Usage: test.bat [-R "test_regex"]
exit /b 2
