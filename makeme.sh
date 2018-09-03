#!/bin/bash


#functions
me=${0##*/}
#usage
print_usage()
{
	echo "Usage: $me [build|buildsingle|clean|superclean]" >&2
	exit 2
}

#

#checks
PROGS="bash pwd gcc g++ make ccache grep"
for PROGCHECK in $PROGS; do
	if [[ -z "$(command -v $PROGCHECK)" ]]; then
		echo "Error: $PROGCHECK is not installed." >&2
		exit 1
	fi
done

CURR=$(pwd)

if [[ -z "${CROSS_COMPILE// }" ]]; then
	echo "Var CROSS_COMPILE is not defined!" >&2
fi


op=$1
if [ $# -ne 1 ];
then
	print_usage
fi

CPUNUM=$(grep -c ^processor /proc/cpuinfo)

cd $CURR

[[ -d out ]] || mkdir out

case $op in
	build)
		echo "building..."
		if [ ! -f out/.config ]; then
			echo "Config file not found, invoking make"
			 make ARCH=arm64 O=out CCACHE=ccache merge_hi3650_defconfig
		fi
		make ARCH=arm64 O=out CCACHE=ccache -j$CPUNUM
		;;
	buildsingle)
		echo "building..."
		if [ ! -f out/.config ]; then
			echo "Config file not found, invoking make"
			 make ARCH=arm64 O=out CCACHE=ccache merge_hi3650_defconfig
		fi
		make ARCH=arm64 O=out CCACHE=ccache -j1
		;;
	clean)
		echo "cleaning..."
		make ARCH=arm64 O=out CCACHE=ccache clean
		;;
	superclean)
		echo "supercleaning..."
		rm -Rf out
		;;
	*)
		print_usage
		;;
esac

