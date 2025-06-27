#!/usr/bin/env python3
#
# This is free and unencumbered software released into the public domain.
#
# Anyone is free to copy, modify, publish, use, compile, sell, or
# distribute this software, either in source code form or as a compiled
# binary, for any purpose, commercial or non-commercial, and by any
# means.

from sys import argv

filename = argv[1]

def process_hex_file(input_file):
    # 打开8个文件，分别存储mod4结果
    input_file_without_suffix = input_file.rsplit('.', 1)[0]
    output_files = [open(f"{input_file_without_suffix}_mod4_{i}.hex", "w") for i in range(4)]

    with open(input_file, "r") as infile:
        for line_number, line in enumerate(infile):
            mod4 = line_number % 4
            output_files[mod4].write(line)

    # 关闭所有文件
    for f in output_files:
        f.close()

if __name__ == "__main__":
    # 使用函数处理文件
    process_hex_file(filename)
