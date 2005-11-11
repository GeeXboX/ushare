# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(GeeXboX uShare, 0.9.1, ushare@geexbox.org)
AM_INIT_AUTOMAKE([gnits std-options check-news dist-bzip2 no-dist-gzip])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AC_PROG_MAKE_SET

AM_GNU_GETTEXT
AM_GNU_GETTEXT_VERSION(0.14.5)
ALL_LINGUAS="fr"

LOCALEDIR="${datadir}/locale"
eval LOCALEPATH="$LOCALEDIR"
AC_DEFINE_UNQUOTED(LOCALEDIR, "$LOCALEPATH",[Path where catalog files will be.])
AC_SUBST(LOCALEDIR)

AM_ICONV
AM_LANGINFO_CODESET

AC_CHECK_HEADER([upnp/upnp.h], [], [exit])
AC_CHECK_HEADER([upnp/upnptools.h], [], [exit])

AC_SEARCH_LIBS(ixmlCloneDOMString, ixml, [], [exit])
AC_SEARCH_LIBS(UpnpInit, upnp, [], [exit])

if test "x$GCC" = "xyes"; then
  CFLAGS="$CFLAGS -Wall -W"
fi

AC_CONFIG_FILES([
        Makefile
        debian/Makefile
        src/Makefile
        intl/Makefile
        po/Makefile.in
        ])
AC_OUTPUT
