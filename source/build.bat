@echo off

if not exist build (
	mkdir build
)
pushd build

cl /I..\..\lib64 /Fecalendar /O2 ..\main.cpp

popd