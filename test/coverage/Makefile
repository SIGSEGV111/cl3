.PHONY: all html-report clean

all: html-report

clean:
	rm -Rf libcl3-core libcl3-mm libcl3-db libcl3-core.info libcl3-mm.info libcl3-db.info

libcl3-core.info: ../core/valgrind.log
	lcov --no-external --base-directory ../../core --directory ../../core --capture --output-file libcl3-core.info >/dev/null

libcl3-mm.info: ../mm/valgrind.log
	lcov --no-external --base-directory ../../mm --directory ../../mm --capture --output-file libcl3-mm.info >/dev/null

libcl3-db.info: ../db/valgrind.log
	lcov --no-external --base-directory ../../db --directory ../../db --capture --output-file libcl3-db.info >/dev/null

libcl3-core/index.html: libcl3-core.info
	genhtml --output-directory libcl3-core libcl3-core.info 2>&1 | grep -A2 -F "Overall coverage rate:"
	echo "*** Report for libcl3-core.so: $(CURDIR)/libcl3-core/index.html"

libcl3-mm/index.html: libcl3-mm.info
	genhtml --output-directory libcl3-mm libcl3-mm.info 2>&1 | grep -A2 -F "Overall coverage rate:"
	echo "*** Report for libcl3-mm.so: $(CURDIR)/libcl3-mm/index.html"

libcl3-db/index.html: libcl3-db.info
	genhtml --output-directory libcl3-db libcl3-db.info 2>&1 | grep -A2 -F "Overall coverage rate:"
	echo "*** Report for libcl3-db.so: $(CURDIR)/libcl3-db/index.html"

html-report: libcl3-core/index.html libcl3-mm/index.html libcl3-db/index.html
