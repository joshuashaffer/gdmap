dnl Process this file with autoconf to produce a configure script.

AC_INIT([GdMap], [0.8.1], [http://sourceforge.net/tracker/?atid=373401&group_id=21965])
AC_CONFIG_SRCDIR([config.h.in])
AM_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE

dnl AM_PROG_LIBTOOL

AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
dnl AC_PROG_LIBTOOL

AM_PROG_CC_C_O
AM_MAINTAINER_MODE

AC_HEADER_STDC
AC_CHECK_HEADERS(string.h sys/time.h stdlib.h)

AC_C_CONST
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_BIGENDIAN

pkg_modules="libxml-2.0 >= 2.0.0"
PKG_CHECK_MODULES(LIBXML, [$pkg_modules])
AC_SUBST(LIBXML_CFLAGS)
AC_SUBST(LIBXML_LIBS)

ALL_LINGUAS="bg da de es fr he hr it nb no ru sv tr"

AM_GLIB_GNU_GETTEXT
AC_PROG_INTLTOOL

GETTEXT_PACKAGE=GdMap
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Name of the package])

# Make sure we use 64-bit versions of various file stuff.
AC_SYS_LARGEFILE

dnl Use -Wall if we have gcc.
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  [*[\ \	]-Wall[\ \	]*]) ;;
  *) CFLAGS="$CFLAGS -W -Wall" ;;
  esac
  CFLAGS="$CFLAGS -D_GNU_SOURCE"
fi

_cppflags=${CPPFLAGS}
_ldflags=${LDFLAGS}

AC_MSG_CHECKING([compiler flag -Wno-pointer-sign])
_cflags="${CFLAGS}"
CFLAGS="$CFLAGS -Wno-pointer-sign"
AC_TRY_COMPILE( , [
    int main(void) { return 0; }
  ],[
    AC_MSG_RESULT([yes])
  ],[
    AC_MSG_RESULT([no])
    CFLAGS="$_cflags"
  ]
)

PKG_CHECK_MODULES(UI, gtk+-2.0 >= 2.6.0)
UI_CFLAGS="$UI_CFLAGS -DGTK_DISABLE_DEPRECATED"

AC_SUBST(UI_CFLAGS)
AC_SUBST(UI_LIBS)

dnl packagesrcdir=`cd $srcdir && pwd`
dnl AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}", [Location of GdMap's sources])

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  po/Makefile.in
  data/Makefile
])

AC_OUTPUT
