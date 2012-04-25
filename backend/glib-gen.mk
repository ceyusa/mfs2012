# Rules for generating enumeration types using glib-mkenums
#
# Define:
# 	glib_enum_h = header template file
# 	glib_enum_c = source template file
# 	glib_enum_headers = list of headers to parse
#
# before including Makefile.am.enums. You will also need to have
# the following targets already defined:
#
# 	CLEANFILES
#	DISTCLEANFILES
#	BUILT_SOURCES
#	EXTRA_DIST
#
# Author: Emmanuele Bassi <ebassi@linux.intel.com>

# Basic sanity checks
$(if $(GLIB_MKENUMS),,$(error Need to define GLIB_MKENUMS))

$(if $(or $(glib_enum_h), \
          $(glib_enum_c)),, \
    $(error Need to define glib_enum_h and glib_enum_c))

$(if $(glib_enum_headers),,$(error Need to define glib_enum_headers))

enum_tmpl_h=$(addprefix $(srcdir)/, $(glib_enum_h:.h=.h.in))
enum_tmpl_c=$(addprefix $(srcdir)/, $(glib_enum_c:.c=.c.in))

CLEANFILES += stamp-enum-types
DISTCLEANFILES += $(glib_enum_h) $(glib_enum_c)
BUILT_SOURCES += $(glib_enum_h) $(glib_enum_c)
EXTRA_DIST += $(enum_tmpl_h) $(enum_tmpl_c)

stamp-enum-types: $(glib_enum_headers) $(enum_tmpl_h)
	$(AM_V_GEN)$(GLIB_MKENUMS) \
		--template $(enum_tmpl_h) \
	$(glib_enum_headers) > xgen-eh \
	&& (cmp -s xgen-eh $(glib_enum_h) || cp -f xgen-eh $(glib_enum_h)) \
	&& rm -f xgen-eh \
	&& echo timestamp > $(@F)

$(glib_enum_h): stamp-enum-types
	@true

$(glib_enum_c): $(glib_enum_headers) $(glib_enum_h) $(enum_tmpl_c)
	$(AM_V_GEN)$(GLIB_MKENUMS) \
		--template $(enum_tmpl_c) \
	$(glib_enum_headers) > xgen-ec \
	&& cp -f xgen-ec $(glib_enum_c) \
	&& rm -f xgen-ec

# Rules for generating marshal files using glib-genmarshal
#
# Define:
# 	glib_marshal_list = marshal list file
# 	glib_marshal_prefix = prefix for marshal functions
#
# before including Makefile.am.marshal. You will also need to have
# the following targets already defined:
#
# 	CLEANFILES
#	DISTCLEANFILES
#	BUILT_SOURCES
#	EXTRA_DIST
#
# Author: Emmanuele Bassi <ebassi@linux.intel.com>

marshal_h = $(glib_marshal_list:.list=.h)
marshal_c = $(glib_marshal_list:.list=.c)

CLEANFILES += stamp-marshal
DISTCLEANFILES += $(marshal_h) $(marshal_c)
BUILT_SOURCES += $(marshal_h) $(marshal_c)
EXTRA_DIST += $(srcdir)/$(glib_marshal_list)

stamp-marshal: $(glib_marshal_list)
	$(AM_V_GEN)$(GLIB_GENMARSHAL) \
		--prefix=$(glib_marshal_prefix) \
		--header \
	$(srcdir)/$(glib_marshal_list) > xgen-mh \
	&& (cmp -s xgen-mh $(marshal_h) || cp -f xgen-mh $(marshal_h)) \
	&& rm -f xgen-mh \
	&& echo timestamp > $(@F)

$(marshal_h): stamp-marshal
	@true

$(marshal_c): $(marshal_h)
	$(AM_V_GEN)(echo "#include \"$(marshal_h)\"" ; \
	$(GLIB_GENMARSHAL) \
		--prefix=$(glib_marshal_prefix) \
		--body \
	$(srcdir)/$(glib_marshal_list)) > xgen-mc \
	&& cp xgen-mc $(marshal_c) \
	&& rm -f xgen-mc

