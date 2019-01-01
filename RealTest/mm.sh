#!/bin/bash


mm
echo -e 'build native camera done..........................\n'

plib=../../../../../out/target/product/generic/system/lib
mv $plib/libNativeCamera.so ./libNativeCamera.so
echo -e 'copy libs done...........................\n'



