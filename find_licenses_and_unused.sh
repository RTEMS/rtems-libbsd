#!/bin/bash

#
# Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
#
#  embedded brains GmbH
#  Dornierstr. 4
#  82178 Puchheim
#  Germany
#  <rtems@embedded-brains.de>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#

# exit on wrong command and undefined variable
set -e
set -u
set -o pipefail

SCRIPTNAME=$0
TARGET="freebsd/"
SOURCE="freebsd-org/"
LIST="libbsd.py"
FORCE=0

printhelp () {
	echo ""
	echo "Call:   ${SCRIPTNAME} <options>"
	echo "1. Find LICENSE files for each file in \"${TARGET}\" if the"
	echo "   license file is not in ${LIST}."
	echo "2. Find all files in \"${TARGET}\" that are not in \"${LIST}\"."
	echo "   Note: This function currently prints a lot of generated files too."
	echo "Reccomended usage:"
	echo "./${SCRIPTNAME} | sort | uniq"
	echo ""
	echo "The following parameters are optional:"
	echo "  -h          Print this help and exit the script."
	echo "  -f          Force printing all license files."
	echo "  -v          Be more verbose. Can be used multiple times."
	exit 0
}

while getopts "hfv" OPTION
do
	case ${OPTION} in
		h)  printhelp ;;
		f)  FORCE=1 ;;
		\?) echo "Unknown option \"-${OPTARG}\"."
			exit 1 ;;
		:)  echo "Option \"-${OPTARG}\" needs an argument."; exit 1 ;;
	esac
done
shift $((${OPTIND} - 1))

checkfile () {
	local FILE="$1"
	local FILE_WITHOUT_PATH=${FILE#"$TARGET"}
	local LICENSE=""

	grep "${FILE_WITHOUT_PATH}" "${LIST}" > /dev/null || \
	    echo "File not in ${LIST}: ${FILE_WITHOUT_PATH}"

	local DIR="${SOURCE}`dirname ${FILE_WITHOUT_PATH}`"
	while [ "$DIR" != "." ]
	do
		local MAYBELICENSE="${DIR}/LICENSE*"
		if [ -e ${MAYBELICENSE} ]
		then
			LICENSE=`ls ${MAYBELICENSE}`
			break
		fi
		local MAYBELICENSE="${DIR}/COPY*"
		if [ -e ${MAYBELICENSE} ]
		then
			LICENSE=`ls ${MAYBELICENSE}`
			break
		fi
		DIR="`dirname ${DIR}`"
	done

	if [ "${LICENSE}" != "" ]
	then
		local LICENSE_WITHOUT_PATH=${LICENSE#"$SOURCE"}
		if grep "${LICENSE_WITHOUT_PATH}" "${LIST}" > /dev/null
		then
			if [ ${FORCE} -ne 0 ]
			then
				echo "License file found: ${LICENSE}"
			fi
		else
			echo "New license file found: ${LICENSE}"
		fi
	fi
}

export -f checkfile
export TARGET
export SOURCE
export LIST
export FORCE

find ${TARGET} -name "*\.[ch]" -exec bash -c 'checkfile "$0"' {} \;
