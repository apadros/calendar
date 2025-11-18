@echo off

if not exist build ( mkdir build )
pushd build

cl /nologo /w /I..\..\..\API\release /Fecalendar /O2 ..\main.cpp ..\..\..\API\release\*.lib

move calendar.exe ..\..

popd