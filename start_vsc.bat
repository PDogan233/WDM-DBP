@echo off
chcp 65001 > nul
setlocal enabledelayedexpansion

echo ============================================
echo   Launching VSCode - ZKIC_project (MSVC x64)
echo ============================================
echo.

REM --- Step 1: Find VS 2022 using vswhere ---
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VCVARS="

if exist "%VSWHERE%" (
    echo [1/3] Searching for Visual Studio 2022...
    for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath 2^>nul`) do (
        set "VS_PATH=%%i"
    )
    if defined VS_PATH (
        set "VCVARS=!VS_PATH!\VC\Auxiliary\Build\vcvars64.bat"
        if exist "!VCVARS!" (
            echo         Found: !VS_PATH!
        )
    )
)

REM Fallback: try common paths
if not defined VCVARS (
    echo [1/3] vswhere not found, trying common paths...
    for %%d in (
        "D:\Microsoft Visual Studio\Community"
        "C:\Program Files\Microsoft Visual Studio\2022\Community"
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise"
        "C:\Program Files\Microsoft Visual Studio\2022\Professional"
    ) do (
        if exist "%%~d\VC\Auxiliary\Build\vcvars64.bat" (
            set "VCVARS=%%~d\VC\Auxiliary\Build\vcvars64.bat"
            echo         Found: %%~d
        )
    )
)

if not defined VCVARS (
    echo [ERROR] Cannot find Visual Studio 2022 with C++ toolchain.
    echo         Please install VS 2022 or update start_vsc.bat manually.
    pause
    exit /b 1
)

echo.
echo [2/3] Setting up MSVC build environment...
call "%VCVARS%"
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] vcvars64.bat failed!
    pause
    exit /b 1
)

echo.
echo [3/3] Launching VSCode...
echo.
echo Environment ready: MSVC + CMake + MSBuild
echo Ctrl+Shift+B to build, F5 to debug
echo.

code .

exit
