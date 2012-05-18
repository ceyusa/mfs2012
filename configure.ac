AC_PREREQ([2.62])

AC_INIT([gtrakt], [0.1.0], [mfs-dm-2011@igalia.com],
	[gtrakt], [https://github.com/ceyusa/mfs2012])

AC_CONFIG_SRCDIR([backend/feed-service.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wall foreign])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AC_HEADER_STDC

PKG_CHECK_MODULES([DEPS],  [glib-2.0
			    gio-2.0
			    gobject-2.0
			    libsoup-2.4
			    json-glib-1.0])

AC_CONFIG_FILES([
	Makefile
	backend/Makefile
])

AC_OUTPUT

AC_OUTPUT(backend/org.mfs.Gtrakt.FeedServer.service)
