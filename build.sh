# build script for linux/mac (clang)
#!/usr/bin/env bash

# recursive file searching and compilation routine
compile() {
    local   FLAGS="-I./lib -std=gnu11 -Wall -Wextra -pedantic"
    local D_FLAGS="-g"
    local P_FLAGS="-finstrument-functions -ldl -rdynamic -DCLANG_INSTR_FUNCS"
    local R_FLAGS="-DNDEBUG -O2 -s"
    local   files=""

    for f in $(find -name "*.c"); do
        files="$files $f";
    done

    mkdir -p "./bin"

    if [[ "$1" == "rel" ]]; then
        set -x
        clang -o "bin/rffmpeg" $files $FLAGS $R_FLAGS
    elif [[ "$1" == "prof" ]]; then
        set -x
        clang -o "bin/rffmpeg" $files $FLAGS $D_FLAGS $P_FLAGS
    else
        set -x
        clang -o "bin/rffmpeg" $files $FLAGS $D_FLAGS
    fi

    set +x
}

# clean() {
#     rm -rf "*.o" &> /dev/null
# }

# entry point of the script
compile $1
