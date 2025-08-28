#!/bin/bash

case_name="$1"
echo "你输入的测试用例名称是: $case_name"

make all -B
make mod4
echo "模拟测试已完成"


target_dir="../../testcase/$case_name"
mkdir -p "$target_dir"

cp build/*.hex "$target_dir/"
cp build/*.txt "$target_dir/"
echo "文件已复制到目标文件夹"
