#!/bin/bash
docker run -v $(pwd):/workspace -w /workspace -it registry.cn-hangzhou.aliyuncs.com/dteegen/sign_tool:1.0.0 /bin/bash -c '
cd lib/host && make clean && GCC=gcc ARC=ar LINK=ld make && cd ../..
cd lib/gm && make clean && GCC=gcc ARC=ar LINK=ld make && cd ../..
SDK=$(pwd) && cd sign_tool && make clean && PENGLAI_SDK=$SDK make
'
