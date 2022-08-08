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

function help {
    echo "./make.sh <options> <command>"
    echo "Comnmands:"
    echo "    all|<empty>"
    echo "    clean"
    echo "    clean_deps"
    echo "    help"
    echo "Options"
    echo "    --bundle"
}


pwd=`pwd`
strset="en-uk"

. ../res/strings/$strset
if [[ -f ../res/strings/${strset}.ovr ]]; then
    echo "found override string set"
    . ../res/strings/${strset}.ovr
fi

dbg=$str29
logs=$str33
us=$str3

if [[ ! -f app/src/main/res/values/settings.xml ]]; then
    echo "KO 44372 Not configured."
    exit 1
fi

cmd=""
let bundle=0
while [[ true ]]; do
    opt=$1
    shift
    if [[ "_$opt" == "_--bundle" ]]; then
        echo " [local build]"
        let bundle=1
        continue
    elif [[ "_$opt" == "_-h" ]]; then
        help
        exit 0
    else
        break
    fi
done
cmd=$opt
if [[ "_$cmd" == "_help" ]]; then
    help
    exit 0
fi

echo "dbg=$dbg"
echo "logs=$logs"
flags=""
gradlecmd="assembleDebug"
if [[ $dbg -eq 0 ]]; then
    gradlecmd="assembleRelease"
    flags="$flags --warning-mode none"
else
#    flags="$flags --warning-mode all --stacktrace --console=verbose"
    flags="$flags --warning-mode all"
fi

if [[ "_$cmd" == "_clean" ]]; then
    rm build -rf
    rm app/build -rf
    rm wearable_JClife/build -rf
    rm blesdk_2025/build -rf
    ANDROID_HOME="`pwd`/android_sdk" ANDROID_SDK_ROOT="$asdk" ./gradlew $flags clean
    exit 0
fi

if [[ "_$cmd" == "_cleandeps" ]]; then
    exit 0
fi

if [[ "_$cmd" != "_" ]]; then
if [[ "_$cmd" != "_all" ]]; then
    echo "KO 89799 Invalid command $cmd"
    exit 1
fi
fi

function check_android_sdk {  #Search K8750R3. SYNC core1/snippet/build_job_make
    sdkmanager=cmdline-tools/latest/bin/sdkmanager
    if [[ ! -f ~/android_sdk/$sdkmanager ]]; then
        echo "pruning old android_sdk"
        rm -r ~/android_sdk
    fi
    if [[ ! -d ~/android_sdk ]]; then
        echo "recreating android_sdk API 32"
        mkdir ~/android_sdk
        for c0dir in `find . -type d -name "core0"`; do
            cp $c0dir/us/android/licenses ~/android_sdk/ -R
            break
        done
        pushd ~/android_sdk >/dev/null
#            sdktools=sdk-tools-linux-4333796.zip
            sdktools=commandlinetools-linux-8512546_latest.zip
            wget https://dl.google.com/android/repository/$sdktools
            unzip $sdktools
            rm $sdktools
            #workaround problem
            mkdir cmdline-tools/latest
            mv cmdline-tools/* cmdline-tools/latest/
            $sdkmanager  "platform-tools"
            $sdkmanager "build-tools;33.0.0"  #Search R6Tr465. SYNC android/app/build.gradle
            $sdkmanager "platforms;android-32"
            #tools/latest/bin/sdkmanager "platform-tools"
            #tools/bin/sdkmanager "build-tools;28.0.3"
            #$sdkmanager "platforms;android-28"
        popd >/dev/null
    fi
}
echo "android sdk"
check_android_sdk

pushd ~/android_sdk >/dev/null
    asdk=`pwd`
popd >/dev/null

echo "sdk.dir = $asdk" > local.properties
echo "gradlecmd $gradlecmd"
ANDROID_HOME="$asdk" ANDROID_SDK_ROOT="$asdk" ./gradlew $flags $gradlecmd
r=$?
if [[ $r -ne 0 ]]; then
    echo "errors found."
    exit $r
fi

if [[ ! $bundle -eq 1 ]]; then
    echo "OK"
    exit 0
fi
echo "Building bundle"

if [[ ! -d ~/playstore_tools ]]; then
    mkdir ~/playstore_tools
    pushd ~/playstore_tools > /dev/null
        wget https://github.com/google/bundletool/releases/download/1.11.0/bundletool-all-1.11.0.jar
        wget https://dl.google.com/dl/android/maven2/com/android/tools/build/aapt2/7.4.0-alpha09-8841542/aapt2-7.4.0-alpha09-8841542-linux.jar
        jar xf aapt2-7.4.0-alpha09-8841542-linux.jar
        chmod +x aapt2
    popd > /dev/null
fi

rm -rf bundle
mkdir -p bundle

aapt2=~/playstore_tools/aapt2
pushd bundle > /dev/null
    mkdir output

    echo "Compiling resources for app"
    mkdir output/app
    $aapt2 compile --dir ../app/src/main/res -o output/app/

    echo "Compiling resources for blesdk_2025"
    mkdir output/blesdk_2025
    $aapt2 compile --dir ../blesdk_2025/src/main/res -o output/blesdk_2025/

    echo "Compiling resources for wearable_JClife"
    mkdir output/wearable_JClife
    $aapt2 compile --dir ../wearable_JClife/src/main/res -o output/wearable_JClife/

    targetapi=`cat ../app/build.gradle | grep "targetSdkVersion" | awk '{print $2}'`
    androidsdk=~/android_sdk/platforms/android-${targetapi}/android.jar
    echo "Linking resources using target API $targetapi"


    $aapt2 link --proto-format -o output.apk -I ${androidsdk} \
    --manifest ../app/src/main/AndroidManifest.xml -R output/app/*.flat \
    --manifest ../blesdk_2025/src/main/AndroidManifest.xml -R output/blesdk_2025/*.flat \
    --manifest ../wearable_JClife/src/main/AndroidManifest.xml -R output/wearable_JClife/*.flat \
    --auto-add-overlay

popd > /dev/null



exit 0
