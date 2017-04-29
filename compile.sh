#!/bin/sh
mkdir extra
g++ -c states.cpp -o extra/states.o
g++ -c keys.cpp -o extra/keys.o
g++ -c timer.cpp -o extra/timer.o
g++ -c  -std=c++11 physics.cpp -lBox2D -lGL -o extra/physics.o
g++ -c main.cpp -o extra/main.o
g++ extra/main.o extra/states.o extra/keys.o extra/timer.o extra/physics.o -o remorse.out -lBox2D -lGL -lGLU -lglut
rm -rf extra
