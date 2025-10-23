@echo off

if not exist build ( mkdir build )
pushd build

cl /nologo /w /I..\..\lib64 /Fecalendar /Od /Zi ..\main.cpp ..\..\lib64\apad_error_v0_3_0.lib ..\..\lib64\apad_file_v0_3_0.lib ..\..\lib64\apad_string_v0_3_0.lib ..\..\lib64\apad_memory_v0_3_0.lib ..\..\lib64\apad_time_v0_3_0.lib

popd