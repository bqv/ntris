#!/bin/bash
VER="0.3.1"
wget -O - https://launchpad.net/~4dtris-dev/+members | html2text >AUTHORS 
wget -O - http://sourceforge.net/apps/wordpress/dtris | html2text >NEWS 
wget -O - http://www.gnu.org/licenses/gpl-3.0.txt >COPYING 
bzr log >ChangeLog 
echo "see README" >INSTALL
aclocal \
&& automake --gnu --add-missing \
&& autoconf
autoscan

unlink install-sh && cp /usr/share/automake-1.11/install-sh .
unlink missing && cp /usr/share/automake-1.11/missing .
unlink depcomp && cp /usr/share/automake-1.11/depcomp .

cd ..
tar -cf 4dtris-$VER-src.tar 4dtris-$VER
gzip -c 4dtris-$VER-src.tar >4dtris-$VER-src.tar.gz