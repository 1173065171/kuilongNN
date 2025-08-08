// refer to https://github.com/hubbsvtgc/LearnRISC-V/blob/609cee346609f14d2f74aea823dd1655e3cd4300/boards/hifive1-revb/src/start.S#L55

.extern linker_stack_start; /* from linker scripts */
.global _start; /* to be visible to linker */
.section .start

_start:
	/* set stack pointer */
	lui sp, %hi(linker_stack_start);
	addi sp, sp, %lo(linker_stack_start);

	/* jump to main C code */
	jal ra, main;

/* infinite loop */
j .

