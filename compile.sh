#!/bin/sh
mkdir extra
g++ -c main.cpp -o extra/main.o
g++ -c states.cpp -o extra/states.o
g++ -c keys.cpp -o extra/keys.o
g++ -c timer.cpp -o extra/timer.o
g++ -c values.cpp -o extra/values.o
g++ extra/main.o extra/states.o extra/keys.o extra/timer.o extra/values.o -o remorse.out -lGL -lGLU -lglut
rm -rf extra
