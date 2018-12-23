#!/bin/bash


mm
echo -e 'build rec media done..........................\n'

plib=../../../../../../../out/target/product/generic/system/lib
cp $plib/libCamera.so ./libCamera4.so
echo -e 'copy libs done...........................\n'



