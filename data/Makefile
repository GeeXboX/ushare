ifeq (,$(wildcard ../config.mak))
$(error "config.mak is not present, run configure !")
endif
include ../config.mak

INSTALL_RULES = install-icon

ICON_SM_PNG   = "sm.png"
ICON_SM_JPEG  = "sm.jpeg"
ICON_LRG_PNG  = "lrg.png"
ICON_LRG_JPEG = "lrg.jpeg"

EXTRADIST = $(ICON_SM_PNG) $(ICON_SM_JPEG) $(ICON_LRG_PNG) $(ICON_LRG_JPEG)

all:

clean:

distclean:

install: $(INSTALL_RULES)

install-icon:
	$(INSTALL) -d $(DESTDIR)$(datadir)/ushare
	$(INSTALL) -m 644 $(ICON_SM_PNG)   $(DESTDIR)$(datadir)/ushare
	$(INSTALL) -m 644 $(ICON_SM_JPEG)  $(DESTDIR)$(datadir)/ushare
	$(INSTALL) -m 644 $(ICON_LRG_PNG)  $(DESTDIR)$(datadir)/ushare
	$(INSTALL) -m 644 $(ICON_LRG_JPEG) $(DESTDIR)$(datadir)/ushare

dist-all:
	cp $(EXTRADIST) $(SRCS) Makefile $(DIST)

.PHONY: clean disctlean install dist-all
