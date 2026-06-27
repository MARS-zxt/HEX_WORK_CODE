@echo off
echo === Compiling C Test Client ===

call "D:\vv_ss\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: vcvars64.bat failed
    exit /b 1
)

cd /d "e:\work_space\HEX_WORK_CODE\ValveSimulator\external"

cl /nologo test_client_example.c ^
    ..\src\network\valve_api.cpp ^
    /I..\src\network ^
    ws2_32.lib ^
    /Fe:..\debug\my_test.exe

if %errorlevel% equ 0 (
    echo.
    echo === Compile SUCCESS ===
    echo Output: debug\my_test.exe
) else (
    echo.
    echo === Compile FAILED ===
    exit /b 1
)
