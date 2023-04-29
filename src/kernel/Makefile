.DEFAULT_GOAL	:= none

OPT		?= -O3

ASM_FLAGS	:= -m32
CPP_FLAGS	:= -std=c++17	\
	-fno-exceptions		\
	-fno-stack-protector	\
	-fno-rtti		\
	-ffreestanding		\
	-nostdlib		\
	-m32 -mno-sse -g $(OPT) -Wall -Werror
LD_FLAGS	:= --no-warn-rwx-segments

ASM_FILES	:= $(wildcard *.s)
CPP_FILES	:= $(wildcard *.cpp)
OBJ_FILES	:= $(addsuffix .o,$(addprefix $(BUILD)/,$(basename $(ASM_FILES) $(CPP_FILES)))) \
	$(shell gcc -m32 --print-file-name crtbegin.o)						\
	$(shell gcc -m32 --print-file-name crtend.o)

.SECONDARY :

$(BUILD)/%.o : Makefile %.s
	@echo "assembling $*.s"
	@mkdir -p $(BUILD)
	gcc -I $(INCLUDE) -c -MD -MF $(BUILD)/$*.d -o $@ $(ASM_FLAGS) $*.s

$(BUILD)/%.o : Makefile %.cpp
	@echo "compiling $*.cpp"
	@mkdir -p $(BUILD)
	g++ -I $(INCLUDE) -c -MD -MF $(BUILD)/$*.d -o $@ $(CPP_FLAGS) $*.cpp

$(BUILD)/%.o : Makefile $(SOURCE)/%.cpp
	@echo "compiling $*.cpp"
	@mkdir -p $(BUILD)
	g++ -I $(INCLUDE) -c -MD -MF $(BUILD)/$*.d -o $@ $(CPP_FLAGS) $(SOURCE)/$*.cpp

$(BUILD)/%.kernel : Makefile $(BUILD)/script.ld $(BUILD)/%.o $(OBJ_FILES)
	@echo "linking $@"
	@mkdir -p $(BUILD)
	ld -T $(BUILD)/script.ld -N -m elf_i386 -e start -o $@ $(LD_FLAGS) $(BUILD)/$*.o $(OBJ_FILES)

$(BUILD)/%.bin : Makefile $(BUILD)/%.kernel
	@echo "producing $@"
	@mkdir -p $(BUILD)
	objcopy -O binary $(BUILD)/$*.kernel $@

$(BUILD)/%.img : Makefile $(BUILD)/%.bin
	@echo "producing $@"
	@mkdir -p $(BUILD)
	dd if=$(BUILD)/$*.bin of=$@ bs=512 conv=sync > /dev/null 2>&1
	@echo ""

$(BUILD)/script.ld : script.ld
	@mkdir -p $(BUILD)
	gcc -E -P -x c -I $(INCLUDE) -DBUILD=$(BUILD) script.ld > $@

none:

clean ::
	rm -rf $(BUILD)

-include $(BUILD)/*.d
