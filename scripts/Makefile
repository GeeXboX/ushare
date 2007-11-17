ifeq (,$(wildcard ../config.mak))
$(error "config.mak is not present, run configure !")
endif
include ../config.mak

CONF_FILE  = "ushare.conf"
INITD_FILE = "ushare"

install:
	$(INSTALL) -d $(sysconfdir)
	$(INSTALL) $(CONF_FILE) $(sysconfdir)
	$(INSTALL) -d $(sysconfdir)/init.d
	$(INSTALL) $(INITD_FILE) $(sysconfdir)/init.d

.PHONY: install
