@echo off
setlocal

:: Build the asstudio CLI (Odin)
:: Usage: tools\build-cli.bat

:: Resolve the project root directory (one level up from this script)
pushd "%~dp0.."
set "ROOT=%CD%"

:: Create build directory if it doesn't exist
if not exist "build" mkdir "build"

:: Compile the Odin project with .exe extension
odin build src/app ^
    -out:build/asstudio.exe ^
    -collection:src=src ^
    -o:speed

:: Exit immediately if the build failed (replaces 'set -e')
if %ERRORLEVEL% neq 0 (
    popd
    exit /b %ERRORLEVEL%
)

echo built: %ROOT%\build\asstudio.exe
echo try: .\build\asstudio.exe validate samples\hello.json
echo      .\build\asstudio.exe generate samples\hello.json -o generated\hello

popd
endlocal
