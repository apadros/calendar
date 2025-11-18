@echo off

if not exist build ( mkdir build )
pushd build

cl /I..\..\..\API\release /c /w /nologo ..\main.cpp

popd
exit /b