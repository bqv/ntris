#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([4DTris], [0.2.5], [https://sourceforge.net/tracker/?group_id=214738])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([4DTris_AI.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([GL], [glBegin])
AC_CHECK_LIB([GLU], [gluBeginCurve])
AC_CHECK_LIB([glut], [glutInit])
AC_CHECK_LIB([m], [sqrt])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_CHECK_FUNCS([sqrt])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
