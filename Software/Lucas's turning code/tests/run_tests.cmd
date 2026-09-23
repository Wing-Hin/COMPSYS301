@echo off
setlocal
pushd "%~dp0.."
where cl >nul 2>nul
if errorlevel 1 (
    for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do call "%%i\VC\Auxiliary\Build\vcvars64.bat" >nul
)
where cl >nul 2>nul
if errorlevel 1 goto fail
if not exist .build mkdir .build
rem Exercise the adapter against the real motorControl.h and mocked driver calls.
rem This does not link the actual generated firmware or emulate hardware.
cl /nologo /std:c11 /W4 /WX /TC /I. /Itests\psoc_stubs /I..\CS301_Class.cydsn /Fo.build\ /Fe.build\test_turn_hardware.exe turn_hardware.c tests\test_turn_hardware.c
if errorlevel 1 goto fail
.build\test_turn_hardware.exe
if errorlevel 1 goto fail
cl /nologo /std:c11 /W4 /WX /TC /I. /Fo.build\ /Fe.build\test_turn.exe turn.c tests\test_turn.c
if errorlevel 1 goto fail
.build\test_turn.exe
if errorlevel 1 goto fail
rem Exercise configurable values as well as the defaults.
cl /nologo /std:c11 /W4 /WX /TC /I. /DTURN_CONFIRM_READINGS=5 /DTURN_APPROACH_SPEED=17 /DTURN_ROTATE_SPEED=31 /DTURN_APPROACH_COUNTS=19 /DTURN_LEFT_ENCODER_FORWARD_SIGN=-1 /DTURN_TIMEOUT_MS=1500UL /Fo.build\ /Fe.build\test_turn_config.exe turn.c tests\test_turn.c
if errorlevel 1 goto fail
.build\test_turn_config.exe
if errorlevel 1 goto fail
popd
exit /b 0
:fail
popd
exit /b 1
