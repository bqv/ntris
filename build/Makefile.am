bin_PROGRAMS = ntris
ntris_SOURCES =  ../src/ai.c    \
                 ../src/ai.h    \
                 ../src/eng.c   \
                 ../src/eng.h   \
                 ../src/main.c  \
                 ../src/menu.c  \
                 ../src/menu.h  \
                 ../src/scn.c   \
                 ../src/scn.h   \
                 ../src/m.c     \
                 ../src/m.h     \
                 ../src/m3d.c   \
                 ../src/m3d.h   \
                 ../src/m4d.c   \
                 ../src/m4d.h   \
                 ../src/g3d.c   \
                 ../src/g3d.h   \
                 ../src/gtxt.c  \
                 ../src/gtxt.h  \
                 ../src/g4d.c   \
                 ../src/g4d.h   \
                 ../src/hst.c   \
                 ../src/hst.h   \
                 ../src/ui.c    \
                 ../src/ui.h    \
                 ../src/mou.c   \
                 ../src/mou.h   \
                 ../src/conf.c  \
                 ../src/conf.h  \
                 ../src/timer.c \
                 ../src/timer.h
ntris_LDADD = $(LIBOBJS)

EXTRA_DIST = config.rpath m4/ChangeLog mkinstalldirs m4/Makefile.in ntris.desktop doc/icon/ntris.png

Applicationsdir = /usr/share/applications
Applications_DATA = ../res/ntris.desktop

bindir = /usr/bin/

man_MANS = ../res/ntris.6

icondir = /usr/share/ntris
icon_DATA = ../doc/icon/ntris.png

ACLOCAL_AMFLAGS = -I m4
