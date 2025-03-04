#!/bin/bash

set -xe

# Build the lib first
cd ..
./build.sh
cd -


if [[ -z $1 ]]; then
    set +x
    echo "[ERROR] No main file provided. Available examples:"
    echo "[INFO ] simpleWindow"
    echo "[INFO ] multiWindow"
    echo "[INFO ] boxes"
    echo "[INFO ] boxesOverflow"
    echo "[INFO ] boxDividers"
    echo "[INFO ] buttonsInABox"
    echo "[INFO ] dropdowns"
    echo "[INFO ] images"
    echo "[INFO ] recyclelists"
    echo "[INFO ] slider"
    echo "[INFO ] contextmenu"
    exit
fi

rm -rf test_executables
cmake -DMAIN_FILE="$1" -B examples_artifacts -GNinja .
cmake --build examples_artifacts -j8

# Executable needs to be in the root dir as to have acces to assets directory
mv test_executables/$1 ../$1
rm -r test_executables
cd ..
./$1
rm $1
