@echo off

mkdir testBuild
pushd testBuild
wsl g++ -std=c++20 ../test.cpp -o test
popd testBuild

wsl ./testBuild/test

pause