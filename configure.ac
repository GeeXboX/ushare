# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(uShare, 0.9.9, ushare@geexbox.org)
AM_INIT_AUTOMAKE([gnits std-options check-news dist-bzip2 no-dist-gzip])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_INSTALL

# Checks for l10n availability.
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION(0.14.5)
ALL_LINGUAS="fr de"

LOCALEDIR="${localedir}"
while true; do
 eval t="$LOCALEDIR"
 test "$t" = "$LOCALEDIR" && break
 LOCALEDIR="$t"
done
AC_DEFINE_UNQUOTED(LOCALEDIR, "$LOCALEDIR",[Path where catalog files will be.])
AC_SUBST(LOCALEDIR)

SYSCONFDIR="${sysconfdir}"
while true; do
 eval t="$SYSCONFDIR"
 test "$t" = "$SYSCONFDIR" && break
 SYSCONFDIR="$t"
done
AC_DEFINE_UNQUOTED(SYSCONFDIR, "$SYSCONFDIR",[Path where system configuration files will be.])
AC_SUBST(SYSCONFDIR)

AM_ICONV
AM_LANGINFO_CODESET
AC_CHECK_FUNCS([setlocale])
AC_CHECK_HEADERS([langinfo.h libintl.h locale.h])

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_DIRENT
AC_HEADER_STDBOOL
AC_CHECK_HEADERS([limits.h fcntl.h sys/ioctl.h syslog.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIGNAL
AC_TYPE_SIZE_T

# Checks for library functions
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memset socket strcasecmp strchr strrchr strdup strndup strerror])

#############################
#     Check for libupnp     #
#############################
upnpdir=""
AC_ARG_WITH(libupnp-prefix,
  [  --with-libupnp-prefix=DIR  search for libupnp in DIR/include and DIR/lib],
  upnpdir="$withval"
)

if test "x$upnpdir" = "x"; then
  PKG_CHECK_MODULES(libupnp,
    libupnp >= 1.4.2,
    [
      CFLAGS="$libupnp_CFLAGS $CFLAGS"
      LDFLAGS="$libupnp_LIBS $LDFLAGS"
    ])
else
  CFLAGS="-I$upnpdir/include $CFLAGS"
  LDFLAGS="-L$upnpdir/lib $LDFLAGS"
fi

# Checks for upnp headers.
AC_CHECK_HEADERS([upnp/upnp.h upnp/upnptools.h], [], [
  AC_MSG_ERROR(*** You need libupnp to build this program ! ***)
])

# Checks for upnp libraries.
AC_SEARCH_LIBS(ixmlRelaxParser, ixml, [], [exit])
AC_SEARCH_LIBS(ThreadPoolAdd, threadutil, [], [exit])
AC_SEARCH_LIBS(UpnpSetMaxContentLength, upnp, [], [exit])



if test "x$GCC" = "xyes"; then
  CFLAGS="$CFLAGS -Wall -W -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_REENTRANT"
fi

AC_DEFINE(_GNU_SOURCE, [],[Use GNU])

AC_CONFIG_FILES([
        Makefile
        src/Makefile
        po/Makefile.in
        scripts/Makefile
        ])
AC_OUTPUT
