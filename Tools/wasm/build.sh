#!/usr/bin/env bash
set -euo pipefail

STUBS=/ardupilot/Tools/wasm/stubs

# --- Parse arguments ---
# First positional arg is the vehicle target (copter, plane, rover, sub, heli,
# antennatracker, blimp).  Remaining args are forwarded to ./waf configure.
VEHICLE="${1:-copter}"
shift 2>/dev/null || true

# Map vehicle name to its waf EXPORT_NAME (used for the JS module name)
case "$VEHICLE" in
    copter)         EXPORT_NAME="ArduCopter" ;;
    heli)           EXPORT_NAME="ArduCopterHeli" ;;
    plane)          EXPORT_NAME="ArduPlane" ;;
    rover)          EXPORT_NAME="ArduRover" ;;
    sub)            EXPORT_NAME="ArduSub" ;;
    antennatracker) EXPORT_NAME="AntennaTracker" ;;
    blimp)          EXPORT_NAME="ArduBlimp" ;;
    *)              EXPORT_NAME="ArduPilot" ;;
esac

echo "=== Building: $VEHICLE  (module: $EXPORT_NAME) ==="

git submodule update --init --recursive \
    modules/mavlink \
    modules/waf \
    modules/DroneCAN \
    modules/littlefs \
    modules/lwip \
    modules/gtest

PTHREAD_STUB=/tmp/pthread_stubs.o
if [ ! -f "$PTHREAD_STUB" ]; then
    emcc -pthread -c "$STUBS/pthread_stubs.c" -o "$PTHREAD_STUB"
fi

# -isystem stubs: provide missing linux/can.h, asm/ioctls.h, fenv.h wrapper
# -DUSE_TERMIOS: use termios.h instead of asm/ioctls.h in UART code
# -pthread: waf's SITL board adds -pthread to LINKFLAGS. Objects must also be compiled with it so emscripten enables atomics/shared-memory
EXTRA="-isystem $STUBS -Wno-limited-postlink-optimizations -Wno-unused-variable -Wno-unused-but-set-variable -DUSE_TERMIOS -pthread"

export CFLAGS="$EXTRA"
export CXXFLAGS="$EXTRA"
export LDFLAGS="$PTHREAD_STUB -pthread -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=1 -s MODULARIZE=1 -s EXPORT_NAME=$EXPORT_NAME -s ENVIRONMENT=web,worker -s INITIAL_MEMORY=33554432 -s EXPORTED_RUNTIME_METHODS=[callMain,cwrap,ccall,getValue,setValue,stackSave,stackRestore,stackAlloc,HEAPU8,FS] -s PROXY_TO_PTHREAD=1 -s PTHREAD_POOL_SIZE=4"

# /usr/bin/size can't read wasm. Point waf at /bin/true so build_summary
# gets empty output and parses zero size rows instead of erroring.
export SIZE=/bin/true

./waf configure \
    --board sitl \
    --toolchain native \
    --disable-scripting \
    --disable-networking \
    --out build/wasm \
    "$@"

# --- build ---
./waf "$VEHICLE" -j"$(nproc)"