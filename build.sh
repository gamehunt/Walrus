#!/bin/bash
set -e

if [ ! -d build ]; then
    cmake -B build
fi

cmake --build build
