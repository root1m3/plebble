#!/bin/bash
#===-                           P L E B B L E
#===-                         https://plebble.us
#===-
#===-              Copyright (C) 2017-2022 root1m3@plebble.us
#===-
#===-                      GNU GENERAL PUBLIC LICENSE
#===-                       Version 3, 29 June 2007
#===-
#===-    This program is free software: you can redistribute it and/or modify
#===-    it under the terms of the GPLv3 License as published by the Free
#===-    Software Foundation.
#===-
#===-    This program is distributed in the hope that it will be useful,
#===-    but WITHOUT ANY WARRANTY; without even the implied warranty of
#===-    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#===-
#===-    You should have received a copy of the General Public License
#===-    along with this program, see LICENCE file.
#===-    see https://www.gnu.org/licenses
#===-
#===----------------------------------------------------------------------------
#===-

function do_help {
	echo "blobmake"
	echo "./make.sh [--jail <jail>] <command>"
	echo "Commands:"
	echo " r2r_sdk                              Extracts the sdk for creating r2r wapplets."
	echo " r2r_lib_list_available               Print r2r wallet plugins available in blob."
	echo " r2r_lib_list_installed               "
	echo " r2r_lib_list_default                 "
	echo " r2r_lib_install <prot-role>          "
	echo " r2r_lib_uninstall <prot-role>        "
	echo " r2r_lib_commit                       "

	echo " apply_blob [apply_blob options] apply_blob."
}

jaildir=""
cmd=""
cmdargs=""
function parse_input {
    echo "Parse input: $@"
    opt=""
    while [[ true ]]; do
        opt=$1
        shift
        if [[ "_$opt" == "_--jail" ]]; then
            jaildir=$1
            shift
            continue
        elif [[ _$opt == _-* ]]; then
            do_help
            echo "KO 86755 Invalid option $opt"
            exit 1
        else
            break
        fi
    done
    cmd=$opt
    if [[ "_$cmd" == "_" ]]; then
        cmd="install"
    fi
    cmdargs="$@"
    echo "cmd=$cmd"
    echo "cmdargs=$cmdargs"
    #echo "cmd=$cmd"
}

parse_input $@

brand=`pwd | tr '/' ' ' | awk '{print $NF}' | tr '_' ' ' | awk '{ print $1 }'`
branch=`pwd | tr '/' ' ' | awk '{print $NF}' | tr '_' ' ' | awk '{ print $2 }'`
hash=`pwd | tr '/' ' ' | awk '{print $NF}' | tr '_' ' ' | awk '{ print $3 }'`

echo jaildir=$jaildir
echo cmd=$cmd
echo cmdargs=$cmdargs

function r2r_lib_list_installed {
    US=$brand
    ch=`cat $jaildir/var/us_nets | grep " $US " | awk '{print $1}'`
    if [[ $? -ne 0 ]]; then
        echo "KO 94752 brand $US is not installed."
        exit 1
    fi
    if [[ "_$ch" == "_" ]]; then
        echo "KO 94749 brand $US is not installed."
        exit 1
    fi
    ushome="/home/gov/.${US}"
    if [ "_$ch" != "_0" ]; then
        ushome="$ushome/$ch"
    fi
    whome="$ushome/wallet"
    thome="$whome/trader"
    r2rdst="$thome/lib"
    echo "r2r libs at $thome/lib"
    if [[ -d $jaildir$r2rdst ]]; then
        for f in `find $jaildir$r2rdst -name "*.so"`; do
            n=${f##*/}
            echo "$jaildir$r2rdst   --  $n"
        done
    fi
}

opt=""
if [[ "_$jaildir" != "_" ]]; then
    opt="--jail $jaildir"
fi

if [[ "_$cmd" == "_apply_blob" ]]; then
    bin/apply_blob $opt $cmdargs
    exit $?
fi
if [[ "_$cmd" == "_install" ]]; then
    bin/apply_blob $opt $cmdargs install
    exit $?
fi
if [[ "_$cmd" == "_r2r_sdk" ]]; then
    rm -rf r2r_sdk
    tar -I zstd -xf core_${branch}_${hash}.tar.zst
    pushd core > /dev/null
        make PREFIX=../r2r_sdk install-dev
    popd > /dev/null
    rm -rf core
    cp src/core1/us r2r_sdk/ -R
    cp src/core1/makefile r2r_sdk/
    echo "r2r_sdk"
    exit 0
fi
if [[ "_$cmd" == "_r2r_lib_list_available" ]]; then
    ls -1 lib/`uname -m`/r2r/*
    exit 0
fi
if [[ "_$cmd" == "_r2r_lib_list_installed" ]]; then
    r2r_lib_list_installed
    exit 0
fi
if [[ "_$cmd" == "_r2r_lib_list_default" ]]; then
    bin/apply_blob $opt $cmdargs r2r_lib_list_default
    exit $?
fi
if [[ "_$cmd" == "_r2r_lib_install" ]]; then
    bin/apply_blob $opt r2r_lib_install $cmdargs 
    exit $?
fi
if [[ "_$cmd" == "_r2r_lib_uninstall" ]]; then
    bin/apply_blob $opt r2r_lib_uninstall $cmdargs
    exit $?
fi
if [[ "_$cmd" == "_r2r_lib_commit" ]]; then
    bin/apply_blob $opt r2r_lib_commit $cmdargs
    exit $?
fi

do_help
exit 1

