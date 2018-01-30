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
		"resources":{"limits":{"cpu":1,"memory":1024}},
		"docker_file":"infrabox/$n/Dockerfile",
		"name":"$n",
		"build_only":true,
		"build_context":"../..",
		"deployments":[
			{
				"type": "docker-registry",
				"host": "v2-registry.dhcp.wdf.sap.corp",
				"repository": "$n",
				"tag" : "$2"
			},
			{
				"type": "docker-registry",
				"host": "v2-registry.dhcp.wdf.sap.corp",
				"repository": "$n",
				"tag" : "latest"
			}
		]
	}
EOF
	done
}

ts="$(date '+%Y%m%d%H%M%S')"

(
echo '{
	"jobs" : ['
generate_jobs "$1" "$ts"
echo '	],
	"version":1
}'
) >"/infrabox/output/infrabox.json"

cat < "/infrabox/output/infrabox.json"
