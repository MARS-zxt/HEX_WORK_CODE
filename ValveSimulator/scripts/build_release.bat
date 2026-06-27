@echo off
REM ============================================================================
REM ValveSimulator Release Build + windeployqt + Inno Setup 打包
REM ============================================================================
REM 运行方式：从项目根目录执行  scripts\build_release.bat
REM 输出：
REM   build_rel\          — Release 构建 + Qt DLL
REM   installer\           — 安装包 .exe
REM ============================================================================
setlocal

set "PROJECT_ROOT=e:\work_space\HEX_WORK_CODE\ValveSimulator"
set "INNO_SETUP=D:\into_set\Inno Setup 6\ISCC.exe"

call "D:\vv_ss\VC\Auxiliary\Build\vcvars64.bat"
cd /d "%PROJECT_ROOT%"

echo ================================================================
echo   ValveSimulator Release Build + Package
echo ================================================================

:: ---- Stage 1: Clean + CMake Configure ----
echo.
echo === [1/4] Cleaning release build ===
if exist build_rel rmdir /s /q build_rel

echo === [2/4] CMake Configure (Release) ===
"C:\Qt\Tools\CMake_64\bin\cmake.exe" -B build_rel -G "Ninja" ^
  -DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/msvc2022_64 ^
  -DCMAKE_MAKE_PROGRAM=C:/Qt/Tools/Ninja/ninja.exe ^
  -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configure FAILED
    exit /b 1
)

:: ---- Stage 2: Build ----
echo.
echo === [3/4] Building Release ===
"C:\Qt\Tools\CMake_64\bin\cmake.exe" --build build_rel --config Release
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build FAILED
    exit /b 1
)

:: ---- Stage 3: windeployqt ----
echo.
echo === [4/4] Running windeployqt ===
cd build_rel
"C:\Qt\6.11.1\msvc2022_64\bin\windeployqt.exe" ValveSimulator.exe --no-translations --release
if %ERRORLEVEL% NEQ 0 (
    echo [WARNING] windeployqt returned non-zero (may be OK)
)
cd ..

echo.
echo ================================================================
echo   Build + Deploy SUCCESS
echo   Output: build_rel\ValveSimulator.exe
echo ================================================================

:: ---- Stage 5: Inno Setup (if available) ----
if exist "%INNO_SETUP%" (
    echo.
    echo === Running Inno Setup 6 ===
    "%INNO_SETUP%" scripts\setup.iss
    if %ERRORLEVEL% NEQ 0 (
        echo [ERROR] Inno Setup compile FAILED
        exit /b 1
    )
    echo.
    echo ================================================================
    echo   Installer generated:
    echo   %PROJECT_ROOT%\installer\ValveSimulator_Setup_v1.0.0.exe
    echo ================================================================
) else (
    echo.
    echo [INFO] Inno Setup 6 not found at "%INNO_SETUP%"
    echo [INFO] Skipping installer generation.
    echo [INFO] Ready to package manually. Files in: build_rel\
)

endlocal
