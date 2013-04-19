
AC_PREREQ([2.65])

AC_INIT([4dtris], [1.0.0], [https://github.org/frony0/4dtris])

AC_CONFIG_SRCDIR([src])

AM_INIT_AUTOMAKE

# Checks for programs.

AC_PROG_CC

# Checks for libraries.

AC_CHECK_LIB([fontconfig], [FcFontList])
AC_CHECK_LIB(GL, glBegin,
			 OPENGL_LIBS="-lGL",
			 AC_CHECK_LIB(opengl32, main,
						  OPENGL_LIBS="-lopengl32",
						  AC_MSG_ERROR([OpenGL library not found.])))
AC_CHECK_LIB(GLU, gluQuadricNormals,
			 OPENGL_LIBS="${OPENGL_LIBS} -lGLU",
			 AC_CHECK_LIB(glu32, main,
						  OPENGL_LIBS="${OPENGL_LIBS} -lglu32",
						  AC_MSG_ERROR([GLU library not found.])))
LIBS="${LIBS} ${OPENGL_LIBS}"
AC_CHECK_LIB([mingw32], [main],
			 LIBDEPS="4dtris.res -mwindows -lmingw32")
AC_CHECK_LIB([SDL], [SDL_Init])
AC_CHECK_LIB([SDLmain], [main])
AC_CHECK_LIB([SDL_ttf], [TTF_RenderUTF8_Blended])
AC_CHECK_LIB([m], [sqrt])
LIBS="${LIBDEPS} ${LIBS}"

# Checks for header files.

AC_CHECK_HEADERS([stdlib.h string.h math.h stdio.h limits.h GL/gl.h GL/glu.h SDL/SDL.h SDL/SDL_ttf.h locale.h fontconfig/fontconfig.h])

# Checks for typedefs, structures, and compiler characteristics.

AC_CHECK_FUNCS([pow setlocale sqrt])

AC_C_INLINE

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
