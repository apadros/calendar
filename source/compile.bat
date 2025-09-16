@echo off

if %1.==. ( goto Help )

if not exist build ( mkdir build )
pushd build

cl /I..\..\lib64 ..\%1 /c

popd
exit /b

:Help
echo:
echo Format: compile.bat file.cpp
echo: