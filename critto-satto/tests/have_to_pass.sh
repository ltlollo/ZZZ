#!/bin/bash

bin="critto-satto"
key="__key_unique"
alg="__alg_unique"
args=" --key $key --alg $alg "
f="__test_unique"
fd="__same_test_unique"
fe="__enc_test_unique"
fnobrake="DONOTBREAK.bin"
cmd=$bin$args

throw(){
    echo $1; exit 1
}

eq_or_die(){
    diff "$1" "$2" || throw "$1, $2, differs"
}

test_uses() {
    echo "Testing encription..."
    echo "Reading file from disk, output to disk"
    $cmd -e --in $f --out 1$fe
    echo "Reading file from stdin, output to disk"
    cat $f | $cmd -e --out 2$fe
    echo "Reading file from disk, output to stdin"
    $cmd -e --in $f > 3$fe
    echo "Reading file from stdin, output to stdin"
    cat $f | $cmd -e > 4$fe

    eq_or_die 1$fe 2$fe
    eq_or_die 1$fe 3$fe
    eq_or_die 1$fe 4$fe
    eq_or_die 2$fe 3$fe
    eq_or_die 2$fe 4$fe
    eq_or_die 3$fe 4$fe
    mv 1$fe $fe
    rm 2$fe 3$fe 4$fe
    echo "...DONE"

    echo "Testing decription..."
    echo "Reading file from disk, output to disk"
    $cmd -d --in $fe --out 1$fd
    echo "Reading file from stdin, output to disk"
    cat $fe | $cmd -d --out 2$fd
    echo "Reading file from disk, output to stdin"
    $cmd -d --in $fe > 3$fd
    echo "Reading file from stdin, output to stdin"
    cat $fe | $cmd -d > 4$fd

    eq_or_die 1$fd 2$fd
    eq_or_die 1$fd 3$fd
    eq_or_die 1$fd 4$fd
    eq_or_die 2$fd 3$fd
    eq_or_die 2$fd 4$fd
    eq_or_die 3$fd 4$fd
    eq_or_die $f 1$fd
    rm $fe 1$fd 2$fd 3$fd 4$fd
    echo "...DONE"
}

generate_tmp_keyalg() {
    echo "Generating temporary alg and key files"
    echo {1..121}|tr -d ' ' 1>$key
    echo {1..121}|tr -d ' ' 1>>$key
    echo {1..121}|tr -d ' ' 1>>$key
    echo {1..121}|tr -d ' ' 1>$alg
    echo {1..121}|tr -d ' ' 1>>$alg
}

test_input() {
    echo $1 > $f
    size=$(cat $f | wc -c)

    if [[ $size -eq 256 ]]; then echo "Testing msg of exactly 256 chars"
    elif [[ $((size%256)) -eq 0 ]]; then echo "Testing msg multiple of 256 chars"
    elif [[ $size -gt 256 ]];then echo "Testing messages bigger than 256 cahrs"
    else echo "Testing messages smaller than 256 cahrs"
    fi

    test_uses
    rm $f
}

test_known(){
    echo "Testing known message"
    echo {1..127}|tr -d ' ' | $cmd -e > $f
    eq_or_die $f $fnobrake
    echo "...DONE"
    rm $f
}

main() {
    generate_tmp_keyalg

    test_input "HELLO WORLD"
    test_input $(echo {1..127}|tr -d ' ')
    test_input $(echo {1..121}|tr -d ' ')
    test_input $(cat $alg)
    test_known

    rm $key $alg

    exit 0
}

main 2>/dev/null
