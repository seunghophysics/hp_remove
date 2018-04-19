#!/bin/bash
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$ROOTSYS/lib
for filename in BPM_ROOT/*.root;
do
    rootfile="hp_removed/$(basename "$filename")"
    if [ -e $rootfile ]; then
	echo "$rootfile exists"
    else
	echo "$rootfile does not exist"
	./hp_remove $filename $rootfile 300
    fi
done
