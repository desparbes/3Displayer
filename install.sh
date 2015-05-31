#!/bin/bash

if [[ $0 != ./`basename $0` ]]; then
    echo "DONT LAUNCH THIS SCRIPT OUTSIDE OF ITS DIRECTORY!"
    exit 1
fi

rm -rf 3Displayer
mkdir -p build
cd build
rm * -rf
cmake -DCMAKE_BUILD_TYPE=Debug -H.. -B.
make
make install
