@echo off

mkdir buildG++
pushd buildG++
wsl g++ -std=c++20 ../main2.cpp -o main -O5
popd buildG++