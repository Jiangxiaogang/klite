DIR=$(SRCPATH)/arch/cortex-m3
SRCDIRS+=$(DIR)
ALLSRC+=$(DIR)/cpu.c
ALLSRC+=$(DIR)/cpu_asm_gcc.s
ALLOBJ+=$(DIR)/cpu.o
ALLOBJ+=$(DIR)/cpu_asm_gcc.o
CFLAGS+=-mcpu=cortex-m3 -mthumb
TARGET =kernel-cm3.lib
