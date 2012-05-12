#!/bin/bash
UpStrmDir=$PWD
echo 'Version number:'
read VER
bzr log >ChangeLog 
cd ..
mkdir 4dtris-$VER
cd 4dtris-$VER
cp -R $UpStrmDir/* .
wget -O - http://sourceforge.net/apps/wordpress/dtris/credits | \
  html2text | \
  awk '/=+/,/ Pages /' >AUTHORS 
wget -O - http://sourceforge.net/apps/wordpress/dtris | \
  html2text | \
  awk '/=+/,/ Pages /' | \
  perl -pe 's/\*{5}/\n/g' >NEWS 
wget -O - http://www.gnu.org/licenses/gpl-3.0.txt >COPYING 
echo "see README" >INSTALL
echo "Check ChangeLog, AUTHORS, NEWS, COPYING - then press enter"
read
ls po/*.po | perl -ne 'm,po/(.*)\.po,; print "$1 ";' > po/LINGUAS
./autogen.sh
autoscan
./configure
cd po; make update-po; cd ..
make distclean
for i in config.guess config.sub install-sh missing depcomp; do 
  unlink $i && cp /usr/share/automake-1.11/$i ./
done
rm -rf *.cache
cd ..
tar -cf 4dtris-$VER-src.tar 4dtris-$VER
gzip -c 4dtris-$VER-src.tar >4dtris-$VER-src.tar.gz
