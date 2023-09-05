# build script for linux/mac (clang)
#! /usr/bin/env bash

# recursive file searching and compilation routine
compile() {
    local   FLAGS="-std=gnu17 -Wall -pedantic -fdiagnostics-color=always"
    local D_FLAGS="-g"
    local R_FLAGS="-Os -s"
    local BIN_DIR="./bin"
    local files=""

    for f in $(find -name "*.c" ! -iname "win.c"); do
        files="$files $f";
    done

    mkdir -p "$BIN_DIR"

    if [[ "$1" == "rel" ]]; then
        clang -o "$BIN_DIR/rffmpeg" $files $FLAGS $R_FLAGS
    else
        clang -o "$BIN_DIR/rffmpeg" $files $FLAGS $D_FLAGS
    fi
}

clean() {
    rm -rf "*.o" &> /dev/null
}

# entry point of the script
compile $1
clean
