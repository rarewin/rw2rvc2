#!/bin/bash

./release/rw2rvc2 "$1" > /dev/null 2>&1

RESULT=$?

echo -n "\"$1\" (error is expected) ... "

if [ "${RESULT}" != "0" ]; then
    echo -e "\e[1;32mFAILD -> OK\e[m"
else
    echo -e "\e[1;31mINCORRECTLY SUCCEEDED -> NG\e[m"
fi
