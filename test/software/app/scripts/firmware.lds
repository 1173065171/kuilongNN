/*
refer to https://github.com/hubbsvtgc/LearnRISC-V/blob/609cee346609f14d2f74aea823dd1655e3cd4300/tools/hifive1-revb-rom.lscript#L15
*/

/* Comments here */
OUTPUT_ARCH("riscv")

/* Entry point */
ENTRY(_start)

/* Memory Layout */
MEMORY
{
	rom (x!raiw) : ORIGIN = 0x00000000, LENGTH = 256k
	ram (rw!xai) : ORIGIN = 0x40000000, LENGTH = 2560k
}

SECTIONS
{
	. = 0x00000000;
	__text_start__ = .;
	.text :
	{
		KEEP(*(.start))
		*(.text*)
	} > rom
	. = ALIGN(4);

	__rodata_start__ = .;
	.rodata :
	{
		*(.rodata .rodata.* .gnu.linkonce.r.*)    
	} > ram
	. = ALIGN(4);
	__rodata_end__ = .;

	.eh_frame_hdr   : 
	{ *(.eh_frame_hdr) *(.eh_frame_entry .eh_frame_entry.*) } > ram

	.eh_frame       : ONLY_IF_RO 
	{ KEEP (*(.eh_frame)) *(.eh_frame.*) } > ram

	.eh_frame       : ONLY_IF_RW 
	{ KEEP (*(.eh_frame)) *(.eh_frame.*) } > ram

	__data_start__ = .;
	.data :
	{
		*(.data .data.* .gnu.linkonce.d.*)    
	} > ram
	. = ALIGN(4);
	__data_end__ = .;

	__bss_start__ = .;
	.sbss           :
	{
		*(.dynsbss)
		*(.sbss .sbss.* .gnu.linkonce.sb.*)
		*(.scommon)
	} > ram
	.bss            :
	{
	*(.dynbss)
	*(.bss .bss.* .gnu.linkonce.b.*)
	*(COMMON)
	. = ALIGN(. != 0 ? 32 / 8 : 1);
	} > ram
	. = ALIGN(32 / 8);
	. = SEGMENT_START("ldata-segment", .);
	. = ALIGN(32 / 8);
	__bss_end__ = .;

	.comment	 : 
	{ *(.comment)} > ram

	/* Set Stack after code & data */
	linker_stack_start = 0x402FFFF0;

	/DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) *(.riscv.attributes)}
}
