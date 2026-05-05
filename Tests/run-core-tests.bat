@echo off
setlocal

set "ROOT=%~dp0.."
set "VCVARS=%ProgramFiles%\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"

if not exist "%VCVARS%" (
    echo Could not find MSVC environment setup at:
    echo %VCVARS%
    exit /b 1
)

call "%VCVARS%"
if errorlevel 1 exit /b %errorlevel%

cl /std:c++20 /EHsc /W4 /permissive- /I"%ROOT%" "%~dp0CoreTests.cpp" /Fo:"%~dp0CoreTests.obj" /Fe:"%~dp0CoreTests.exe"
if errorlevel 1 exit /b %errorlevel%

"%~dp0CoreTests.exe"
