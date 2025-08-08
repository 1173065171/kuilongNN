#!/usr/bin/env python3
"""
ELF符号大小分析脚本 - 增强版
分析从objdump或nm输出的符号表，计算代码段、数据段和只读数据段的符号大小
"""

import sys
import re
from typing import List, Tuple, Dict

def parse_symbol_line(line: str) -> Tuple[str, int, str, str]:
    """解析符号表行，返回(地址, 地址值, 类型, 符号名)"""
    # 匹配格式: 地址 类型 符号名
    match = re.match(r'^([0-9a-fA-F]{8,16})\s+([TtRrBbDdWwAaVvCcSsUu])\s+(.+)$', line.strip())
    if match:
        addr_str, symbol_type, symbol_name = match.groups()
        addr_value = int(addr_str, 16)
        return (addr_str, addr_value, symbol_type, symbol_name)
    return None

def clean_symbol_name(name: str) -> str:
    """清理符号名，解码C++修饰名"""
    # 替换一些常见的C++修饰符
    replacements = {
        '*Z': '_Z',  # C++修饰名前缀
        '*': '',     # 去掉星号
    }
    
    cleaned = name
    for old, new in replacements.items():
        cleaned = cleaned.replace(old, new)
    
    # 尝试简化C++函数名
    if cleaned.startswith('_Z'):
        # 这是一个简化的C++函数名解析
        if 'malloc' in cleaned:
            return 'malloc_related'
        elif 'printf' in cleaned:
            return 'printf_related'
        elif 'Kui' in cleaned:
            return cleaned  # 保持Kui相关的函数名
        else:
            return cleaned
    
    return cleaned

def get_symbol_section_name(symbol_type: str) -> str:
    """根据符号类型返回段名称"""
    section_mapping = {
        'T': 'Text (Global Code)',
        't': 'Text (Local Code)', 
        'D': 'Data (Global Initialized)',
        'd': 'Data (Local Initialized)',
        'B': 'BSS (Global Uninitialized)',
        'b': 'BSS (Local Uninitialized)',
        'R': 'Read-only Data (Global)',
        'r': 'Read-only Data (Local)',
        'W': 'Weak Symbol',
        'w': 'Weak Symbol (Local)',
        'A': 'Absolute Symbol',
        'V': 'Weak Object (Global)',
        'v': 'Weak Object (Local)',
        'C': 'Common Symbol',
        'S': 'Small Object (Global)',
        's': 'Small Object (Local)',
        'U': 'Undefined Symbol',
        'u': 'Undefined Symbol (Local)'
    }
    return section_mapping.get(symbol_type, f'Unknown ({symbol_type})')

def analyze_symbols_by_section(symbol_data: str) -> Dict[str, List[Dict]]:
    """按段分析符号大小"""
    lines = symbol_data.strip().split('\n')
    all_symbols = []
    
    # 解析所有符号
    for line in lines:
        if line.strip():
            parsed = parse_symbol_line(line)
            if parsed:
                addr_str, addr_value, symbol_type, symbol_name = parsed
                all_symbols.append({
                    'address': addr_value,
                    'addr_str': addr_str,
                    'type': symbol_type,
                    'section': get_symbol_section_name(symbol_type),
                    'name': symbol_name,
                    'clean_name': clean_symbol_name(symbol_name),
                    'size': 0
                })
    
    # 按段分组
    sections = {
        'code': [s for s in all_symbols if s['type'] in ['T', 't']],
        'data': [s for s in all_symbols if s['type'] in ['D', 'd']],
        'readonly': [s for s in all_symbols if s['type'] in ['R', 'r']],
        'bss': [s for s in all_symbols if s['type'] in ['B', 'b']],
        'other': [s for s in all_symbols if s['type'] not in ['T', 't', 'D', 'd', 'R', 'r', 'B', 'b']]
    }
    
    # 为每个段计算符号大小
    for section_name, symbols in sections.items():
        if symbols:
            # 按地址排序
            symbols.sort(key=lambda x: x['address'])
            
            # 计算符号大小
            for i in range(len(symbols) - 1):
                symbols[i]['size'] = symbols[i + 1]['address'] - symbols[i]['address']
            
            # 最后一个符号的大小设为0（无法确定）
            symbols[-1]['size'] = 0
    
    return sections

