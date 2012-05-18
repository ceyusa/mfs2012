bin_PROGRAMS =		\
	feed-test

libexec_PROGRAMS =	\
	feed-service

feed_test_SOURCES =	\
	feed-test.c	\
	gt-feed.c	\
	gt-feed.h

feed_service_SOURCES =		\
	feed-service.c		\
	gt-feed-server.c	\
	gt-feed-server.h	\
	gt-feed.c		\
	gt-feed.h

feed_test_CFLAGS = \
	$(DEPS_CFLAGS)

feed_test_LDADD = \
	$(DEPS_LIBS)

feed_service_CFLAGS = \
	$(DEPS_CFLAGS)

feed_service_LDADD = \
	$(DEPS_LIBS)

dbusdir = $(DBUS_SERVICES_DIR)
dbus_in_files = org.mfs.Gtrakt.FeedServer.service.in
dbus_DATA = org.mfs.Gtrakt.FeedServer.service

%.service: %.service.in
	@sed -e "s|\@libexecdir\@|$(libexecdir)|" $< > $@

CLEANFILES = $(dbus_DATA)
EXTRA_DIST = $(dbus_in_files)

MAINTAINERCLEANFILES =	\
	*.in \
	*~

DISTCLEANFILES = $(MAINTAINERCLEANFILES)
