#!/bin/bash

for key in $(ls /tmp/key_*)
do
    /tmp/cryptolock -d "$key"
    if [ $? -eq 0 ]
    then
        break
    fi
done
