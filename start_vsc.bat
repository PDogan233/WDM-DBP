@echo off
chcp 65001 > nul
echo ============================================
echo   Launching VSCode - ZKIC_project (MSVC x64)
echo ============================================

if not exist "D:\Microsoft Visual Studio\Community\VC\Auxiliary\Build\vcvars64.bat" (
    echo [ERROR] Cannot find Visual Studio 2022 vcvars64.bat
    echo Please check: D:\Microsoft Visual Studio\Community\VC\Auxiliary\Build\vcvars64.bat
    pause
    exit /b 1
)

echo [1/2] Setting up MSVC build environment...
call "D:\Microsoft Visual Studio\Community\VC\Auxiliary\Build\vcvars64.bat"
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] vcvars64.bat failed
    pause
    exit /b 1
)

echo [2/2] Launching VSCode...
echo.
echo Environment ready: MSVC + CMake + MSBuild
echo Ctrl+Shift+B to build, F5 to debug
echo.

code .

exit
