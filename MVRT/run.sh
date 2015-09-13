#! /bin/bash
# Compiles and runs boost files

filename="${@%.*}"

g++ $filename.cpp -o $filename -I /usr/local/lib -lboost_system; sudo ./$filename
