@echo off
setlocal enabledelayedexpansion

REM Find glslc: prefer PATH, then VULKAN_SDK
where glslc >nul 2>&1
if %errorlevel%==0 (
  set GLSLC=glslc
) else if defined VULKAN_SDK if exist "%VULKAN_SDK%\Bin\glslc.exe" (
  set "GLSLC=%VULKAN_SDK%\Bin\glslc.exe"
) else (
  echo Error: glslc not found. Ensure it is in PATH or set VULKAN_SDK.
  exit /b 1
)

REM Compile all .vert and .frag files in this directory
for %%F in (*.vert *.frag) do (
  if exist "%%F" (
    "%GLSLC%" "%%F" -o "%%F.spv"
  )
)

echo Shader compilation complete.
pause