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

cachedir=~/wget_cache
mkdir -p $cachedir
usdir=../../..
javac=`which javac`
if [ "_$JAVA_HOME" != "_" ]; then
    javac=$JAVA_HOME/bin/javac
fi

cmd=$1
shift

cyphersuite="bouncycastle"
if [ "_$cmd" == "_-android" ]; then
    cyphersuite="spongycastle"
    cmd=$1
    shift
elif [ "_$cmd" == "_-spongy" ]; then
    cyphersuite="spongycastle"
    cmd=$1
    shift
elif [ "_$cmd" == "_-bouncy" ]; then
    cyphersuite="bouncycastle"
    cmd=$1
    shift
elif [ "_$cmd" == "_spongy" ]; then
    cyphersuite="spongycastle"
    cmd=$1
    shift
elif [ "_$cmd" == "_bouncy" ]; then
    cyphersuite="bouncycastle"
    cmd=$1
    shift
elif [ "_$cmd" == "_-console" ]; then
    cyphersuite="bouncycastle"
    cmd=$1
    shift
else
    cyphersuite="bouncycastle"
fi

android=0
suffix="bc"
if [ "_$cyphersuite" == "_spongycastle" ]; then
    libspath="..:../libs_spongy/core-1.58.0.0.jar:../libs_spongy/pg-1.54.0.0.jar:../libs_spongy/pkix-1.54.0.0.jar:../libs_spongy/prov-1.58.0.0.jar"
    android=1
    suffix="sc"
else
    libspath="..:../libs_bouncy/bcprov-jdk15on-164.jar:../libs_bouncy/bcpkix-jdk15on-164.jar:../libs_bouncy/bcpg-jdk15on-164.jar:../libs_bouncy/bctls-jdk15on-164.jar:../libs_bouncy/bcmail-jdk15on-164.jar"
    android=0
    suffix="bc"
fi
echo "cyphersuite $cyphersuite suffic $suffix"

strset="en-uk"
. ../../../res/strings/$strset
if [ -f ../../../res/strings/${strset}.ovr ]; then
    echo "found override string set"
    . ../../../res/strings/${strset}.ovr
fi

if [ ! -f us/CFG.java ]; then
    echo "KO 79986 Not configured."
    exit 1
fi

dbg=$str29
logs=$str33
us=$str3

flags="-g "
if [ $dbg -eq 0 ]; then
    flags="-g:none "
fi

echo "dbg=$dbg"
echo "logs=$logs"
echo "Flags: $flags"

function clean {
    rm -rf libs_bouncy
    rm -rf libs_spongy
    clean_bin
}

function clean_bin {
    rm -rf build
    rm -rf test_build
    rm -f *.jar
}

function wget_cache {
    url=$1
    out=$2
    hash=`echo "--$url--" | sha256sum | awk '{print $1}'`
    while [ true ]; do
            if [ -f $cachedir/$hash ]; then
        echo "Copying from cached file $cachedir/$hash"
        cp $cachedir/$hash $out
        return
            fi
            echo "updating cache $cachedir/$hash"
        mkdir -p $cachedir
        wget $url -O $cachedir/$hash
    done
}

function getdeps {
    if [ $android -eq 1 ]; then
        if [ ! -f libs_spongy/pg-1.54.0.0.jar ]; then
            mkdir libs_spongy
            wget_cache http://search.maven.org/remotecontent?filepath=com/madgag/spongycastle/core/1.58.0.0/core-1.58.0.0.jar libs_spongy/core-1.58.0.0.jar
            wget_cache http://search.maven.org/remotecontent?filepath=com/madgag/spongycastle/prov/1.58.0.0/prov-1.58.0.0.jar libs_spongy/prov-1.58.0.0.jar
            wget_cache http://search.maven.org/remotecontent?filepath=com/madgag/spongycastle/pkix/1.54.0.0/pkix-1.54.0.0.jar libs_spongy/pkix-1.54.0.0.jar
            wget_cache http://search.maven.org/remotecontent?filepath=com/madgag/spongycastle/pg/1.54.0.0/pg-1.54.0.0.jar libs_spongy/pg-1.54.0.0.jar
        fi
    fi
    if [ $android -eq 0 ]; then
        if [ ! -f libs_bouncy/bcmail-jdk15on-164.jar ]; then
            mkdir libs_bouncy
            wget_cache https://www.bouncycastle.org/download/bcprov-jdk15on-164.jar libs_bouncy/bcprov-jdk15on-164.jar
            wget_cache https://www.bouncycastle.org/download/bcpkix-jdk15on-164.jar libs_bouncy/bcpkix-jdk15on-164.jar
            wget_cache https://www.bouncycastle.org/download/bcpg-jdk15on-164.jar libs_bouncy/bcpg-jdk15on-164.jar
            wget_cache https://www.bouncycastle.org/download/bctls-jdk15on-164.jar libs_bouncy/bctls-jdk15on-164.jar
            wget_cache https://www.bouncycastle.org/download/bcmail-jdk15on-164.jar libs_bouncy/bcmail-jdk15on-164.jar
        fi
    fi
}

