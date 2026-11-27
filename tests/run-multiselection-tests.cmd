@echo off
setlocal
where cl.exe >nul 2>&1
if errorlevel 1 (
  echo Run this script from an x86 Visual Studio Developer Command Prompt.
  exit /b 2
)
pushd "%~dp0.."
if not exist Bin\tests mkdir Bin\tests
cl /nologo /std:c++14 /EHsc /W4 /WX /Fo:Bin\tests\MultiSelectionTests.obj /Fe:Bin\tests\MultiSelectionTests.exe tests\MultiSelectionTests.cpp
if errorlevel 1 (popd & exit /b 1)
Bin\tests\MultiSelectionTests.exe
set test_result=%errorlevel%
popd
exit /b %test_result%
