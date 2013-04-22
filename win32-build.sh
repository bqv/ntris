#!/bin/sh -x 

astyle -z2 -c -A1 -s4 -q -n -r "*.c" "*.h"
./autogen.sh
./configure --host i486-mingw32
make
install -pvs build/4dtris.exe bin/
make clean
