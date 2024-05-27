#!/bin/bash
if [ ! -d sdk ]; then
	echo "please execute the script at the project root"
	exit 1
fi
docker run -v $(pwd):/workspace -w /workspace -it registry.cn-hangzhou.aliyuncs.com/dteegen/sign_tool:1.0.0 /bin/bash -c '
cd sdk
cd lib/host && make clean && GCC=gcc ARC=ar LINK=ld make && cd ../..
cd lib/gm && make clean && GCC=gcc ARC=ar LINK=ld make && cd ../..
SDK=$(pwd) && cd sign_tool && make clean && PENGLAI_SDK=$SDK make && cd ..
cd lib/host && make clean && cd ../..
cd lib/gm && make clean && cd ../..
'
