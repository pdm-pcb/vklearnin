#!/bin/bash

for dir in $(find -type d -iname build -o -iname debug -o -iname release);
	do rm -rf $dir;
done

for file in $(find -iname *.vert.d -o -iname *.vert.d -o -iname demo);
	do rm -f $file;
done
