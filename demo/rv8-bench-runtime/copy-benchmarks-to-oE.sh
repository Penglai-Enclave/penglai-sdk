#!/bin/bash
scp -P 12055 ../aes-runtime/aes ../prime-runtime/prime ./qsort ./dhrystone ./miniz ./norx ./sha512  root@localhost:~/rv8-runtime/
