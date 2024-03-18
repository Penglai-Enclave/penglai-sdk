all:
	make -C runtime
	make -C lib

install:
	mkdir -p /opt/penglai/lib
	mkdir -p /opt/penglai/runtime
	find lib -name "*.a" -exec cp --parent {} /opt/penglai/ \;
	find lib -name "*.h" -exec cp --parent {} /opt/penglai/ \;
	cp -r penglai_sdk_ssl /opt/penglai/
	cp -r runtime/lib /opt/penglai/runtime
	cp app.* /opt/penglai


clean:
	make -C runtime clean
	make -C lib clean
	make -C demo clean
	rm -f /opt/penglai/runtime/lib/*
	rm -f /opt/penglai/lib/
