ifeq (,$(wildcard ../config.mak))
$(error "../config.mak is not present, run configure !")
endif
include ../config.mak

PROG = ushare
MANS = ushare.1

EXTRADIST = \
	presentation.h \
	metadata.h \
	mime.h \
	buffer.h \
	util_iconv.h \
	content.h \
	cfgparser.h \
	trace.h \
	osdep.h \
	ctrl_telnet.h \
	ushare.h \
	gettext.h \
	minmax.h \
	ufam.h \


SRCS = \
	http.c \
	presentation.c \
	metadata.c \
	mime.c \
	buffer.c \
	util_iconv.c \
	content.c \
	cfgparser.c \
	trace.c \
	osdep.c \
	ctrl_telnet.c \
	ufam.c \
	ushare.c

OBJS = $(SRCS:.c=.o)

.SUFFIXES: .c .o

all: depend $(PROG)

.c.o:
	$(CC) -c $(CFLAGS) $(OPTFLAGS) -o $@ $<

$(PROG): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(EXTRALIBS) -o $@

clean:
	-$(RM) -f *.o $(PROG)
	-$(RM) -f .depend

distclean:

install: $(PROG) install-man
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) $(PROG) $(DESTDIR)$(bindir)
	$(STRIP) $(INSTALLSTRIP) $(DESTDIR)$(bindir)/$(PROG)

install-man: $(MANS)
	for m in $(MANS); do \
		section=`echo $$m | sed -e 's/^.*\\.//'`; \
		$(INSTALL) -d $(DESTDIR)$(mandir)/man$$section; \
		$(INSTALL) $$m $(DESTDIR)$(mandir)/man$$section; \
	done

depend:
	$(CC) -I.. -MM $(CFLAGS) $(SRCS) 1>.depend

.PHONY: clean distclean install depend install-man

dist-all:
	cp $(EXTRADIST) $(SRCS) Makefile $(DIST) $(MANS)

.PHONY: dist-all

#
# include dependency files if they exist
#
ifneq ($(wildcard .depend),)
include .depend
endif
