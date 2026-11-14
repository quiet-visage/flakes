#!/bin/bash

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

# Make a directory for our third-party libraries
if [ ! -d "vendor/" ]; then
    mkdir -p vendor
fi

# Clone and build funchook, if not done already.
if [ -d "vendor/funchook" ] && [ ! -f "vendor/funchook/libfunchook.a" ] && [ ! -f "vendor/funchook/libdistorm.a" ]; then
    rm -rf vendor/funchook
fi

if [ ! -d "vendor/funchook" ]; then
    mkdir -p vendor/funchook
    git clone https://github.com/Doctor-Coomer/funchook.git vendor/funchook/
    mkdir -p vendor/funchook/build
    cd vendor/funchook
    cmake -DCMAKE_BUILD_TYPE=Release .
    make
fi

# Build instrumentation library
cd "$PROJECT_ROOT"
make clean && make
if [ -f "tf2.so" ]; then
    if [[ "$(execstack -q tf2.so)" = "X tf2.so" ]]; then
	execstack -c tf2.so
    fi
fi
