@echo off
mkdir build
pushd build
cl ../main.cpp /EHsc /std:c++20 /I ../include/ /O2
popd