@echo off

if not exist build ( mkdir build )
pushd build

cl /nologo /w /I..\..\lib64 /Fecalendar /Od /Zi ..\main.cpp ..\..\lib64\*.lib

popd