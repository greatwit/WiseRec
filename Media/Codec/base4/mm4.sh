#!/bin/bash


mm

TARGET=libCodecBase.so
TARGET_=libCodecBase4.so

echo -e "build ${TARGET} done..........................\n"

plib=../../../../../../../out/target/product/generic/system/lib
mv $plib/$TARGET ../../bin/$TARGET_
echo -e "mv ${TARGET} done...........................\n"



