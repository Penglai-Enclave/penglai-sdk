DESTDIR ?= /opt/penglai

all:
	make -C runtime
	make -C lib
	make -C penglai-host

install:
	mkdir -p $(DESTDIR)/lib
	mkdir -p $(DESTDIR)/runtime
	mkdir -p $(DESTDIR)/bin
	find lib -name "*.a" -exec cp --parent {} $(DESTDIR)/ \;
	find lib -name "*.h" -exec cp --parent {} $(DESTDIR)/ \;
	cp -r penglai_sdk_ssl $(DESTDIR)/
	cp -r runtime/lib $(DESTDIR)/runtime
	cp -r penglai-host/penglai-host $(DESTDIR)/bin
	chmod +x $(DESTDIR)/bin/penglai-host
	cp app.* $(DESTDIR)

clean:
	make -C runtime clean
	make -C lib clean
	make -C penglai-host clean
	rm -rf $(DESTDIR)/runtime/lib/*
	rm -rf $(DESTDIR)/lib/*
