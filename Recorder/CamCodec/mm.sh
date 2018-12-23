#!/bin/bash


mm
echo -e 'build rec media done..........................\n'

#plib=../../../../out/target/product/generic/obj/lib
plib=../../../../../../out/target/product/generic/system/lib
cp $plib/libCamCodec.so .
echo -e 'copy libs done...........................\n'



