#!/bin/bash


mm

TARGET=libCamera.so
TARGET_=libCamera7.so

echo -e "build ${TARGET} done......\n"

plib=../../../../../../../out/target/product/generic/system/lib
mv $plib/$TARGET ../../bin/$TARGET_
echo -e "mv ${TARGET} to ${TARGET_} done......\n"


TARGET=libCameraTest.so
TARGET_=libCameraTest7.so

echo -e "build ${TARGET} done......\n"

plib=../../../../../../../out/target/product/generic/system/lib
mv $plib/$TARGET ../../bin/$TARGET_
echo -e "mv ${TARGET} to ${TARGET_} done......\n"