def format_size(size: int) -> str:
    """格式化大小显示"""
    if size == 0:
        return "Unknown"
    elif size < 1024:
        return f"{size} bytes"
    else:
        return f"{size} bytes ({size/1024:.1f} KB)"

def analyze_data_patterns(symbols: List[Dict]) -> Dict:
    """分析数据符号的模式"""
    patterns = {
        'string_literals': [],
        'constants': [],
        'arrays': [],
        'vtables': [],
        'global_vars': [],
        'other': []
    }
    
    for symbol in symbols:
        name = symbol['name'].lower()
        clean_name = symbol['clean_name'].lower()
        
        if any(keyword in name for keyword in ['str', 'string', '.LC']):
            patterns['string_literals'].append(symbol)
        elif any(keyword in name for keyword in ['const', 'CONST']):
            patterns['constants'].append(symbol)
        elif any(keyword in name for keyword in ['array', 'table', 'list']):
            patterns['arrays'].append(symbol)
        elif any(keyword in name for keyword in ['vtbl', 'vtable', '_ZTV']):
            patterns['vtables'].append(symbol)
        elif symbol['type'] in ['D', 'd'] and not any(keyword in name for keyword in ['_Z', '__']):
            patterns['global_vars'].append(symbol)
        else:
            patterns['other'].append(symbol)
    
    return patterns

def print_section_analysis(section_name: str, symbols: List[Dict], show_details: bool = True):
    """打印特定段的分析结果"""
    if not symbols:
        print(f"\n📂 {section_name.upper()} 段: 无符号")
        return
    
    symbols_with_size = [s for s in symbols if s['size'] > 0]
    total_symbols = len(symbols)
    analyzable_symbols = len(symbols_with_size)
    
    print(f"\n📂 {section_name.upper()} 段分析:")
    print("=" * 60)
    print(f"   总符号数量: {total_symbols}")
    print(f"   可分析大小: {analyzable_symbols}")
    
    if symbols_with_size:
        total_size = sum(s['size'] for s in symbols_with_size)
        avg_size = total_size // analyzable_symbols
        print(f"   总段大小: {format_size(total_size)}")
        print(f"   平均符号大小: {format_size(avg_size)}")
        
        # 显示最大的符号
        large_symbols = sorted(symbols_with_size, key=lambda x: x['size'], reverse=True)
        print(f"\n🔝 最大的10个符号:")
        print("-" * 60)
        print(f"{'地址':<12} {'大小':<16} {'类型':<20} {'符号名'}")
        print("-" * 60)
        
        for symbol in large_symbols[:10]:
            size_str = format_size(symbol['size'])
            print(f"{symbol['addr_str']:<12} {size_str:<16} {symbol['section']:<20} {symbol['clean_name'][:40]}")
        
        # 数据段特殊分析
        if section_name in ['data', 'readonly']:
            patterns = analyze_data_patterns(symbols_with_size)
            print(f"\n🎯 {section_name.upper()}段模式分析:")
            print("-" * 60)
            
            pattern_names = {
                'string_literals': '字符串字面量',
                'constants': '常量',
                'arrays': '数组/表',
                'vtables': '虚函数表',
                'global_vars': '全局变量',
                'other': '其他'
            }
            
            for pattern_key, pattern_symbols in patterns.items():
                if pattern_symbols:
                    pattern_size = sum(s['size'] for s in pattern_symbols)
                    pattern_count = len(pattern_symbols)
                    print(f"   {pattern_names[pattern_key]}: {pattern_count}个, {format_size(pattern_size)}")
                    
                    # 显示该模式下最大的几个符号
                    top_symbols = sorted(pattern_symbols, key=lambda x: x['size'], reverse=True)[:3]
                    for sym in top_symbols:
                        print(f"     • {sym['addr_str']}: {format_size(sym['size'])} - {sym['clean_name'][:50]}")
    
    if show_details:
        print(f"\n📋 完整符号列表:")
        print("-" * 60)
        print(f"{'地址':<12} {'大小':<12} {'类型':<20} {'符号名'}")
        print("-" * 60)
        
        for symbol in symbols:
            size_str = format_size(symbol['size'])
            print(f"{symbol['addr_str']:<12} {size_str:<12} {symbol['section']:<20} {symbol['clean_name'][:40]}")

