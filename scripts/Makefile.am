EXTRA_DIST := ushare ushare.conf

init_scripts := ushare

sysconf_DATA := ushare.conf

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(sysconfdir)/init.d
	@for s in $(init_scripts); do \
		echo " $(INSTALL_SCRIPT) -m 755 $(srcdir)/$$s $(DESTDIR)$(sysconfdir)/init.d/$$s"; \
		$(INSTALL_SCRIPT) -m 755 "$(srcdir)/$$s" "$(DESTDIR)$(sysconfdir)/init.d/$$s"; \
	done
