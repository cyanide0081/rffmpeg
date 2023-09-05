# build script for linux/mac (clang)
#! /usr/bin/env bash

# recursive file searching and compilation routine
compile() {
    local   FLAGS="-std=gnu17 -Wall -pedantic -fdiagnostics-color=always"
    local D_FLAGS="-g"
    local R_FLAGS="-Os -s"
    local     BIN="./build/rffmpeg"
    local files=""

    for f in $(find -name "*.c"); do
        files="$files $f";
    done

    mkdir -p "./build"

    if [[ "$1" == "rel" ]]; then
        clang -o $BIN $files $FLAGS $R_FLAGS
    else
        clang -o $BIN $files $FLAGS $D_FLAGS
    fi
}

clean() {
    rm -rf "*.o" &> /dev/null
}

# entry point of the script
compile $1
clean
