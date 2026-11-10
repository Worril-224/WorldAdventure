@echo off
setlocal
set "UE_DIR=C:\Program Files\Epic Games\UE_5.8"
set "DOTNET_ROOT=%UE_DIR%\Engine\Binaries\ThirdParty\DotNet\10.0\win-x64"
set "PATH=%DOTNET_ROOT%;%PATH%"
call "%UE_DIR%\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun -project="%~dp0..\WorldAdventure.uproject" -noP4 -platform=Win64 -clientconfig=Development -cook -allmaps -build -stage -pak -archive -archivedirectory="%~dp0..\Builds"
exit /b %ERRORLEVEL%
