#!/bin/bash


mm
echo -e 'build rec media done..........................\n'

plib=../../../../../../../out/target/product/generic/system/lib
mv $plib/libCamera.so ../../bin/libCamera4.so
echo -e 'mv libs done...........................\n'



