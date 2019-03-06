DIR=$(SRCPATH)/arch/cortex-m4
SRCDIRS+=$(DIR)
ALLSRC+=$(DIR)/cpu.c
ALLSRC+=$(DIR)/cpu_asm_gcc.s
ALLOBJ+=$(DIR)/cpu.o
ALLOBJ+=$(DIR)/cpu_asm_gcc.o
CFLAGS+=-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16
TARGET =kernel-cm4.lib
