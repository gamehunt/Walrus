#!/bin/bash

if [ ! -d build ]; then
    cmake -B build
fi

cmake --build build
