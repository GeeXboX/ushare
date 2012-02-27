ifeq (,$(wildcard ../config.mak))
$(error "../config.mak is not present, run configure !")
endif
include ../config.mak

ifeq ($(DEVELOPER),yes)
  ifeq ($(HAVE_ETAGS),yes)
    BUILD_RULES   += TAGS
  endif
  ifeq ($(HAVE_CTAGS),yes)
    BUILD_RULES   += tags
  endif
endif

PROG = ushare

EXTRADIST = ushare.1 \
	cds.h \
	cms.h \
	msr.h \
	http.h \
	presentation.h \
	metadata.h \
	mime.h \
	services.h \
	buffer.h \
	util_iconv.h \
	content.h \
	cfgparser.h \
	trace.h \
	redblack.h \
	osdep.h \
	ctrl_telnet.h \
	ushare.h \
	gettext.h \
	minmax.h \
	ufam.h \


SRCS = \
	cds.c \
	cms.c \
	msr.c \
	http.c \
	presentation.c \
	metadata.c \
	mime.c \
	services.c \
	buffer.c \
	util_iconv.c \
	content.c \
	cfgparser.c \
	trace.c \
	redblack.c \
	osdep.c \
	ctrl_telnet.c \
	ufam.c \
	ushare.c

OBJS = $(SRCS:.c=.o)

.SUFFIXES: .c .o

all: depend $(BUILD_RULES) $(PROG)

.c.o:
	$(CC) -c $(CFLAGS) $(OPTFLAGS) -o $@ $<

$(PROG): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(EXTRALIBS) -o $@

TAGS:
	@rm -f $@; \
	( find -name '*.[chS]' -print ) | xargs etags -a

tags:
	@rm -f $@; \
	( find -name '*.[chS]' -print ) | xargs ctags -a;

clean:
	-$(RM) -f *.o $(PROG)
	-$(RM) -f .depend

distclean:

install: $(PROG)
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) $(PROG) $(DESTDIR)$(bindir)
	$(STRIP) $(INSTALLSTRIP) $(DESTDIR)$(bindir)/$(PROG)

depend:
	$(CC) -I.. -MM $(CFLAGS) $(SRCS) 1>.depend

.PHONY: clean distclean install depend

dist-all:
	cp $(EXTRADIST) $(SRCS) Makefile $(DIST)

.PHONY: dist-all

#
# include dependency files if they exist
#
ifneq ($(wildcard .depend),)
include .depend
endif
