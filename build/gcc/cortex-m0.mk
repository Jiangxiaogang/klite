DIR=$(SRCPATH)/port/cortex-m0
SRCDIRS+=$(DIR)
ALLSRC+=$(DIR)/cpu.c
ALLSRC+=$(DIR)/cpu_asm_gcc.s
ALLOBJ+=$(DIR)/cpu.o
ALLOBJ+=$(DIR)/cpu_asm_gcc.o
CFLAGS+=-mcpu=cortex-m0 -mthumb
TARGET =kernel-cm0.lib
