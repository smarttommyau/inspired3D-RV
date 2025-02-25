PREFIX?="D:\programs\MounRiver\MounRiver_Studio2\resources\app\resources\win32\components\WCH\Toolchain\RISC-V Embedded GCC12\bin\riscv-wch-elf"

TARGET_MCU?=CH32V003
TARGET_EXT?=c

CH32V003FUN?=../ch32v003fun

WRITE_SECTION?=flash
SYSTEM_C?=$(CH32V003FUN)/*.c
CFLAGS?=-g -Os -flto -ffunction-sections -fdata-sections -fmessage-length=0 -msmall-data-limit=8

# Fedora places newlib in a different location
ifneq ($(wildcard /etc/fedora-release),)
	NEWLIB?=/usr/arm-none-eabi/include
else
	NEWLIB?=/usr/include/newlib
endif


MINICHLINK?=$(CH32V003FUN)/../minichlink
LDFLAGS+=-Wl,--print-memory-usage

ifeq ($(TARGET_MCU),CH32V003)
	CFLAGS_ARCH+=-march=rv32ec -mabi=ilp32e -DCH32V003=1
	GENERATED_LD_FILE?=$(CH32V003FUN)/generated_ch32v003.ld
	TARGET_MCU_LD:=0
	LINKER_SCRIPT?=$(GENERATED_LD_FILE)
	LDFLAGS+=-L$(CH32V003FUN)/../misc -lgcc
else
	MCU_PACKAGE?=1

	ifeq ($(findstring CH32V10,$(TARGET_MCU)),CH32V10) # CH32V103
		TARGET_MCU_PACKAGE?=CH32V103R8T6
		CFLAGS_ARCH+=	-march=rv32imac \
			-mabi=ilp32 \
			-DCH32V10x=1

		# MCU Flash/RAM split
		ifeq ($(findstring R8, $(TARGET_MCU_PACKAGE)), R8)
			MCU_PACKAGE:=1
		else ifeq ($(findstring C8, $(TARGET_MCU_PACKAGE)), C8)
			MCU_PACKAGE:=1
		else ifeq ($(findstring C6, $(TARGET_MCU_PACKAGE)), C6)
			MCU_PACKAGE:=2
		endif

		TARGET_MCU_LD:=1
	else ifeq ($(findstring CH32X03,$(TARGET_MCU)),CH32X03) # CH32X033, X035
		TARGET_MCU_PACKAGE?=CH32X035F8U6
		CFLAGS_ARCH+=-march=rv32imac \
			-mabi=ilp32 \
			-DCH32X03x=1

		# MCU Flash/RAM split
		ifeq ($(findstring F8, $(TARGET_MCU_PACKAGE)), F8)
			MCU_PACKAGE:=1
		else ifeq ($(findstring R8, $(TARGET_MCU_PACKAGE)), R8)
			MCU_PACKAGE:=1
		else ifeq ($(findstring K8, $(TARGET_MCU_PACKAGE)), K8)
			MCU_PACKAGE:=1
		else ifeq ($(findstring C8, $(TARGET_MCU_PACKAGE)), C8)
			MCU_PACKAGE:=1
		else ifeq ($(findstring G8, $(TARGET_MCU_PACKAGE)), G8)
			MCU_PACKAGE:=1
		else ifeq ($(findstring G6, $(TARGET_MCU_PACKAGE)), G6)
			MCU_PACKAGE:=1
		else ifeq ($(findstring F7, $(TARGET_MCU_PACKAGE)), F7)
			MCU_PACKAGE:=1
		endif

		TARGET_MCU_LD:=4		
	else ifeq ($(findstring CH32V20,$(TARGET_MCU)),CH32V20) # CH32V203
		TARGET_MCU_PACKAGE?=CH32V203F8P6
		CFLAGS_ARCH+=	-march=rv32imac \
			-mabi=ilp32 \
			-DCH32V20x=1

		# MCU Flash/RAM split


		# Package
		ifeq ($(findstring 203RB, $(TARGET_MCU_PACKAGE)), 203RB)
			CFLAGS+=-DCH32V20x_D8
		else ifeq ($(findstring 208, $(TARGET_MCU_PACKAGE)), 208)
			CFLAGS+=-DCH32V20x_D8W
		else ifeq ($(findstring F8, $(TARGET_MCU_PACKAGE)), F8)
			MCU_PACKAGE:=1
		else ifeq ($(findstring G8, $(TARGET_MCU_PACKAGE)), G8)
			MCU_PACKAGE:=1
		else ifeq ($(findstring K8, $(TARGET_MCU_PACKAGE)), K8)
			MCU_PACKAGE:=1
		else ifeq ($(findstring C8, $(TARGET_MCU_PACKAGE)), C8)
			MCU_PACKAGE:=1
		else ifeq ($(findstring F6, $(TARGET_MCU_PACKAGE)), F6)
			MCU_PACKAGE:=2
		else ifeq ($(findstring G6, $(TARGET_MCU_PACKAGE)), G6)
			MCU_PACKAGE:=2
		else ifeq ($(findstring K6, $(TARGET_MCU_PACKAGE)), K6)
			MCU_PACKAGE:=2
		else ifeq ($(findstring C6, $(TARGET_MCU_PACKAGE)), C6)
			MCU_PACKAGE:=2
		else ifeq ($(findstring RB, $(TARGET_MCU_PACKAGE)), RB)
			MCU_PACKAGE:=3
		else ifeq ($(findstring GB, $(TARGET_MCU_PACKAGE)), GB)
			MCU_PACKAGE:=3
		else ifeq ($(findstring CB, $(TARGET_MCU_PACKAGE)), CB)
			MCU_PACKAGE:=3
		else ifeq ($(findstring WB, $(TARGET_MCU_PACKAGE)), WB)
			MCU_PACKAGE:=3
		else
			CFLAGS+=-DCH32V20x_D6
		endif

		TARGET_MCU_LD:=2
	else ifeq ($(findstring CH32V30,$(TARGET_MCU)),CH32V30) #CH32V307
		TARGET_MCU_PACKAGE?=CH32V307VCT6
		MCU_PACKAGE?=1

		CFLAGS_ARCH+= -march=rv32imafc \
			-mabi=ilp32f \
			-DCH32V30x=1

		# MCU Flash/RAM split
		ifeq ($(findstring RC, $(TARGET_MCU_PACKAGE)), RC)
			MCU_PACKAGE:=1
		else ifeq ($(findstring VC, $(TARGET_MCU_PACKAGE)), VC)
			MCU_PACKAGE:=1
		else ifeq ($(findstring WC, $(TARGET_MCU_PACKAGE)), WC)
			MCU_PACKAGE:=1
		else ifeq ($(findstring CB, $(TARGET_MCU_PACKAGE)), CB)
			MCU_PACKAGE:=2
		else ifeq ($(findstring FB, $(TARGET_MCU_PACKAGE)), FB)
			MCU_PACKAGE:=2
		else ifeq ($(findstring RB, $(TARGET_MCU_PACKAGE)), RB)
			MCU_PACKAGE:=2
		endif

		# Package
		ifeq ($(findstring 303, $(TARGET_MCU_PACKAGE)), 303)
			CFLAGS+=-DCH32V30x_D8
		else
			CFLAGS+=-DCH32V30x_D8C
		endif

		TARGET_MCU_LD:=3
	else
		$(error Unknown MCU $(TARGET_MCU))
	endif

	LDFLAGS+=-lgcc
	GENERATED_LD_FILE:=$(CH32V003FUN)/generated_$(TARGET_MCU_PACKAGE).ld
	LINKER_SCRIPT:=$(GENERATED_LD_FILE)
endif

CFLAGS+= \
	$(CFLAGS_ARCH) -static-libgcc \
	-I$(NEWLIB) \
	-I$(CH32V003FUN)/../extralibs \
	-I$(CH32V003FUN) \
	-nostdlib \
	-I. -I$(CH32V003FUN)/../data -Wall $(EXTRA_CFLAGS) \
	-Wshadow -Wswitch -Wfloat-equal

LDFLAGS+=-T $(LINKER_SCRIPT) -Wl,--gc-sections
FILES_TO_COMPILE:=$(SYSTEM_C) $(TARGET).$(TARGET_EXT) $(ADDITIONAL_C_FILES) 

$(TARGET).bin : $(TARGET).elf
	$(PREFIX)-objdump -S $^ > $(TARGET).lst
	$(PREFIX)-objdump -t $^ > $(TARGET).map
	$(PREFIX)-objcopy -O binary $< $(TARGET).bin
	$(PREFIX)-objcopy -O ihex $< $(TARGET).hex
	$(MAKE) elf-size

cppcheck:
	cppcheck --enable=all -I$(CH32V003FUN) -I$(CH32V003FUN)/../data \
	-I. $(TARGET).$(TARGET_EXT) $(ADDITIONAL_C_FILES) \
	--force --quiet --suppress=unusedFunction --suppress=missingIncludeSystem --check-level=exhaustive

ifeq ($(OS),Windows_NT)
closechlink :
	-taskkill /F /IM minichlink.exe /T
else
closechlink :
	-killall minichlink
endif

terminal : monitor

monitor :
	$(MINICHLINK)/minichlink -T

unbrick :
	$(MINICHLINK)/minichlink -u

gdbserver : 
	-$(MINICHLINK)/minichlink -baG

clangd :
	make clean
	bear -- make build
	@echo "CompileFlags:" > .clangd
	@echo "  Remove: [-march=*, -mabi=*]" >> .clangd

clangd_clean :
	rm -f compile_commands.json .clangd
	rm -rf .cache

FLASH_COMMAND?=$(MINICHLINK)/minichlink -w $< $(WRITE_SECTION) -b

.PHONY : $(GENERATED_LD_FILE)
$(GENERATED_LD_FILE) :
	$(PREFIX)-gcc -E -P -x c -DTARGET_MCU=$(TARGET_MCU) -DMCU_PACKAGE=$(MCU_PACKAGE) -DTARGET_MCU_LD=$(TARGET_MCU_LD) $(CH32V003FUN)/ch32v003fun.ld > $(GENERATED_LD_FILE)

$(TARGET).elf : $(FILES_TO_COMPILE) $(LINKER_SCRIPT) $(EXTRA_ELF_DEPENDENCIES)
	$(PREFIX)-gcc -o $@ $(FILES_TO_COMPILE) $(CFLAGS) $(LDFLAGS)

# Rule for independently building ch32v003fun.o indirectly, instead of recompiling it from source every time.
# Not used in the default 003fun toolchain, but used in more sophisticated toolchains.
ch32v003fun.o : $(SYSTEM_C)
	$(PREFIX)-gcc -c -o $@ $(SYSTEM_C) $(CFLAGS)

cv_flash : $(TARGET).bin
	make -C $(MINICHLINK) all
	$(FLASH_COMMAND)

cv_clean :
	rm -rf $(TARGET).elf $(TARGET).bin $(TARGET).hex $(TARGET).lst $(TARGET).map $(TARGET).hex $(GENERATED_LD_FILE) || true

build : $(TARGET).bin

OBJSIZE?= $(PREFIX)-size
elf-size:
	@echo "------------------"
	@echo "FLASH: $(shell $(OBJSIZE) -d $(TARGET).elf | awk '/[0-9]/ {print $$1 + $$2}') bytes"
	@echo "SRAM:  $(shell $(OBJSIZE) -d $(TARGET).elf | awk '/[0-9]/ {print $$2 + $$3}') bytes"
	@echo "------------------"
