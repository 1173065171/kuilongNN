import sys

# 定义输入和输出文件的名称
# input_file = 'kernel.hex'
# output_file = 'kernel.hpp'

# input_file = 'input.hex'
# output_file = 'input.hpp'

# input_file = 'bias.hex'
# output_file = 'bias.hpp'

# input_file = 'refer.hex'
# output_file = 'refer.hpp'

input_file = sys.argv[1]
output_file = sys.argv[2]
arrar_name = sys.argv[3]

# 用于存储所有解析后的十六进制数据
hex_data = []

try:
    # 打开输入文件并读取内容
    with open(input_file, 'r') as infile:
        for line in infile:
            line = line.strip()
            # 每两个字符一组进行分割
            for i in range(0, len(line), 2):
                hex_pair = line[i:i + 2]
                # 将十六进制字符串转换为整数
                hex_data.append(int(hex_pair, 16))

    # 计算数组元素的总数
    array_size = len(hex_data)

    # 打开输出文件以写入内容
    with open(output_file, 'w') as outfile:
        # 写入数组定义的开头部分
        # outfile.write(f"int8_t kernel_data[{array_size}] = {{\n")
        # outfile.write(f"int8_t input_data[{array_size}] = {{\n")
        # outfile.write(f"int8_t bias_data[{array_size}] = {{\n")
        outfile.write(f"__attribute__((aligned(16)))int8_t {arrar_name}[{array_size}] = {{\n")

        # 按每行 9 个元素的格式输出，最后一行除外
        for i in range(0, array_size, 8):
            line_elements = hex_data[i:i + 8]
            hex_strings = [f"(int8_t)0x{byte:02x}" for byte in line_elements]
            line = ', '.join(hex_strings)
            if i + 8 < array_size:
                line += ', \\'
            outfile.write(f"    {line}\n")

        # 写入数组定义的结尾部分
        outfile.write("};")

    print(f"成功生成 {output_file} 文件。")
except FileNotFoundError:
    print(f"未找到 {input_file} 文件，请检查文件名和路径。")
except ValueError:
    print("输入文件中包含非十六进制字符，请检查文件内容。")
except Exception as e:
    print(f"发生未知错误: {e}")
