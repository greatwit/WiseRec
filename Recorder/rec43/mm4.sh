#!/bin/bash


mm
echo -e 'build rec media done..........................\n'

#plib=../../../../out/target/product/generic/obj/lib
plib=../../../../../out/target/product/generic/system/lib
cp $plib/libwise_rec43.so .
echo -e 'copy libs done...........................\n'



