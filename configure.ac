AC_PREREQ(2.52)

AC_INIT(libmgi,2.0.3,-)
AC_CONFIG_SRCDIR([src/mgime.c])

MGi_MAJOR_VERSION=2
MGi_MINOR_VERSION=0
MGi_MICRO_VERSION=3
MGi_INTERFACE_AGE=0
MGi_BINARY_AGE=0
MGi_VERSION=$MGi_MAJOR_VERSION.$MGi_MINOR_VERSION.$MGi_MICRO_VERSION

AC_SUBST(MGi_MAJOR_VERSION)
AC_SUBST(MGi_MINOR_VERSION)
AC_SUBST(MGi_MICRO_VERSION)
AC_SUBST(MGi_INTERFACE_AGE)
AC_SUBST(MGi_BINARY_AGE)
AC_SUBST(MGi_VERSION)

# libtool versioning
LT_RELEASE=$MGi_MAJOR_VERSION.$MGi_MINOR_VERSION
LT_CURRENT=`expr $MGi_MICRO_VERSION - $MGi_INTERFACE_AGE`
LT_REVISION=$MGi_INTERFACE_AGE
LT_AGE=`expr $MGi_BINARY_AGE - $MGi_INTERFACE_AGE`

AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

dnl ========================================================================
dnl Checks for system type.
AC_CANONICAL_SYSTEM

dnl ========================================================================
dnl Check for tools
AC_PROG_CC
AC_PROG_LD
AC_PROG_RANLIB
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl ========================================================================
dnl Put the name and version of the package here
AM_INIT_AUTOMAKE

dnl AC_DEFINE_UNQUOTED(MGI_MAJOR_VERSION, $MGi_MAJOR_VERSION, [MGi major version])
dnl AC_DEFINE_UNQUOTED(MGI_MINOR_VERSION, $MGi_MINOR_VERSION, [MGi minor version])
dnl AC_DEFINE_UNQUOTED(MGI_MICRO_VERSION, $MGi_MICRO_VERSION, [MGi micro version])
dnl AC_DEFINE_UNQUOTED(MGI_INTERFACE_AGE, $MGi_INTERFACE_AGE, [MGi interface age])
dnl AC_DEFINE_UNQUOTED(MGI_BINARY_AGE, $MGi_BINARY_AGE, [MGi binary age])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h limits.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

dnl ========================================================================
dnl User selectable options
dnl inc_softkey="yes"
dnl inc_handwriting="yes"
dnl inc_phone="yes"

dnl AC_ARG_ENABLE(softkey,
dnl [  --enable-softkey     include softkey implementation <default=yes>],
dnl inc_softkey=$enableval)

dnl if test "x$inc_softkey" = "xyes"; then
dnl   AC_DEFINE(_MGI_INC_SOFTKEY, 1, 
dnl             [Define if include softkey implementation]) 
dnl fi

dnl AC_ARG_ENABLE(handwriting,
dnl [  --enable-handwriting     include handwriting implementation <default=yes>],
dnl inc_handwriting=$enableval)

dnl if test "x$inc_handwriting" = "xyes"; then
dnl   AC_DEFINE(_MGI_INC_HANDWRITING, 1, 
dnl             [Define if include handwriting implementation]) 
dnl fi

dnl AC_ARG_ENABLE(phone,
dnl [  --enable-phone     include phone implementation <default=yes>],
dnl inc_phone=$enableval)

dnl if test "x$inc_phone" = "xyes"; then
dnl   AC_DEFINE(_MGI_INC_PHONE, 1, 
dnl             [Define if include phone implementation]) 
dnl fi

build_minigui_v3="no"
have_libminigui="no"
threads_version="no"
procs_version="no"
standalone_version="no"
lib_pthread="yes"
build_tooltip="yes"
build_animate="yes"
mgdesktop_version="no"

dnl ========================================================================
dnl check for libminigui
AC_CHECK_HEADERS(minigui/common.h, have_libminigui=yes, foo=bar)

