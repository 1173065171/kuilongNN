#!/bin/bash

# 清理项目的脚本
# 删除开发工具和构建文件

echo "开始清理项目..."

# 删除IDE和构建相关目录
rm -rf .git
rm -rf .idea
rm -rf build
rm -rf test
rm -rf tmp

# 删除源代码目录中的C++文件
find ./src/ -name "*.cpp" -exec rm -rf {} \;
find ./src/ -name "*.hpp" -exec rm -rf {} \;

# 删除Python缓存文件
find ./ -name "__pycache__" -exec rm -rf {} \;

# 删除firmware目录下除了gcc_backend之外的文件夹
find ./firmware -mindepth 1 -maxdepth 1 -type d ! -name "gcc_backend" -exec rm -rf {} \;

# 删除Cython的编译脚本
rm -rf cython_setup.py
rm -rf .gitignore
rm -rf command.md
rm -rf main.py
mv main_kui.py main.py


echo "项目清理完成！"
