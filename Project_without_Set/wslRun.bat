@echo off

mkdir buildG++
pushd buildG++
wsl g++ -std=c++20 ../main.cpp -o main -Ofast
popd buildG++

wsl ./buildG++/main

pause