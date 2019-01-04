#!/bin/bash


mm

TARGET=libCamera.so
TARGET_=libCamera4.so

echo -e "build ${TARGET} done..........................\n"

plib=../../../../../../../out/target/product/generic/system/lib
mv $plib/$TARGET ../../bin/$TARGET_
echo -e "mv ${TARGET} done...........................\n"



