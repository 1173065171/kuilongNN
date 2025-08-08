
run_model_compiler() {
    echo "--- 开始前端编译器处理 ---"
    
    nm -n build/firmware.elf > build/symbols.txt
    which python
    python scripts/elf_analyzer.py build/symbols.txt

    echo "--- 处理完成 ---"
}

run_model_compiler