function dosource {
    jarfile=$us-sdk_$suffix.jar
    make flags="$flags" libspath="$libspath" suffix="$suffix" us="$us"  jar -j8
}

function dosource0 {
    rm -f $jarfile
    mkdir -p build
    echo "using javac=$javac"
    echo "compiling files:"
    files = `find us -type f -name "*.java" | sort`
    for f in files; do
        $javac -Xlint $flags -d build -cp $libspath $f
    done
    pushd build >/dev/null
       jar cf ../$jarfile *
       echo "produced file $jarfile"
    popd >/dev/null
}

function test {
    jarfile=$us-sdk-test_$suffix.jar
    make flags="$flags" libspath="$libspath" suffix="$suffix" us="$us"  test -j8
    return;

        jarfile=$us-sdk_$suffix.jar
        jartfile=$us-sdk-test_${suffix}.jar
    rm -f $jartfile
        mkdir -p test_build
        echo "compiling files:"
        find test/us -type f -name "*.java"

        find test/us -type f -name "*.java" -exec $javac -Xlint $flags -d test_build -cp $jarfile:$libspath {} \;
        echo "Compiling main"
        $javac -Xlint $flags -d test_build -cp test_build:$jarfile:$libspath test/main.java
        echo "packing"
    pushd test_build >/dev/null
            jar cfe ../$jartfile main.class *
            echo "produced file $jartfile"
    popd >/dev/null
}

function select_cryptosuite {
    androidlast=`cat selected_cryptosuite`
    if [[ "_$androidlast" == "_$android" ]]; then
    echo "Source already include the selected cryptolib"
    return
    fi
    echo -n "Patching sources: "
    if [ $android -eq 1 ]; then
        echo "using spongycastle"
        find us/gov/crypto -name "*.java" -exec sed -i "s/import org.bouncycastle./import org.spongycastle./g" {} \;
        find us/gov/socket -name "*.java" -exec sed -i "s/import org.bouncycastle./import org.spongycastle./g" {} \;
        find test/us/gov/crypto -name "*.java" -exec sed -i "s/import org.bouncycastle./import org.spongycastle./g" {} \;
    else
        echo "using bouncycastle"
        find us/gov/crypto -name "*.java" -exec sed -i "s/import org.spongycastle./import org.bouncycastle./g" {} \;
        find us/gov/socket -name "*.java" -exec sed -i "s/import org.spongycastle./import org.bouncycastle./g" {} \;
        find test/us/gov/crypto -name "*.java" -exec sed -i "s/import org.spongycastle./import org.bouncycastle./g" {} \;
    fi
    echo "$android" > selected_cryptosuite
}

function all {
    echo "compiling using $javac"
    select_cryptosuite
    getdeps
    if [ "_$cyphersuite" == "_spongycastle" ]; then
        echo "cyphersuite:  ( )bouncycastle  (*)spongycastle"
    else
        echo "cyphersuite:  (*)bouncycastle  ( )spongycastle"
    fi
    dosource
    test
    echo ""
}

echo "cmd $cmd"
if [ "_$cmd" == "_" ]; then
    cmd="all"
fi

if [ "_$cmd" == "_all" ]; then
    all
fi

if [ "_$cmd" == "_source" ]; then
    dosource
fi

if [ "_$cmd" == "_test" ]; then
    test
fi

if [ "_$cmd" == "_clean" ]; then
    clean
fi

if [ "_$cmd" == "_clean-bin" ]; then
    clean_bin
fi

exit 0

