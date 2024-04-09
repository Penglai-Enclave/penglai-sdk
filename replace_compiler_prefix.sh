#!/bin/bash

# 检查是否存在riscv64-unknown-linux-gnu-相关的交叉编译工具
if command -v riscv64-unknown-linux-gnu-gcc &> /dev/null; then
    echo "riscv64-unknown-linux-gnu-交叉编译工具链已安装，无需修改"
else
    echo "未检测到riscv64-unknown-linux-gnu-交叉编译工具链，开始替换文件中的默认编译工具"
    # 查找当前目录及子目录中所有Makefile、app.mk、config.mak文件，并替换其中的riscv64-unknown-linux-gnu-字符串
    find . \( -name Makefile -o -name app.mk -o -name config.mak \) -type f -exec sed -i 's/riscv64-unknown-linux-gnu-//g' {} +
    find . \( -name Makefile -o -name app.mk -o -name config.mak \) -type f -exec sed -i 's/riscv64-unknown-elf-//g' {} +
    find . \( -name Makefile -o -name app.mk -o -name config.mak \) -type f -exec sed -i '/TOOLCHAINPATH :=/c\TOOLCHAINPATH :=' {} +
    find . \( -name Makefile -o -name app.mk -o -name config.mak \) -type f -exec sed -i '/TOOLCHAINPATH :=/c\TOOLCHAINPATH :=' {} +

    #暂时不支持evm编译，待处理
    find . \( -name Makefile -o -name app.mk -o -name config.mak \) -type f -exec sed -i -e '/\tmake -C evm$/s/^/#/' -e '/\tmake -C evm\/host$/s/^/#/' {} +

    echo "替换完成"
fi
