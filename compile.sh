#!/bin/sh
mkdir extra
g++ -c main.cpp -o extra/main.o
g++ -c states.cpp -o extra/states.o
g++ -c keys.cpp -o extra/keys.o
g++ -c timer.cpp -o extra/timer.o
g++ extra/main.o extra/states.o extra/keys.o extra/timer.o -o remorse.out -lGL -lGLU -lglut
rm -rf extra