dnl ========================================================================
dnl check for run-time mode of MiniGUI
dnl ========================================================================
AC_CHECK_TYPE([struct _WINDOW_ELEMENT_RENDERER], build_minigui_v3="yes", foo=bar, [#include "include/mgi.h"])

AC_CHECK_DECLS(_MGRM_THREADS, threads_version="yes", foo=bar, [#include <minigui/common.h>])

AC_CHECK_DECLS(_MGRM_PROCESSES, procs_version="yes", foo=bar, [#include <minigui/common.h>])

AC_CHECK_DECLS(_MGRM_STANDALONE, standalone_version="yes", foo=bar, [#include <minigui/common.h>])

if test "x$build_minigui_v3" == "xyes" ; then
    if test "x$threads_version" = "xyes"; then
        CFLAGS="$CFLAGS -D_REENTRANT"
        APP_LIBS="$APP_LIBS -lminigui_ths -lpthread"
    else
        if test "x$standalone_version" = "xyes"; then
            APP_LIBS="$APP_LIBS -lminigui_sa"
        else
            APP_LIBS="$APP_LIBS -lminigui_procs -lpthread"
        fi
    fi
else
    APP_LIBS="$APP_LIBS -lminigui"
    if test "x$threads_version" = "xyes"; then
        CFLAGS="$CFLAGS -D_REENTRANT"
        APP_LIBS="$APP_LIBS -lpthread"
    else
        AC_CHECK_DECLS(_MGGAL_SHADOW, APP_LIBS="$APP_LIBS -lpthread", lib_pthread="no", [#include <minigui/common.h>])
        if test "x$lib_pthread" = "xno"; then
            AC_CHECK_DECLS(_MGGAL_MLSHADOW, APP_LIBS="$APP_LIBS -lpthread", no_pthread="no", [#include <minigui/common.h>])
        fi
    fi

fi

dnl ========================================================================
dnl check for softkeyboard 
dnl ========================================================================
AC_ARG_ENABLE([tooltip],
[  --enable-tooltip,    enable tooltip for softkeyboard <default=yes>],
build_tooltip=$enableval)

AC_ARG_ENABLE([animate],
[  --enable-animate,    enable animate for softkeyboard <default=yes>],
build_animate=$enableval)

AC_ARG_ENABLE([mgdesktop-version],
[  --enable-mgdesktop-version,    enable mgdesktop for softkeypad <default=no>],
mgdesktop_version=$enableval)


if test "x$build_animate" = "xyes" ; then
    if test "x$build_minigui_v3" == "xyes" ; then
        AC_DEFINE(KBD_ANIMATE, 1, [Define if ANIMATE is supported.])
    fi
fi

if test "x$build_tooltip" = "xyes" ; then
    if test "x$build_minigui_v3" == "xyes" ; then
        AC_DEFINE(KBD_TOOLTIP, 1, [Define if TOOLTIP is supported.])
    fi
fi

if test "x$mgdesktop_version" = "xyes" ; then
    if test "x$build_minigui_v3" == "xyes" ; then
        AC_DEFINE(MGDESKTOP_VERSION, 1, [Define if MGDESKTOP_VERSION is supported.])
    fi
fi

AM_CONFIG_HEADER(mgiconfig.h)

AC_ARG_WITH(skbsize,
        [  --with-skbsize=[480x272/320x240/240x320]])
case "$with_skbsize" in
  480x272)
    AC_DEFINE(SOFTKBD_480_272, 1, [Define if SOFTKBD_480_272 is supported.])
    ;;
  240x320)
    AC_DEFINE(SOFTKBD_240_320, 1, [Define if SOFTKBD_240_320 is supported.])
    ;;
  320x240)
    AC_DEFINE(SOFTKBD_320_240, 1, [Define if SOFTKBD_320_240 is supported.])
    ;;
  *)
    with_skbsize=480x272
    AC_DEFINE(SOFTKBD_480_272, 1, [Define if SOFTKBD_480_272 is supported.])
    ;;
esac


CFLAGS="$CFLAGS -D__MGI_LIB__"

AC_CHECK_DECLS(_HAVE_MATH_LIB, APP_LIBS="$APP_LIBS -lm", foo=bar, [#include <minigui/common.h>])
AC_CHECK_DECLS(_MGIMAGE_PNG, APP_LIBS="$APP_LIBS -lpng -lz", foo=bar, [#include <minigui/common.h>])
AC_CHECK_DECLS(_MGIMAGE_JPG, APP_LIBS="$APP_LIBS -ljpeg", foo=bar, [#include <minigui/common.h>])
AC_CHECK_DECLS(_MGFONT_TTF, APP_LIBS="$APP_LIBS -lttf", foo=bar, [#include <minigui/common.h>])
AC_CHECK_DECLS(_MGFONT_FT2, APP_LIBS="$APP_LIBS -lfreetype", foo=bar, [#include <minigui/common.h>])

AM_CONDITIONAL(MGRM_THREADS, test "x$threads_version" = "xyes")
AM_CONDITIONAL(MGRM_PROCESSES, test "x$procs_version" = "xyes")
AM_CONDITIONAL(MGRM_STANDALONE, test "x$standalone_version" = "xyes")

AC_SUBST(APP_LIBS)

AC_OUTPUT(
Makefile
m4/Makefile
include/Makefile
examples/Makefile
src/Makefile
src/softkeyboard/Makefile
src/softkeyboard/softkeyboard/Makefile
src/softkeyboard/softkeyboard/240-320/Makefile
src/softkeyboard/softkeyboard/320-240/Makefile
src/softkeyboard/softkeyboard/480-272/Makefile
)

if test "x$have_libminigui" != "xyes"; then
    AC_MSG_WARN([
        MiniGUI is not properly installed on the system. You need
        MiniGUI Ver 3.0.1 or later for building this package.
        Please configure and install MiniGUI Ver 3.0.1 first.
    ])
fi

