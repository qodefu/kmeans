#!/usr/bin/env bash

clang++ -ggdb \
    -I./raylib \
    -o kmeans kmeans.cc \
    -L./raylib \
    -lraylib -lm

clang++ -ggdb \
    -I./raylib \
    -o 3d 3d.cc \
    -L./raylib \
    -lraylib -lm

##if [[ $? -eq 0 ]]
##then
##
##  ./3d
##else
##    echo "build failure"
##fi
