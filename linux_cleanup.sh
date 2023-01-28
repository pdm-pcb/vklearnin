#!/bin/bash

for dir in $(find -type d -iname build -o -iname debug -o -iname release);
	do rm -rf $dir;
done
