bin_PROGRAMS =		\
	feed-test	\
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


MAINTAINERCLEANFILES =	\
	*.in \
	*~

DISTCLEANFILES = $(MAINTAINERCLEANFILES)
