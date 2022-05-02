############cmd=$1

function help {
	echo "blobmake"
	echo "./make.sh r2r_sdk            Extracts the sdk for creating r2r wapplets."
}

brand=`pwd | tr '/' ' ' | awk '{print $NF}' | tr '_' ' ' | awk '{ print $1 }'`
branch=`pwd | tr '/' ' ' | awk '{print $NF}' | tr '_' ' ' | awk '{ print $2 }'`
hash=`pwd | tr '/' ' ' | awk '{print $NF}' | tr '_' ' ' | awk '{ print $3 }'`

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

help
exit 1
