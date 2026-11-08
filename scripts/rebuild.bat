@echo off
setlocal
rem Rebuild only the configured FREDPP Debug targets; never erase the whole
rem source checkout's out/build or out/install trees.
call "%~dp0build.bat" --clean-first || exit /b 1
call "%~dp0test.bat" || exit /b 1
echo.
echo FREDPP rebuild and tests completed successfully.
exit /b 0
