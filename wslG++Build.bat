@echo off

mkdir buildG++
pushd buildG++
wsl g++ -I ../include/ -std=c++20 ../main.cpp -o main -O5
popd buildG++