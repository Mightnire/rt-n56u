SRC_NAME="libiconv-1.16"

all: config_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME) AUTOMAKE=: ACLOCAL=: AUTOCONF=: AUTOHEADER=:

config_test:
	( if [ -f ./config_done ]; then \
		echo "the same configuration"; \
	else \
		make configure && touch config_done; \
	fi )

configure:
	( cd $(SRC_NAME) ; \
	AUTOMAKE=true ACLOCAL=true AUTOCONF=true AUTOHEADER=true \
	./configure \
		--prefix=$(STAGEDIR) \
		--enable-shared \
		--disable-static \
		--disable-debug-mode \
		--enable-relocatable \
		--disable-nls \
		--host=$(HOST_TARGET) \
		--build=$(HOST_BUILD); \
	)

clean:
	if [ -f $(SRC_NAME)/Makefile ] ; then \
		$(MAKE) -C $(SRC_NAME) clean ; \
	fi ; \
	rm -f config_done

install:
	$(MAKE) -C $(SRC_NAME) install DESTDIR=""

romfs:
	cp -fP $(DESTDIR)/lib/libiconv.so* $(ROMFSDIR)/lib
	chmod +x $(ROMFSDIR)/lib/libiconv.so*
