#!/bin/sh

if [ $1 = 'build' ]
    then
    export CC=/usr/bin/clang
    export CXX=/usr/bin/clang++
    rm -f -r build
    mkdir build
    cd build
    cmake ..
    make
    cd ..
fi

if [ $1 = 'test' ]
    then
    cd build
    make test
    cd ..
fi

if [ $1 = 'clean' ]
    then
    #clean build
    rm -f -r build
fi
