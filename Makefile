ifeq (,$(wildcard config.mak))
$(error "config.mak is not present, run configure !")
endif
include config.mak

DISTFILE = ushare-$(VERSION).tar.bz2

EXTRADIST = AUTHORS \
	    ChangeLog \
	    configure \
	    COPYING \
	    NEWS \
	    README \
	    THANKS \
	    TODO \

SUBDIRS = po \
	  scripts \
	  src \

all:
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean

distclean: clean
	-$(RM) -f config.log
	-$(RM) -f config.mak
	-$(RM) -f config.h

install:
	$(MAKE) -C scripts install
	$(MAKE) -C src install

.PHONY: clean distclean install

dist:
	-$(RM) $(DISTFILE)
	dist=$(shell pwd)/ushare-$(VERSION) && \
	for subdir in . $(SUBDIRS); do \
		mkdir -p "$$dist/$$subdir"; \
		$(MAKE) -C $$subdir dist-all DIST="$$dist/$$subdir"; \
	done && \
	tar cjf $(DISTFILE) ushare-$(VERSION)
	-$(RM) -rf ushare-$(VERSION)

dist-all:
	cp $(EXTRADIST) $(SRCS) Makefile $(DIST)

.PHONY: dist dist-all
