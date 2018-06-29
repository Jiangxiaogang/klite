# KLite makefile
# jiangxiaogang<kerndev@foxmail.com>
# 2017.6.14
CC := /opt/arm/bin/arm-none-eabi-gcc
AR := /opt/arm/bin/arm-none-eabi-ar
CFLAGS :=-W -O0  -fshort-wchar

SRCPATH:= ../../sources/kernel
SRCDIRS:= $(SRCPATH)
INCDIRS:= -I$(SRCPATH)
ALLSRC := $(wildcard $(SRCDIRS)/*.c) 
ALLOBJ := $(ALLSRC:%.c=%.o) 
TARGET := kernel.lib

ifdef ARCH
include $(ARCH).mk
target: $(ALLOBJ)
	@$(AR) -r $(TARGET) $(ALLOBJ)
	@echo make done.
endif

help:
	@echo "usage:"
	@echo "  make ARCH=target"
	@echo "target:"
	@echo "  cortex-m0"
	@echo "  cortex-m3"
	@echo "  cortex-m4"
	
clean:
	@rm -rf $(ALLOBJ) $(TARGET)
	@echo clean done.
	
%.o: %.s
	@echo compiling $< 
	@$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@ 

%.o: %.c  
	@echo compiling $<
	@$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@ 
	