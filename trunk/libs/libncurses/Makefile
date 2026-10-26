SRC_NAME=ncurses-6.2
SRC_URL=https://github.com/mirror/ncurses/archive/v6.2.zip
THISDIR = $(shell pwd)
STAGEDIR ?= $(shell cd $(THISDIR)/../../stage && pwd)

all: download_test extract_test config_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME)

download_test:
	( if [ ! -f $(SRC_NAME).zip ]; then \
		wget -t5 --timeout=20 --no-check-certificate -O $(SRC_NAME).zip $(SRC_URL); \
	fi )

extract_test:
	( if [ ! -d $(SRC_NAME) ]; then \
		unzip -qq $(SRC_NAME).zip ; \
		patch -p1 -d $(SRC_NAME) < $(SRC_NAME).patch ; \
	fi )

config_test:
	( if [ -f ./config_done ]; then \
		echo "the same configuration"; \
	else \
		make configure && touch config_done; \
	fi )

configure:
	( cd $(SRC_NAME) ; \
	./configure \
		--prefix=$(STAGEDIR) \
		--with-install-prefix=$(STAGEDIR) \
		--without-ada \
		--without-termlib \
		--without-progs \
		--without-tests \
		--without-manpages \
		--disable-db-install \
		--enable-widec \
		--with-shared \
		--without-debug \
		--with-normal \
		--without-profile \
		--enable-overwrite \
		--enable-pc-files \
		--with-pkg-config-libdir="$(STAGEDIR)/lib/pkgconfig/" \
		--host=$(HOST_TARGET) \
		--build=$(HOST_BUILD) ; \
	)

clean:
	if [ -f $(SRC_NAME)/Makefile ] ; then \
		$(MAKE) -C $(SRC_NAME) distclean ; \
	fi ; \
	rm -f config_done

install:
	$(MAKE) -C $(SRC_NAME) install.libs install.includes DESTDIR=""
	mkdir -p $(STAGEDIR)/lib/pkgconfig $(STAGEDIR)/bin
	if [ -f $(SRC_NAME)/misc/ncursesw.pc ]; then \
		cp -f $(SRC_NAME)/misc/ncursesw.pc $(STAGEDIR)/lib/pkgconfig/ ; \
	fi
	if [ -f $(SRC_NAME)/misc/ncurses-config ]; then \
		cp -f $(SRC_NAME)/misc/ncurses-config $(STAGEDIR)/bin/ncursesw6-config ; \
		chmod 755 $(STAGEDIR)/bin/ncursesw6-config ; \
	fi

romfs:
	cp -fP $(DESTDIR)/lib/libncursesw.so* $(ROMFSDIR)/lib
