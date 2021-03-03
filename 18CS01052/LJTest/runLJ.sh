#!/bin/bash
g++ -o LJDecompTest LJDecompTest.cpp
./LJDecompTest < in/LJ1.in > out/LJ1.out
./LJDecompTest < in/LJ2.in > out/LJ2.out
./LJDecompTest < in/LJ3.in > out/LJ3.out