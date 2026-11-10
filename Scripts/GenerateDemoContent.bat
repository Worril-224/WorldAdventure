@echo off
setlocal
set "UE_DIR=C:\Program Files\Epic Games\UE_5.8"
"%UE_DIR%\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "%~dp0..\WorldAdventure.uproject" -run=pythonscript -script="%~dp0GenerateDemoContent.py" -unattended -nop4 -log
exit /b %ERRORLEVEL%
