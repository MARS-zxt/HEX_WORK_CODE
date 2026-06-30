@echo off
call "D:\vv_ss\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
cd /d "e:\work_space\HEX_WORK_CODE\ValveSimulator\external"
echo === Compiling test_client_example.c ===
cl.exe /nologo /W3 /O2 /utf-8 /TP /I..\src\network test_client_example.c ..\src\network\valve_api.cpp /Fe:my_test.exe /link ws2_32.lib
if %ERRORLEVEL% EQU 0 (
    echo === Compile SUCCESS ===
    dir my_test.exe
    echo.
    echo === Deploying to build directories ===
    copy /Y my_test.exe "..\build\"   >nul 2>&1 && echo   OK: build\
    copy /Y my_test.exe "..\debug\"   >nul 2>&1 && echo   OK: debug\
    echo === Done ===
) else (
    echo === Compile FAILED ===
)
