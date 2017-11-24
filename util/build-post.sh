#!/bin/bash

if test -d "/infrabox/upload/testresult"; then
	i=0
	for f in tmp/*/src/*/test/test_detail.xml; do
		python util/infrabox-testresult/testresult.py -f xunit -i "$f" -o /infrabox/upload/testresult/testresult-$i.json
		i=$(($i+1))
	done
fi
