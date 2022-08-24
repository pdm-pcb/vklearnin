#!/bin/bash

cd ./build/linux-debug

valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=memtest.log \
         ./vulkan-learnin

grep -ni "definitely" -A 10 memtest.log

cd ../..
