DESTDIR ?= /opt/penglai

all:
	make -C runtime
	make -C lib

install:
	mkdir -p $(DESTDIR)/lib
	mkdir -p $(DESTDIR)/runtime
	find lib -name "*.a" -exec cp --parent {} $(DESTDIR)/ \;
	find lib -name "*.h" -exec cp --parent {} $(DESTDIR)/ \;
	cp -r penglai_sdk_ssl $(DESTDIR)/
	cp -r runtime/lib $(DESTDIR)/runtime
	cp app.* $(DESTDIR)

clean:
	make -C runtime clean
	make -C lib clean
	make -C demo clean
	rm -f $(DESTDIR)/runtime/lib/*
	rm -f $(DESTDIR)/lib/*
