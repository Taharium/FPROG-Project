@echo off
mkdir build
pushd build
cl ../main2.cpp /EHsc /std:c++20 /ZI /DEBUG
popd
devenv .\build\main2.exe