bin_PROGRAMS = 4dtris
4dtris_SOURCES =  src/4dt_ai.c   \
                  src/4dt_eng.c  \
                  src/4dt_main.c \
                  src/4dt_menu.c \
                  src/4dt_scn.c  \
                  src/4dt_m3d.c  \
                  src/4dt_m4d.c  \
                  src/4dt_g3d.c  \
                  src/4dt_gtxt.c \
                  src/4dt_g4d.c  \
                  src/4dt_hst.c  \
                  src/4dt_ui.c   \
                  src/4dt_ai.h   \
                  src/4dt_eng.h  \
                  src/4dt_main.h \
                  src/4dt_menu.h \
                  src/4dt_scn.h  \
                  src/4dt_m3d.h  \
                  src/4dt_m4d.h  \
                  src/4dt_g3d.h  \
                  src/4dt_gtxt.h \
                  src/4dt_g4d.h  \
                  src/4dt_hst.h  \
                  src/4dt_ui.h
4dtris_LDADD = $(LIBOBJS)

EXTRA_DIST = config.rpath mkinstalldirs m4/Makefile.in m4/ChangeLog  4dtris.desktop doc/icon/4dtris.png

Applicationsdir = /usr/share/applications
Applications_DATA = 4dtris.desktop

icondir = /usr/share/games/4dtris
icon_DATA = doc/icon/4dtris.png

SUBDIRS = src po

ACLOCAL_AMFLAGS = -I m4

check-gettext:
	@if test x$(USE_NLS) != "xyes" ; then echo "Missing gettext. Rerun configure and check for" \
	"'checking whether to use NLS... yes'!" ; exit 1 ; fi

update-po: check-gettext
	@find $(srcdir)/src/ -name "*.c" -print | sort > $(srcdir)/po/POTFILES.in.2 ; \
	if diff $(srcdir)/po/POTFILES.in $(srcdir)/po/POTFILES.in.2 >/dev/null 2>&1 ; then \
		rm -f $(srcdir)/po/POTFILES.in.2 ; \
	else \
		mv $(srcdir)/po/POTFILES.in.2 $(srcdir)/po/POTFILES.in ; \
	fi
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-po

update-gmo: check-gettext
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-gmo

force-update-gmo: check-gettext
	touch po/*.po
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-gmo

force-update-gmo-%: check-gettext
	@language=`echo $@ | sed s/force-update-gmo-//` ; \
	if test ! -f po/$$language.po ; then echo "file po/$$language.po does not exist" ; exit 1 ; fi ; \
	touch po/$$language.po ; \
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-gmo

.PHONY: check-gettext update-po update-gmo force-update-gmo
