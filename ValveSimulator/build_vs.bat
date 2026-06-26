@echo off
REM Build ValveSimulator with VS 2022 + Qt 6.11.1
call "D:\vv_ss\VC\Auxiliary\Build\vcvars64.bat"
cd /d e:\work_space\HEX_WORK_CODE\ValveSimulator

echo === Cleaning build directory ===
if exist build rmdir /s /q build

echo === Running CMake configure ===
"C:\Qt\Tools\CMake_64\bin\cmake.exe" -B build -G "Ninja" ^
  -DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/msvc2022_64 ^
  -DCMAKE_MAKE_PROGRAM=C:/Qt/Tools/Ninja/ninja.exe

if %ERRORLEVEL% NEQ 0 (
    echo === CMake configure FAILED ===
    exit /b 1
)

echo === Building ===
"C:\Qt\Tools\CMake_64\bin\cmake.exe" --build build --config Release

if %ERRORLEVEL% NEQ 0 (
    echo === Build FAILED ===
    exit /b 1
)

echo === Build SUCCESS ===
dir build\ValveSimulator.exe 2>nul
