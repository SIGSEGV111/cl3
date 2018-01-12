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
		"resources":{"limits":{"cpu":4,"memory":4096}},
		"docker_file":"infrabox/$n/Dockerfile",
		"name":"$n",
		"build_only":false
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
