SRC_NAME = lrzsz-0.12.20
SRC_URL = https://ohse.de/uwe/releases/lrzsz-0.12.20.tar.gz
THISDIR = $(shell pwd)

all: download_test extract_test config_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME) ACLOCAL=: AUTOCONF=: AUTOMAKE=: AUTOHEADER=:

download_test:
	( if [ ! -f $(THISDIR)/$(SRC_NAME).tar.gz ]; then \
		wget -t5 --timeout=20 --no-check-certificate -O $(SRC_NAME).tar.gz $(SRC_URL); \
	fi )

extract_test:
	( if [ ! -d $(THISDIR)/$(SRC_NAME) ]; then \
		tar zxf $(SRC_NAME).tar.gz; \
	fi ; \
	find $(SRC_NAME) -name "configure.in" -o -name "configure.ac" -o -name "Makefile.am" -o -name "acinclude.m4" -o -name "acconfig.h" -o -name "*.m4" ! -name "aclocal.m4" | xargs -r touch -d "-10 minutes" 2>/dev/null || true ; \
	find $(SRC_NAME) -name "aclocal.m4" | xargs -r touch -d "-5 minutes" 2>/dev/null || true ; \
	find $(SRC_NAME) -name "configure" | xargs -r touch -d "-3 minutes" 2>/dev/null || true ; \
	find $(SRC_NAME) -name "config.h.in" -o -name "stamp-h.in" | xargs -r touch -d "-2 minutes" 2>/dev/null || true ; \
	find $(SRC_NAME) -name "Makefile.in" | xargs -r touch -d "-1 minute" 2>/dev/null || true ; \
	)

config_test:
	( if [ -f ./config_done ]; then \
		echo "the same configuration"; \
	else \
		make configure && touch config_done; \
	fi )

configure:
	( cd $(SRC_NAME) ; \
	ACLOCAL=true AUTOCONF=true AUTOMAKE=true AUTOHEADER=true \
	./configure \
		--prefix=/usr \
		--disable-nls \
		--host=$(HOST_TARGET) \
		--build=$(HOST_BUILD) ; \
	)

clean:
	if [ -f $(SRC_NAME)/Makefile ] ; then \
		$(MAKE) -C $(SRC_NAME) distclean ; \
	fi ; \
	rm -f config_done

romfs:
	$(ROMFSINST) -p +x $(THISDIR)/$(SRC_NAME)/src/lrz /usr/bin/lrz
	$(ROMFSINST) -p +x $(THISDIR)/$(SRC_NAME)/src/lsz /usr/bin/lsz
