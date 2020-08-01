set -e
set +u
set -o pipefail
readonly CL3_CXX="${CL3_CXX:-$(which g++)}"
readonly CL3_CC="${CL3_CC:-$(which gcc)}"
readonly CL3_BUILDTYPE="${CL3_BUILDTYPE:-user}"
readonly CL3_AMALGAM="${CL3_AMALGAM:-yes}"
readonly CL3_STRIP="${CL3_STRIP:-no}"
set -u

readonly CL3_SCRIPT="$(readlink -f "$0")"
readonly CL3_SCRIPTDIR="$(dirname "$CL3_SCRIPT")"
readonly CL3_GIT_ROOT="$(cd "$CL3_SCRIPTDIR" && git rev-parse --show-toplevel)"
readonly CL3_PWD="$(readlink -f .)"
readonly CL3_GENDIR="$CL3_PWD"
readonly CL3_PJOBS=$(grep "^processor[[:space:]]]*:[[:space:]]*[0-9]\+$" /proc/cpuinfo | wc -l)

renice 20 $$ >/dev/null
ionice -c idle -p $$

function die()
{
	echo "ERROR: $1" 1>&2
	exit 1
}

function IsVarSet()
{
	for varname in "$@"; do
		if ! ( [ -v "$varname" ] && [ -n "${!varname}" ] ); then
			return 1
		fi
	done
	return 0
}

function InTree()
{
	local p="$1"
	if test "${p:0:${#CL3_GIT_ROOT}}" == "${CL3_GIT_ROOT}"; then
		return 0
	else
		return 1
	fi
}

function MkArg()
{
	local key="$1"
	local varname="$2"

	if IsVarSet "$varname"; then
		echo "-$key=$varname"
	fi
}

function GitHasUncommitedChanges()
{
	if (($(cd "$CL3_GIT_ROOT" && git status -suall | wc -l) > 0)); then
		return 0
	else
		return 1
	fi
}

function GitGetVersionTag()
{
	(cd "$CL3_GIT_ROOT" && git describe --tags)
}

if InTree "$CL3_PWD"; then
	die "in-tree build is not supported"
fi

# -Wno-multichar
readonly CL3_CXX_BASE="-fvisibility-inlines-hidden -fvisibility=hidden -std=c++11 -fdata-sections -ffunction-sections -lpthread -I $CL3_GENDIR/include -L $CL3_GENDIR/lib"

readonly CL3_CXX_ARCH="$(MkArg mcpu CL3_ARCH)"
readonly CL3_CXX_LIB="-fPIC -shared"
readonly CL3_CXX_EXE="-fPIE"

readonly CL3_CXX_INFRA="-g -O1 -DCL3_DEBUG"
readonly CL3_CXX_DEBUG="-g -O0 -DCL3_DEBUG -rdynamic"
readonly CL3_CXX_PERF=" -g -O3 -DCL3_RELEASE -flto"
readonly CL3_CXX_QA_DEVEL="-Wno-unused-parameter -Wno-deprecated-declarations -Wno-unused-function -Wall -Wextra -Werror --coverage"
readonly CL3_CXX_QA_RELEASE="-Wall -Wextra -Werror"
readonly CL3_VERSION="$(GitGetVersionTag)"
readonly CL3_CXX_VERSION="-DCL3_VERSION=\"$CL3_VERSION\""

case "$CL3_BUILDTYPE" in
	devel)
		readonly CL3_CXX_BM="$CL3_CXX_DEBUG $CL3_CXX_QA_DEVEL"
		;;

	infra)
		if GitHasUncommitedChanges; then die "WTF??? There should not be any uncommited changes in an infrastructure build"; fi
		readonly CL3_CXX_BM="$CL3_CXX_INFRA $CL3_CXX_QA_DEVEL"
		;;

	user)
		readonly CL3_CXX_BM="$CL3_CXX_PERF"
		;;

	release)
		if GitHasUncommitedChanges; then die "cannot build release with uncommited local changes"; fi
		readonly CL3_CXX_BM="$CL3_CXX_DEBUG $CL3_CXX_QA_RELEASE"
		;;

	*)
		die "unknown CL3_BUILDTYPE"
		;;
esac

readonly CL3_CXX_OPTS="$CL3_CXX_BASE $CL3_CXX_ARCH $CL3_CXX_BM $CL3_CXX_VERSION"
