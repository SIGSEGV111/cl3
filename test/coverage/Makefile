.PHONY: all html-report

all: html-report

clean:
	rm -Rf libcl3-core libcl3-mm libcl3-core.info libcl3-mm.info

libcl3-core.info: ../core/valgrind.log
	lcov --no-external --base-directory ../../core --directory ../../core --capture --output-file libcl3-core.info >/dev/null

libcl3-mm.info: ../mm/valgrind.log
	lcov --no-external --base-directory ../../mm --directory ../../mm --capture --output-file libcl3-mm.info >/dev/null

libcl3-core/index.html: libcl3-core.info
	genhtml --output-directory libcl3-core libcl3-core.info 2>&1 | grep -A2 -F "Overall coverage rate:"
	echo "*** Report for libcl3-core.so: $(CURDIR)/libcl3-core/index.html"


libcl3-mm/index.html: libcl3-mm.info
	genhtml --output-directory libcl3-mm libcl3-mm.info 2>&1 | grep -A2 -F "Overall coverage rate:"
	echo "*** Report for libcl3-mm.so: $(CURDIR)/libcl3-mm/index.html"

html-report: libcl3-core/index.html libcl3-mm/index.html
