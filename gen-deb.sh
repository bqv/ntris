#!/bin/bash
DEBEMAIL=laszlo.simon@gmail.com
DEBFULLNAME=Laszlo Simon

dh_make -c gpl3 -n -s -e $DEBEMAIL

mv debian/control debian/control.bak
perl -pe 's/Section:.*/Section: games/; \
          s|Homepage:.*|Homepage: https://launchpad.net/4dtris|; \
          s/Description:.*/Description: Tetris like game in 4D/; \
          s/ <.*/ This Application is a Tetris game extended to 4 dimension using OpenGL and GLUT./;' \
  <debian/control.bak >debian/control

mv debian/menu.ex debian/menu.bak
perl -pe 's/X11\|text\|vc\|wm/X11/; s/see-menu-manual/Games/;' \
  <debian/menu.bak >debian/menu.ex;

fakeroot debian/rules clean
fakeroot debian/rules build
fakeroot debian/rules binary
debuild -us -uc

