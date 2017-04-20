#!/bin/sh
g++ -c main.cpp -o main.o
g++ -c states.cpp -o states.o
g++ -c keys.cpp -o keys.o
g++ main.o states.o keys.o -o remorse.out -lGL -lGLU -lglut
g++ physics.cpp -lBox2D -lGL