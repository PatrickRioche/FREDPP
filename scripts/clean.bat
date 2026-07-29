@echo off
setlocal
cd /d "%~dp0\.."
if exist out\build rmdir /s /q out\build
if exist out\install rmdir /s /q out\install
echo Clean complete.
