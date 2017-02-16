#!/bin/bash

bin="critto-satto --key key.bin --alg alg.bin "
f="__test_unique"
fd="__same_test_unique"
fe="__enc_test_unique"
cmd="../critto_santo "
key=" --key key.bin"
alg=" --alg alg.bin"

test_fail () {
    $bin -d --in $1 1>/dev/null
    cat $1 | $bin -d 1>/dev/null
    $bin -a --in $1 1>/dev/null
    cat $1 | $bin -a 1>/dev/null
}

echo "Testing working on wrong files"

echo "HELLO WORLD" > $f
test_fail $f
test_fail msg.bin
rm $f

