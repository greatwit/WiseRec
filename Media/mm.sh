#!/bin/bash


mm

TARGET=libgreat_media.so
TARGET_=libgreat_media.so

echo -e "build ${TARGET} done..........................\n"

plib=../../../../../out/target/product/generic/system/lib
mv $plib/$TARGET ./$TARGET
echo -e "mv ${TARGET} done...........................\n"

