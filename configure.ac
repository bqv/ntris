
AC_PREREQ([2.65])

AC_INIT([4dtris], [1.0.0], [https://github.org/frony0/4dtris])

AC_CONFIG_SRCDIR([src])

AM_INIT_AUTOMAKE

# Checks for programs.

AC_PROG_CC

# Checks for libraries.

AC_CHECK_LIB([fontconfig], [FcFontList])
AC_CHECK_LIB([GL], [glBegin])
AC_CHECK_LIB([GLU], [gluQuadricNormals])
AC_CHECK_LIB([SDL], [SDL_Init])
AC_CHECK_LIB([SDL_ttf], [TTF_RenderUTF8_Blended])
AC_CHECK_LIB([m], [sqrt])

# Checks for header files.

AC_CHECK_HEADERS([stdlib.h string.h math.h stdio.h limits.h GL/gl.h GL/glu.h SDL/SDL.h SDL/SDL_ttf.h locale.h fontconfig/fontconfig.h]

# Checks for typedefs, structures, and compiler characteristics.

AC_CHECK_FUNCS([pow setlocale sqrt])

AC_C_INLINE

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
