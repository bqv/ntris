#!/bin/sh -x 

astyle -z2 -c -A1 -s4 -q -n -r "*.c" "*.h"
./autogen.sh
./configure
make
install -pv build/ntris bin/
make clean
