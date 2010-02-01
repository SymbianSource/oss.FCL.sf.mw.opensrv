# Move this back to GENHEADERS once dbus-binding-tool supports 'g' type
#    tp-chan-type-contact-search-gen.h

GENHEADERS = \
    tp-connmgr-gen.h \
    tp-conn-gen.h \
    tp-chan-gen.h \
    tp-conn-iface-avatars-gen.h \
    tp-chan-iface-dtmf-gen.h \
    tp-chan-iface-group-gen.h \
    tp-chan-iface-hold-gen.h \
    tp-chan-iface-password-gen.h \
    tp-chan-iface-transfer-gen.h \
    tp-chan-iface-ice-signalling-gen.h \
    tp-chan-type-contact-list-gen.h \
    tp-chan-type-room-list-gen.h \
    tp-chan-type-streamed-media-gen.h \
    tp-chan-type-text-gen.h \
    tp-conn-iface-aliasing-gen.h \
    tp-conn-iface-capabilities-gen.h \
    tp-conn-iface-contact-info-gen.h \
    tp-conn-iface-forwarding-gen.h \
    tp-conn-iface-presence-gen.h \
    tp-conn-iface-privacy-gen.h \
    tp-conn-iface-renaming-gen.h \
    tp-props-iface-gen.h \
    tp-ch-gen.h \
    tp-ice-session-handler-gen.h \
    tp-ice-stream-handler-gen.h \
    tp-ifaces-signals-marshal.h \
    tp-chan-signals-marshal.h \
    tp-conn-signals-marshal.h \
    tp-connmgr-signals-marshal.h

MARSHALLER_SOURCES = \
    tp-ifaces-signals-marshal.c \
    tp-chan-signals-marshal.c \
    tp-conn-signals-marshal.c \
    tp-connmgr-signals-marshal.c


BUILT_SOURCES = \
    $(GENHEADERS) \
    $(MARSHALLER_SOURCES)


# Correctly clean the generated headers, but keep the xml description
CLEANFILES = $(BUILT_SOURCES)
#Rule to generate the binding headers
%-gen.h: $(top_builddir)/xml/%.xml Makefile.am
	dbus-binding-tool --prefix=$* --mode=glib-client $< > $*-gen.h

# Create the necessary marshallers for the signals implemented by the
# library

%-marshal.h: %-marshal.list Makefile.am
	glib-genmarshal --header --prefix=$(subst -,_,$*)_marshal $< > $*-marshal.h

%-marshal.c: %-marshal.list Makefile.am
	glib-genmarshal --body --prefix=$(subst -,_,$*)_marshal $< > $*-marshal.c

lib_LTLIBRARIES = libtelepathy.la

libtelepathy_la_CFLAGS = $(DBUS_CFLAGS) $(ERROR_CFLAGS) $(GLIB_CFLAGS)
libtelepathy_la_LIBADD = $(DBUS_LIBS) $(GLIB_LIBS)
libtelepathy_la_LDFLAGS = \
    -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
    -export-symbols-regex '^tp_' \
    -export-dynamic

CORESOURCES = \
	tp-chan.c \
	tp-conn.c \
	tp-connmgr.c \
	tp-helpers.c \
	tp-props-iface.c

COREHEADERS = \
	tp-chan-ifaces-signal-setters.h \
	tp-conn-ifaces-signal-setters.h \
	tp-chan.h \
	tp-conn.h \
	tp-connmgr.h \
	tp-constants.h \
	tp-helpers.h \
	tp-interfaces.h \
	tp-props-iface.h

libtelepathy_la_SOURCES = \
	tp-chan-ifaces-signal-setters.c \
	tp-conn-ifaces-signal-setters.c \
	$(MARSHALLER_SOURCES) \
	$(CORESOURCES) \
	$(COREHEADERS)

telepathyincdir=$(includedir)/telepathy-1.0/libtelepathy

telepathyinc_HEADERS = \
	$(COREHEADERS) \
	$(GENHEADERS)
