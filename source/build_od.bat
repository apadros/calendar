@echo off

if not exist build ( mkdir build )
pushd build

cl /nologo /w /I..\..\..\API\release /Fecalendar /Od /Zi ..\main.cpp ..\..\..\API\release\*.lib

popd