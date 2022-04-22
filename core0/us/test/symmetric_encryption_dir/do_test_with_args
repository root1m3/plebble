#!/bin/bash

read -r priv_a public_a <<< $(LD_LIBRARY_PATH=../../gov:/usr/lib ../helpers/generate_keys_cpp) 
read -r priv_b public_b <<< $(LD_LIBRARY_PATH=../../gov:/usr/lib ../helpers/generate_keys_cpp) 

MESSAGE=$1

encrypted="$(LD_LIBRARY_PATH=../../gov:/usr/lib ./test_cpp $priv_a $public_b encrypt "$MESSAGE")"

decrypted="$(java -cp ".:../../sdk/java/libs/*:../../sdk/java/us-wallet-sdk.jar" Main $priv_b $public_a "decrypt" $encrypted)"

encrypted2="$(java -cp ".:../../sdk/java/libs/*:../../sdk/java/us-wallet-sdk.jar" Main $priv_b $public_a "encrypt" "$MESSAGE")"

decrypted2="$(LD_LIBRARY_PATH=../../gov:/usr/lib ./test_cpp $priv_a $public_b decrypt $encrypted2)"
if [ "$MESSAGE" == "$decrypted" ] && [ "$MESSAGE" == "$decrypted2" ]
then
    echo "PASSED"
else 
    echo "FAILED"
fi



