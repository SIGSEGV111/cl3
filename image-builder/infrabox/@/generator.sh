#!/bin/bash

function generate_jobs()
{
	cd "$1/infrabox"
	i=0
	ls -1 | sort | while read -r n; do
		if test "$n" == "@"; then continue; fi
		if (($i != 0)); then echo ","; fi
		i=$(($i+1))

		cat << EOF
	{
		"type":"docker",
		"resources":{"limits":{"cpu":1,"memory":8192}},
		"docker_file":"infrabox/$n/Dockerfile",
		"name":"$n",
		"build_only":true,
		"keep":true,
		"deployments":[{
			"type": "docker-registry",
            "host": "v2-kube.wdf.sap.corp:5000",
            "repository": "$n"
		}]
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
