bin_PROGRAMS = 4dtris
4dtris_SOURCES =  src/4dt_ai.c    \
                  src/4dt_eng.c   \
                  src/4dt_main.c  \
                  src/4dt_menu.c  \
                  src/4dt_scn.c   \
                  erc/4dt_m.c     \
                  src/4dt_m3d.c   \
                  src/4dt_m4d.c   \
                  src/4dt_g3d.c   \
                  src/4dt_gtxt.c  \
                  src/4dt_g4d.c   \
                  src/4dt_hst.c   \
                  src/4dt_ui.c    \
                  src/4dt_conf.c  \
                  src/4dt_mou.c   \
                  src/4dt_timer.c \
                  src/4dt_ai.h    \
                  src/4dt_eng.h   \
                  src/4dt_menu.h  \
                  src/4dt_scn.h   \
                  src/4dt_m.h     \
                  src/4dt_m3d.h   \
                  src/4dt_m4d.h   \
                  src/4dt_g3d.h   \
                  src/4dt_gtxt.h  \
                  src/4dt_g4d.h   \
                  src/4dt_hst.h   \
                  src/4dt_ui.h    \
                  src/4dt_conf.h  \
                  src/4dt_mou.h   \
                  src/4dt_timer.h
4dtris_LDADD = $(LIBOBJS)

EXTRA_DIST = config.rpath m4/ChangeLog mkinstalldirs m4/Makefile.in 4dtris.desktop doc/icon/4dtris.png

Applicationsdir = /usr/share/applications
Applications_DATA = 4dtris.desktop

bindir = /usr/games

man_MANS = 4dtris.6

icondir = /usr/share/games/4dtris
icon_DATA = doc/icon/4dtris.png

SUBDIRS = po

ACLOCAL_AMFLAGS = -I m4
