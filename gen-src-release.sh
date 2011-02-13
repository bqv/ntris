#!/bin/bash
VER=`echo $PWD | perl -ne '/(\d+\.\d+\.\d+)$/; print $1'`
wget -O - http://sourceforge.net/apps/wordpress/dtris/credits | \
  html2text | \
  awk '/=+/,/ Pages /' >AUTHORS 
wget -O - http://sourceforge.net/apps/wordpress/dtris | \
  html2text | \
  awk '/=+/,/ Pages /' | \
  perl -pe 's/\*{5}/\n/g' >NEWS 
wget -O - http://www.gnu.org/licenses/gpl-3.0.txt >COPYING 
bzr log >ChangeLog 
echo "see README" >INSTALL
ls po/*.po | perl -ne 'm,po/(.*)\.po,; print "$1 ";' > po/LINGUAS
./autogen.sh
autoscan
./configure
cd po; make update-po; cd ..
make distclean
for i in config.guess config.sub install-sh missing depcomp; do 
  unlink $i && cp /usr/share/automake-1.11/$i ./
done
cd ..
tar -cf 4dtris-$VER-src.tar 4dtris-$VER
gzip -c 4dtris-$VER-src.tar >4dtris-$VER-src.tar.gz
