./autogen.sh
./configure --host i486-mingw32
make
install -pvs build/4dtris.exe bin/
make clean
