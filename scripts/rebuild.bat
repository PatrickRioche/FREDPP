@echo off
setlocal
call "%~dp0clean.bat" || exit /b 1
call "%~dp0build.bat" || exit /b 1
call "%~dp0test.bat" || exit /b 1
echo.
echo FREDPP rebuild and tests completed successfully.
