@echo off
setlocal

cd /d "%~dp0"

echo Building the inventory backend...
call build.bat
if errorlevel 1 (
    echo.
    echo Build failed. Press any key to close.
    pause >nul
    exit /b 1
)

if not exist "warehouse.exe" (
    echo.
    echo warehouse.exe was not created. Press any key to close.
    pause >nul
    exit /b 1
)

echo Starting the dashboard at http://localhost:3000/
start "" http://localhost:3000/
node web\server.js

