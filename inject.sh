#!/bin/bash

LIB_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/tf2.so"
PROCID=$(pgrep tf_linux64 | head -n 1)

if [ "$EUID" -ne 0 ]; then
    echo "Please run as root (current EUID: $EUID)"
    exit 1
fi

if [ -z "$PROCID" ]; then
    echo "Please open game (failed to find tf_linux64 process)"
    exit 1
fi

if [ ! -f "$LIB_PATH" ]; then
    echo "[DEBUG] Library file not found at: $LIB_PATH"
    exit 1
fi

unload() {
    echo -e "\nUnloading library with handle $LIB_HANDLE"
    
    UNLOAD_OUT=$(gdb -n --batch \
             -ex "set sysroot /proc/$PROCID/root" \
             -ex "attach $PROCID" \
             -ex "call ((int (*) (void *)) dlclose)((void *) $LIB_HANDLE)" \
             -ex "call ((char * (*) (void)) dlerror)()" \
             -ex "detach" 2>&1)
    RC=$(echo "$UNLOAD_OUT" | grep -oP '\$2 = 0x\K[0-9a-f]+')

    if [[ "$RC" == "0" ]]; then
        echo "Library unloaded successfully"
    else
        echo "Failed to unload library"
        echo "[DEBUG] PID: $PROCID, Handle: $LIB_HANDLE, dlerror pointer: ${RC:-<none>}"
        echo "[DEBUG] GDB output:"
        echo "$UNLOAD_OUT"
    fi
}

trap unload SIGINT

LOAD_OUT=$(gdb -n --batch \
                  -ex "set sysroot /proc/$PROCID/root" \
                  -ex "attach $PROCID" \
                  -ex "call ((void * (*) (const char*, int)) dlopen)(\"$LIB_PATH\", 1)" \
                  -ex "detach" 2>&1)
LIB_HANDLE=$(echo "$LOAD_OUT" | grep -oP '\$1 = \(void \*\) \K0x[0-9a-f]+')

if [ -z "$LIB_HANDLE" ]; then
    echo "Failed to load library"
    echo "[DEBUG] PID: $PROCID, Library path: $LIB_PATH"
    echo "[DEBUG] GDB output:"
    echo "$LOAD_OUT"
    exit 1
fi

if [[ "$LIB_HANDLE" = "0x0" ]]; then
    echo "Failed to load library at $LIB_HANDLE"
    ERR_OUT=$(gdb -n --batch \
               -ex "set sysroot /proc/$PROCID/root" \
               -ex "attach $PROCID" \
	       -ex "call ((char * (*) (void)) dlerror)()" \
               -ex "detach" 2>&1)
    ERR=$(echo "$ERR_OUT" | grep '\$1')
    echo "Result from dlerror: $ERR"
    echo "[DEBUG] PID: $PROCID, Library path: $LIB_PATH"
    echo "[DEBUG] GDB dlopen output:"
    echo "$LOAD_OUT"
    echo "[DEBUG] GDB dlerror output:"
    echo "$ERR_OUT"
    exit 1
fi

echo "Library loaded successfully at $LIB_HANDLE. Use Ctrl+C to unload."

tail -n +1 -f "/proc/$PROCID/root/tmp/tf2.log"
