#!/bin/sh
gcc -O0 -m32 -o datagen datagen.c \
&& gcc -O2 -o test4 test.c -I ../../nemu -I ../../nemu/include -D DATA_BYTE=4 \
&& gcc -O2 -o test2 test.c -I ../../nemu -I ../../nemu/include -D DATA_BYTE=2 \
&& gcc -O2 -o test1 test.c -I ../../nemu -I ../../nemu/include -D DATA_BYTE=1 \
&& (./datagen | ./test4) \
&& (./datagen | ./test2) \
&& (./datagen | ./test1) \
&& echo OK || echo FAILED

rm -f test4 test2 test1 data.txt datagen
