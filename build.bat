@echo off
mkdir build
pushd build
cl ../main2.cpp /EHsc /std:c++20 /O2
popd