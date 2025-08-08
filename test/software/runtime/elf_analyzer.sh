
run_model_compiler() {
    echo "--- 开始分析编译链接 ---"
    
    nm -n build/firmware.elf > build/symbols.txt
    which python
    python scripts/elf_analyzer.py build/symbols.txt enhanced_elf_analysis_report.txt

    echo "--- 处理完成 ---"
}

run_model_compiler
