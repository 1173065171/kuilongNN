import re
import matplotlib.pyplot as plt

def read_file(file_path):
    try:
        with open(file_path, 'r') as file:
            return file.read()
    except FileNotFoundError:
        print(f"未找到 {file_path} 文件，请确保文件存在。")
        exit()
    except PermissionError:
        print(f"没有权限读取 {file_path} 文件，请检查文件权限。")
        exit()

def extract_matches(text):
    patterns = [
        r'slab order: (\d+)',
        r'slab size: (\d+)',
        r'slab addr: (\w+)',
        r'buddy order: (\d+)',
        r'buddy size: (\d+)',
        r'buddy addr: (\w+)',
        r'free addr: (\w+)'
    ]
    all_matches = []
    for pattern in patterns:
        for match in re.finditer(pattern, text):
            keyword = pattern.split(':')[0]
            all_matches.append((match.start(), keyword, match.group(1)))
    all_matches.sort(key=lambda x: x[0])
    return all_matches

def process_matches(all_matches):
    malloc_size = [0]
    current_size = 0
    addr_size_map = {}
    for _, keyword, value in all_matches:
        if keyword in ['slab size', 'buddy size']:
            # 将字符串形式的十六进制数转换为十进制整数进行计算
            current_size = int(value, 16)
        elif keyword in ['slab addr', 'buddy addr']:
            if current_size > 0:
                new_malloc_size = malloc_size[-1] + current_size
                malloc_size.append(new_malloc_size)
                # 存储十六进制字符串
                addr_size_map[value] = hex(current_size)
                current_size = 0
        elif keyword == 'free addr':
            if value in addr_size_map:
                # 将十六进制字符串转换为十进制整数进行计算
                size_to_free = int(addr_size_map[value], 16)
                new_malloc_size = malloc_size[-1] - size_to_free
                malloc_size.append(new_malloc_size)
                del addr_size_map[value]
        print(f"{keyword}: {value}")
    # 将最终的内存大小列表转换为十六进制字符串列表
    malloc_size_hex = [hex(size) for size in malloc_size]
    return malloc_size_hex


def plot_memory_allocation(malloc_size):
    # 将malloc_size的值转换为KB
    malloc_size_kb = [size / 1024 for size in malloc_size]
    x = range(len(malloc_size_kb))
    # 增大柱子宽度
    bar_width = 0.6
    # 增大图表的宽度
    plt.figure(figsize=(40, 10))
    bars = plt.bar(x, malloc_size_kb, width=bar_width, color='#FFB6C1', label='Memory Allocation')
    plt.plot(x, malloc_size_kb, color='#D8BFD8', marker='o', label='Memory Trend')

    # 找出尖峰点
    peak_points = []
    valley_points = []
    for i in range(1, len(malloc_size_kb) - 1):
        if malloc_size_kb[i] > malloc_size_kb[i - 1] and malloc_size_kb[i] > malloc_size_kb[i + 1]:
            peak_points.append(i)
        elif malloc_size_kb[i] < malloc_size_kb[i - 1] and malloc_size_kb[i] < malloc_size_kb[i + 1]:
            valley_points.append(i)

    # 标注尖峰点
    for i in peak_points:
        height = malloc_size_kb[i]
        plt.text(i, height, f'{height:.2f}', ha='center', va='bottom', fontsize=10)

    # 标注谷底点
    for i in valley_points:
        height = malloc_size_kb[i]
        plt.text(i, height, f'{height:.2f}', ha='center', va='top', fontsize=10)

    plt.xlabel('Step')
    plt.ylabel('Memory Size (KB)')
    plt.title('Memory Allocation and Deallocation Trend')

    # 增大 x 轴标注的间隔距离，这里设置为每隔 5 个刻度显示一个
    interval = 5
    plt.xticks([i for i in x if i % interval == 0])

    # 设置 x 轴的显示范围，消除前后的空白
    plt.xlim(-bar_width / 2, len(x) - 1 + bar_width / 2)

    plt.legend()
    plt.grid(True)

    plt.savefig('memory_allocation_plot.png')

if __name__ == "__main__":
    file_path = '../log/gcc_backend.log'
    text = read_file(file_path)
    all_matches = extract_matches(text)
    malloc_size = process_matches(all_matches)
    # print(malloc_size)
    # 先将整数转换为十六进制字符串，再转换为十进制
    # malloc_size_str = [str(hex(num)).replace("0x", "") for num in malloc_size]
    # print(malloc_size_str)
    decimal_malloc_size = [int(num, 16) for num in malloc_size]
    print(decimal_malloc_size)
    plot_memory_allocation(decimal_malloc_size)
