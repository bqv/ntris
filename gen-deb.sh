#!/bin/bash

function prepare_source_pack {
  if [ $(ls 4dtris-*-src.tar.gz | wc -l) -ne "1" ]
  then 
    echo "There should be one 4dtris-*-src.tar.gz"
    exit 1
  fi
  rename 's/4dtris-(\d+\.\d+\.\d+)-src.tar.gz/4dtris_$1.orig.tar.gz/' 4dtris-*-src.tar.gz
}

function create_debian_dir {
  rm -rf debian
  dh_make -c gpl3 -n -s -e $DEBEMAIL
  mv debian/control debian/control.bak
  perl -pe 's/Section:.*/Section: games/; \
            s|Homepage:.*|Homepage: https://launchpad.net/4dtris|; \
            s/3\.9\.2/3\.9\.3/; \
            s/Description:.*/Description: Tetris like game in 4D/; \
            s/Build-Depends:.*/Build-Depends: debhelper (>= 8), autotools-dev, libtool, gettext, libsdl1.2-dev, libsdl-ttf2.0-dev, libgl1-mesa-dev, libglu1-mesa-dev, libfontconfig-dev/; \
            s/^ <.*/ This Application is a Tetris game extended to 4 dimension using OpenGL and SDL./;' \
    <debian/control.bak >debian/control
  mv debian/menu.ex debian/menu.bak
  perl -pe 's/X11\|text\|vc\|wm/X11/; s/Applications\/see-menu-manual/Games\/Blocks/; \ 
            s/usr\/bin/usr\/games/' \
    <debian/menu.bak >debian/menu
  rm -rf debian/*.ex
  rm -rf debian/*.EX
  rm -rf debian/README.*
  perl -i -pe "s/unstable/$DISTRO/; s/$VER/$VER-$DEBVER/" debian/changelog
  perl -i -pe 's/^#.*//; \
               s/<years>/2012/g; \
               s/<url:\/\/example\.com>/https:\/\/launchpad.net\/4dtris/; \
               s/<put.*?here>/Laszlo Simon <laszlo.simon@gmail.com>/; \
               s/<likewise.*?author>/Laszlo Simon <laszlo.simon@gmail.com>/;' debian/copyright
  perl -i -pe 's/$ENV{USER}/Laszlo Simon/' debian/*
  echo "Check debian directory"
  read
}

DEBEMAIL="laszlo.simon@gmail.com"
EMAIL=$DEBEMAIL
DEBFULLNAME="Laszlo Simon"
NAME=$DEBFULLNAME
./gen-src-release.sh
cd ..
prepare_source_pack
VER=$(ls 4dtris_*.orig.tar.gz | perl -pe 's/.*_(\d+\.\d+\.\d+)\..*/$1/' )
cd 4dtris-$VER
echo "Distribution:"
read DISTRO
echo "Deb version:"
read DEBVER
create_debian_dir

fakeroot debian/rules clean
fakeroot debian/rules build
fakeroot debian/rules binary
# debuild -us -uc # without PGP signature
# debuild -B # build binary package
debuild -S
cd ..
echo "upload: dput ppa:4dtris-dev/all-releases 4dtris_..._source.changes"
# dput mentors 4dtris....changes
