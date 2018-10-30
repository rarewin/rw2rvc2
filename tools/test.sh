#!/bin/bash

TEMP=`tempfile`

./release/rw2rvc2 "$1" > ${TEMP}.s
riscv64-linux-gnu-gcc -static ${TEMP}.s -o ${TEMP}
${TEMP}

RESULT=$?

echo -n "\"$1\" -> ${RESULT}  (expected: $2) ... "

if [ "${RESULT}" = "${2}" ]; then
    echo -e "\e[1;32mOK\e[m"
else
    echo -e "\e[1;31mNG\e[m"
fi

rm -f ${TEMP}.s ${TEMP}
