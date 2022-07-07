#!/bin/bash
scp -P 12055 ./aes ./prime ./qsort ./dhrystone ./miniz ./norx ./sha512  root@localhost:~/rv8-bench-host
