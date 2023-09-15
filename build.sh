# build script for linux/mac (clang)
#!/usr/bin/env bash

# recursive file searching and compilation routine
compile() {
    local   FLAGS="-I./lib -std=gnu11 -Wall -Wextra -pedantic -fdiagnostics-color=always"
    local D_FLAGS="-g"
    local R_FLAGS="-DNDEBUG -Os -s"
    local BIN_DIR="./bin"
    local files=""

    for f in $(find -name "*.c" ! -iname "win.c"); do
        files="$files $f";
    done

    mkdir -p "$BIN_DIR"

    if [[ "$1" == "rel" ]]; then
        set -xe
        clang -o "$BIN_DIR/rffmpeg" $files $FLAGS $R_FLAGS
    else
        set -xe
        clang -o "$BIN_DIR/rffmpeg" $files $FLAGS $D_FLAGS
    fi
}

clean() {
    rm -rf "*.o" &> /dev/null
}

# entry point of the script
compile $1
clean