def print_comprehensive_analysis(sections: Dict[str, List[Dict]]):
    """打印综合分析结果"""
    print("=" * 80)
    print("ELF符号综合分析报告")
    print("=" * 80)
    
    # 总体统计
    total_symbols = sum(len(symbols) for symbols in sections.values())
    print(f"\n📊 总体统计:")
    print(f"   符号总数: {total_symbols}")
    
    for section_name, symbols in sections.items():
        if symbols:
            symbols_with_size = [s for s in symbols if s['size'] > 0]
            total_size = sum(s['size'] for s in symbols_with_size)
            print(f"   {section_name.upper()}段: {len(symbols)}个符号, {format_size(total_size)}")
    
    # 各段详细分析
    section_order = ['code', 'data', 'readonly', 'bss', 'other']
    for section_name in section_order:
        if section_name in sections:
            print_section_analysis(section_name, sections[section_name], show_details=False)
    
    # 跨段分析
    print(f"\n🔍 跨段分析:")
    print("=" * 60)
    
    # 查找相关符号
    all_symbols_with_size = []
    for symbols in sections.values():
        all_symbols_with_size.extend([s for s in symbols if s['size'] > 0])
    
    # KuiLoong相关符号跨段分析
    kui_symbols = [s for s in all_symbols_with_size if 'Kui' in s['name']]
    if kui_symbols:
        kui_by_section = {}
        for sym in kui_symbols:
            section = sym['section']
            if section not in kui_by_section:
                kui_by_section[section] = []
            kui_by_section[section].append(sym)
        
        print(f"KuiLoong相关符号分布:")
        for section, syms in kui_by_section.items():
            total_size = sum(s['size'] for s in syms)
            print(f"   {section}: {len(syms)}个, {format_size(total_size)}")

def generate_enhanced_report(sections: Dict[str, List[Dict]], report_filename: str = "enhanced_elf_analysis_report.txt") -> bool:
    """生成增强的ELF分析报告文件"""
    try:
        with open(report_filename, 'w', encoding='utf-8') as f:
            # 捕获print_comprehensive_analysis的输出
            old_stdout = sys.stdout
            sys.stdout = f
            print_comprehensive_analysis(sections)
            
            # 添加详细的符号列表
            for section_name, symbols in sections.items():
                if symbols:
                    print(f"\n" + "="*80)
                    print(f"{section_name.upper()}段详细符号列表")
                    print("="*80)
                    print_section_analysis(section_name, symbols, show_details=True)
            
            sys.stdout = old_stdout
        
        print(f"\n✅ 详细报告已保存到: {report_filename}")
        return True
    except Exception as e:
        print(f"⚠️  保存报告时出错: {e}")
        return False

def main():
    # 读取符号数据
    if len(sys.argv) > 1:
        try:
            with open(sys.argv[1], 'r') as f:
                symbol_data = f.read()
        except FileNotFoundError:
            print(f"错误: 文件 '{sys.argv[1]}' 不存在")
            sys.exit(1)
    else:
        print("用法: python enhanced_elf_analyzer.py <符号表文件>")
        print("符号表文件可以通过以下命令生成:")
        print("  nm -S your_binary > symbols.txt")
        print("  objdump -t your_binary > symbols.txt")
        sys.exit(1)
    
    # 按段分析符号
    sections = analyze_symbols_by_section(symbol_data)
    
    # 打印综合分析结果
    print_comprehensive_analysis(sections)
    
    # 生成详细报告文件
    generate_enhanced_report(sections)

if __name__ == "__main__":
    main()