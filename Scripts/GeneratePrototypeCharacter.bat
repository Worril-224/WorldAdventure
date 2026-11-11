@echo off
setlocal
if not defined UE_DIR set "UE_DIR=C:\Program Files\Epic Games\UE_5.8"
set "PROJECT_FILE=%~dp0..\WorldAdventure.uproject"
set "SCRIPT_FILE=%~dp0GeneratePrototypeCharacter.py"
set "EDITOR_EXE=%UE_DIR%\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"

if not exist "%EDITOR_EXE%" (
    echo UnrealEditor-Cmd.exe not found: "%EDITOR_EXE%"
    exit /b 1
)
if not exist "%PROJECT_FILE%" (
    echo Project not found: "%PROJECT_FILE%"
    exit /b 1
)
if not exist "%SCRIPT_FILE%" (
    echo Generator not found: "%SCRIPT_FILE%"
    exit /b 1
)

"%EDITOR_EXE%" "%PROJECT_FILE%" -run=pythonscript -script="%SCRIPT_FILE%" -unattended -nop4 -log
exit /b %ERRORLEVEL%
