#!/bin/bash

function generate_jobs()
{
	cd "$1/infrabox"
	i=0
	ls -1 | sort | while read -r n; do
		if test "$n" == "@"; then continue; fi
		if test -e "$n/disabled"; then continue; fi
		if (($i != 0)); then echo ","; fi
		i=$(($i+1))

		cat << EOF
	{
		"type":"docker",
		"resources":{"limits":{"cpu":1,"memory":2048}},
		"docker_file":"infrabox/$n/Dockerfile.compile",
		"name":"$n-compile",
		"build_only":false,
		"build_context":"../.."
	}
EOF
	done
}

(
echo '{
	"jobs" : ['
generate_jobs "$1"
echo '	],
	"version":1
}'
) >"/infrabox/output/infrabox.json"

cat < "/infrabox/output/infrabox.json"

# ,
# 	{
# 		"type":"docker",
# 		"resources":{"limits":{"cpu":1,"memory":2048}},
# 		"docker_file":"infrabox/$n/Dockerfile.tests",
# 		"name":"$n-tests",
# 		"build_only":false,
# 		"build_context":"../..",
# 		"environment":{"COMPILE_CONTAINER":"$n-compile"},
# 		"depends_on": ["$n-compile"]
# 	}
