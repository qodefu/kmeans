#!/usr/bin/env bash

clang++ -ggdb \
    -I./raylib \
    -o ${1} "${1}.cc" \
    -L./raylib \
    -lraylib -lm -lz


if [[ $? -eq 0 ]]
then

    ./${1} ${2} ${3} ${4}
else
    echo "build failure"
fi
