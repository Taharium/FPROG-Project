@echo off
REM call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 > nul

mkdir build
pushd build
cl ../main2.cpp /EHsc /std:c++20 /O2 /Ob3 /Oi /GL
popd