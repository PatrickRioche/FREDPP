@echo off
setlocal
cd /d "%~dp0\.."
ctest --test-dir out\build\x64-Debug -C Debug --output-on-failure
exit /b %errorlevel%
