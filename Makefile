.DEFAULT_GOAL	:= tetris
SHELL		:= /bin/sh
INCLUDE		:= include
BUILD		:= build


QEMU		?= qemu-system-i386
QEMU_ACCEL	?= tcg,thread=multi
QEMU_CPU	?= max
QEMU_SMP	?= 4
QEMU_MEM	?= 128M
QEMU_FLAGS	= -accel $(QEMU_ACCEL) -cpu $(QEMU_CPU) -smp $(QEMU_SMP) -m $(QEMU_MEM)

KERNELS		:= $(notdir $(basename $(wildcard src/*.cpp)))

$(KERNELS) : % :
	@$(MAKE) -C src/kernel		\
	INCLUDE=$(realpath $(INCLUDE))	\
	BUILD=$(CURDIR)/$(BUILD)	\
	SOURCE=$(realpath src)		\
	$(CURDIR)/$(BUILD)/$*.img

qemu :
	@echo "$(QEMU)"

qemu_flags :
	@echo "$(QEMU_FLAGS)"

all : $(KERNELS)

clean ::
	rm -rf $(BUILD)

.PHONY : $(KERNELS) all clean
