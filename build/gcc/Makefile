# KLite
# Who can rewrite this file?
# jiangxiaogang<kerndev@foxmail.com>
# 2016.7.25

SRCDIR:=../../sources
INCDIR:=../../sources/
OBJDIR:=obj

CC:= C:/gcc-arm-none-eabi/bin/arm-none-eabi-gcc
AR:= C:/gcc-arm-none-eabi/bin/arm-none-eabi-ar

SRCS:=$(SRCDIR)/kernel.c
OBJS:=$(OBJDIR)/kernel.o

TARGET=unknown

#build for m0
ifeq ("$(ARCH)","cortex-m0")
SRCS+=$(SRCDIR)/gcc/cpu_core_cm0.c
SRCS+=$(SRCDIR)/gcc/cpu_core_cm0_asm.s
OBJS+=$(OBJDIR)/cpu_core_cm0.o
OBJS+=$(OBJDIR)/cpu_core_cm0_asm.o
TARGET=kernel_cm0.lib
CFLAGS:=-W -O0 -I$(INCDIR) -mthumb
endif

#build for m3
ifeq ("$(ARCH)","cortex-m3")
SRCS+=$(SRCDIR)/gcc/cpu_core_cm3.c
SRCS+=$(SRCDIR)/gcc/cpu_core_cm3_asm.s
OBJS+=$(OBJDIR)/cpu_core_cm3.o
OBJS+=$(OBJDIR)/cpu_core_cm3_asm.o
TARGET=kernel_cm3.lib
CFLAGS:=-W -O0 -I$(INCDIR) -mthumb 
endif

#build for m4
ifeq ("$(ARCH)","cortex-m4")
SRCS+=$(SRCDIR)/gcc/cpu_core_cm4.c
SRCS+=$(SRCDIR)/gcc/cpu_core_cm4_asm.s
OBJS+=$(OBJDIR)/cpu_core_cm4.o
OBJS+=$(OBJDIR)/cpu_core_cm4_asm.o
TARGET=kernel_cm4.lib
CFLAGS:=-W -O0 -I$(INCDIR) -mthumb -mfpu=fpv4-sp-d16
endif

default:$(TARGET)

unknown:
	@echo "usage: 'make ARCH=arch'"
	@echo "arch: cortex-m0,cortex-m3,cortex-m4"
	
kernel-cm0.lib: $(OBJS)
	@echo arch=$(ARCH)
	@echo target=$(TARGET)
	@$(AR) -r $(TARGET) $(OBJS)
	
kernel-cm3.lib: $(OBJS)
	@echo arch=$(ARCH)
	@echo target=$(TARGET)
	@$(AR) -r $(TARGET) $(OBJS)
	
kernel-cm4.lib: $(OBJS)
	@echo arch=$(ARCH)
	@echo target=$(TARGET)
	@$(AR) -r $(TARGET) $(OBJS)
	
$(OBJDIR)/kernel.o: $(SRCDIR)/kernel.c
	@if [ ! -d $(OBJDIR) ] ; then mkdir -v -p $(OBJDIR) ; fi
	@echo CC $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@
	@$(CC) $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@
	
$(OBJDIR)/cpu_core_cm0.o: $(SRCDIR)/gcc/cpu_core_cm0.c
	@if [ ! -d $(OBJDIR) ] ; then mkdir -v -p $(OBJDIR) ; fi
	@echo CC $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@
	@$(CC) $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@
	
$(OBJDIR)/cpu_core_cm0_asm.o: $(SRCDIR)/gcc/cpu_core_cm0_asm.s
	@if [ ! -d $(OBJDIR) ] ; then mkdir -v -p $(OBJDIR) ; fi
	@echo CC $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@
	@$(CC) $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@
	
$(OBJDIR)/cpu_core_cm3.o: $(SRCDIR)/gcc/cpu_core_cm3.c
	@if [ ! -d $(OBJDIR) ] ; then mkdir -v -p $(OBJDIR) ; fi
	@echo CC $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@
	@$(CC) $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@
	
$(OBJDIR)/cpu_core_cm3_asm.o: $(SRCDIR)/gcc/cpu_core_cm3_asm.s
	@if [ ! -d $(OBJDIR) ] ; then mkdir -v -p $(OBJDIR) ; fi
	@echo CC $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@
	@$(CC) $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@
	
$(OBJDIR)/cpu_core_cm4.o: $(SRCDIR)/gcc/cpu_core_cm4.c
	@if [ ! -d $(OBJDIR) ] ; then mkdir -v -p $(OBJDIR) ; fi
	@echo CC $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@
	@$(CC) $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@
	
$(OBJDIR)/cpu_core_cm4_asm.o: $(SRCDIR)/gcc/cpu_core_cm4_asm.s
	@if [ ! -d $(OBJDIR) ] ; then mkdir -v -p $(OBJDIR) ; fi
	@echo CC $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@
	@$(CC) $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@

clean:
	@rm -f $(OBJDIR)/* *.lib
	