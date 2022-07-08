#!/bin/bash -e
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

pwd=`pwd`

strset="en-uk"

. ../res/strings/$strset
if [ -f ../res/strings/${strset}.ovr ]; then
    echo "found override string set"
    . ../res/strings/${strset}.ovr
fi

dbg=$str29
logs=$str33
us=$str3

#if [ $logs -eq 0 ]; then
#    echo "WARNING Ignoring flag --without-logs"
#fi
if [ ! -f app/src/main/res/values/settings.xml ]; then
    echo "KO 44372 Not configured."
    exit 1
fi

echo "dbg=$dbg"
echo "logs=$logs"

if [ "_$1" == "_clean" ]; then
    rm build -rf
    rm app/build -rf
    rm wearable_JClife/build -rf
    rm blesdk_2025/build -rf
    ANDROID_HOME="`pwd`/android_sdk" ANDROID_SDK_ROOT="$asdk" ./gradlew clean
    exit 0
fi

if [ "_$1" == "_cleandeps" ]; then
#    rm android_sdk -rf
    exit 0
fi

gradlecmd="assembleDebug"
if [ $dbg -eq 0 ]; then
    gradlecmd="assembleRelease"
fi

echo "android sdk"

if [ ! -d ~/android_sdk ]; then
    mkdir ~/android_sdk
    cp licenses ~/android_sdk/ -R
    pushd ~/android_sdk
      sdktools=sdk-tools-linux-4333796.zip
      wget https://dl.google.com/android/repository/$sdktools
      unzip $sdktools
      rm $sdktools
      tools/bin/sdkmanager "platform-tools"
      tools/bin/sdkmanager "build-tools;28.0.3"
      tools/bin/sdkmanager "platforms;android-28"
    popd
fi

pushd ~/android_sdk >/dev/null
    asdk=`pwd`
popd >/dev/null

echo "sdk.dir = $asdk" > local.properties

echo "gradlecmd $gradlecmd"

#set +e
r=0
ANDROID_HOME="`pwd`/android_sdk" ANDROID_SDK_ROOT="$asdk" ./gradlew --console=verbose --warning-mode all $gradlecmd || r=$?
#&& (exit 1)

#set -e
echo "OK"
exit $r
