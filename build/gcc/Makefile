# klite
# jiangxiaogang<kerndev@foxmail.com>
# 2016.7.25

SRCDIR:=../../sources
OBJDIR:=obj

CC:= C:/Program Files/Git/gcc-arm-none-eabi/bin/arm-none-eabi-gcc
AR:= C:/Program Files/Git/gcc-arm-none-eabi/bin/arm-none-eabi-ar

SRCS:=$(SRCDIR)/kernel.c
OBJS:=$(OBJDIR)/kernel.o

TARGET=unknown

#build for m3
ifeq ("$(ARCH)","cortex-m3")
SRCS+=$(SRCDIR)/gcc/cpu_core_cm3.s
OBJS+=$(OBJDIR)/cpu_core_cm3.o
TARGET=kernel-cm3.lib
CFLAGS:=-W -O0 -mthumb
endif

#build for m4
ifeq ("$(ARCH)","cortex-m4")
SRCS+=$(SRCDIR)/gcc/cpu_core_cm4.s
OBJS+=$(OBJDIR)/cpu_core_cm4.o
TARGET=kernel-cm4.lib
CFLAGS:=-W -O0 -mthumb -mfpu=fpv4-sp-d16
endif


default:$(TARGET)

unknown:
	@echo "usage: 'make ARCH=arch'"
	@echo "arch: cortex-m3,cortex-m4"
	
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
	
$(OBJDIR)/cpu_core_cm3.o: $(SRCDIR)/gcc/cpu_core_cm3.s
	@if [ ! -d $(OBJDIR) ] ; then mkdir -v -p $(OBJDIR) ; fi
	@echo CC $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@
	@$(CC) $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@
	
$(OBJDIR)/cpu_core_cm4.o: $(SRCDIR)/gcc/cpu_core_cm4.s
	@if [ ! -d $(OBJDIR) ] ; then mkdir -v -p $(OBJDIR) ; fi
	@echo CC $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@
	@$(CC) $(CFLAGS) -mcpu=$(ARCH) -c $< -o $@

clean:
	@rm -f $(OBJDIR)/* *.lib
	