#!/bin/bash

# 清理项目的脚本
# 删除开发工具和构建文件

echo "开始清理项目..."

# 删除IDE和构建相关目录
rm -rf .idea
rm -rf build
rm -rf test
rm -rf tmp

# 删除源代码目录中的C++文件
find ./src/ -name "*.cpp" -exec rm -rf {} \;
find ./src/ -name "*.hpp" -exec rm -rf {} \;

# 删除Python缓存文件
find ./ -name "__pycache__" -exec rm -rf {} \;

rm -rf cython_setup.py

echo "项目清理完成！"
