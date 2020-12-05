#!/bin/zsh

echo "Running unit tests:"

rm ./tests.log

all_tests=0
ok=0

echo "lib tests: ----------"
for i in build/tests/lib/*_tests
do
    if test -f $i
    then
        if $VALGRIND ./$i 2>> ./tests.log
        then
            let ok++;
            echo "[PASS]"
        else
            echo "[FAILED]"
            # tail tests/tests.log
        fi
    fi
    let all_tests++;
done

echo "toy tests: ----------"
for i in build/tests/toy/*_tests
do
    if test -f $i
    then
        if $VALGRIND ./$i 2>> ./tests.log
        then
            let ok++;
            echo "[PASS]"
        else
            echo "[FAILED]"
            # tail tests/tests.log
        fi
    fi
    let all_tests++;
done

echo "----"
echo "${ok}/${all_tests} passed"
