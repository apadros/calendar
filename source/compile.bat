@echo off

if not exist build (
  mkdir build
)
pushd build

cl /I..\..\lib64 ..\%1 /c

popd