@echo off
setlocal
cd /d "%~dp0\.."

cmake -S . -B out\build\x64-Debug -G "Visual Studio 18 2026" -A x64
if errorlevel 1 (
  echo.
  echo Visual Studio 18 generator unavailable. Trying Visual Studio 17 2022...
  cmake -S . -B out\build\x64-Debug -G "Visual Studio 17 2022" -A x64
)
if errorlevel 1 exit /b 1

rem Build serially for reliability in synchronized folders (for example Google Drive).
cmake --build out\build\x64-Debug --config Debug --parallel 1
exit /b %errorlevel%
