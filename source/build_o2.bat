@echo off

if not exist build ( mkdir build )
pushd build

cl /nologo /w /I..\..\lib64 /Fetasks /O2 ..\main.cpp ..\..\lib64\*.lib

move calendar.exe ..\..

popd