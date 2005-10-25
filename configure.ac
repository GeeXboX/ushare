# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(GeeXboX uShare, 0.9.0, ushare@geexbox.org)
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER([config.h])

AC_PROG_CC
AC_PROG_MAKE_SET

AC_CHECK_HEADER([upnp/upnp.h], [], [exit])
AC_CHECK_HEADER([upnp/upnptools.h], [], [exit])

AC_SEARCH_LIBS(ixmlCloneDOMString, ixml, [], [exit])
AC_SEARCH_LIBS(UpnpInit, upnp, [], [exit])

if test "x$GCC" = "xyes"; then
  CFLAGS="$CFLAGS -Wall -W"
fi

AC_CONFIG_FILES([
        Makefile
        src/Makefile
        ])
AC_OUTPUT
