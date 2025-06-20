@echo off

if not exist build (
	mkdir build
)
pushd build

cl /I..\..\lib64 /Fecalendar /Od /Zi ..\main.cpp ..\..\lib64\apad_error_v0_2_0.lib ..\..\lib64\apad_file_v0_2_0.lib

popd