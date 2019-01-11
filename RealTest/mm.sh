#!/bin/bash


mm

TARGET=libNativeCamera.so
TARGET_=libNativeCamera.so

echo -e "build ${TARGET} done..........................\n"

plib=../../../../../out/target/product/generic/system/lib
mv $plib/$TARGET ./$TARGET
echo -e "mv ${TARGET} done...........................\n"



TARGET=libNativeCodec.so
TARGET_=libNativeCodec.so

echo -e "build ${TARGET} done..........................\n"

plib=../../../../../out/target/product/generic/system/lib
mv $plib/$TARGET ./$TARGET
echo -e "mv ${TARGET} done...........................\n"