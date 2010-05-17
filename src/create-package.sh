#!/bin/bash
aclocal
autoconf
autoreconf (autoreconf -f -i -Wall,no-obsolete)
automake --add-missing
dh_make -n -s -e laszlo.simon@gmail.com	
fakeroot debian/rules clean
fakeroot debian/rules build
fakeroot debian/rules binary
debuild -us -uc

