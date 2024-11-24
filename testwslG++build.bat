@echo off

mkdir buildG++
pushd buildG++
wsl g++ -I ../include -std=c++20 ../test.cpp -o test -O5
popd buildG++