#!/bin/bash
set -e

./build.sh
./build/Walrus "$@"
