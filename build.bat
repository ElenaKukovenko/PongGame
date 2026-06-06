@echo off
chcp 65001 >nul
color 0A

echo ========================================
echo    PONG TENNIS GAME - BUILD SCRIPT
echo    Version 2.0 (with speed boost)
echo ========================================
echo.

echo Checking compiler...
where cl >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERROR] cl.exe not found!
    echo.
    echo Please run this script from:
    echo "Developer Command Prompt for VS 2022"
    echo.
    echo Or open: Start Menu -^> Visual Studio 2022 -^> 
    echo         Developer Command Prompt for VS 2022
    echo.
    pause
    exit /b 1
)

echo Compiler found. Building...
echo.

cl /nologo /W3 /O2 /Fe:PongGame.exe main.c win32_ui.c game_logic.c user32.lib gdi32.lib msimg32.lib

if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo    BUILD SUCCESSFUL!
    echo ========================================
    echo.
    echo Game features:
    echo   - Two players on one device
    echo   - WASD vs Arrow Keys
    echo   - Ball speed increases with each hit
    echo   - First to 5 points wins
    echo.
    echo Running PongGame.exe...
    echo.
    PongGame.exe
) else (
    echo.
    echo ========================================
    echo    BUILD FAILED!
    echo ========================================
    echo.
    echo Check errors above.
    echo.
    pause
)