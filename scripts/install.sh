#!/bin/bash

cd $(dirname $0)
mkdir -p ../build/debug
cd ../build/debug
if [ ! -f Makefile ]; then
    rm -rf *
    cmake -DCMAKE_BUILD_TYPE=Debug -H../.. -B.
fi

make -s
make -s install 
