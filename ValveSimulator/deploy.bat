@echo off
call "D:\vv_ss\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
echo === Deploying Qt DLLs ===
cd /d e:\work_space\HEX_WORK_CODE\ValveSimulator\build
"C:\Qt\6.11.1\msvc2022_64\bin\windeployqt.exe" ValveSimulator.exe --no-translations
echo === Deploy done ===
echo === Running ValveSimulator ===
start ValveSimulator.exe
