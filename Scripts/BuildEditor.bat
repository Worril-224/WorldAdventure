@echo off
setlocal
set "UE_DIR=C:\Program Files\Epic Games\UE_5.8"
set "DOTNET_ROOT=%UE_DIR%\Engine\Binaries\ThirdParty\DotNet\10.0\win-x64"
set "PATH=%DOTNET_ROOT%;%PATH%"
"%UE_DIR%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" WorldAdventureEditor Win64 Development -2022 -Project="%~dp0..\WorldAdventure.uproject" -WaitMutex
exit /b %ERRORLEVEL%
