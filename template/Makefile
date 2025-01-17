//////// Modify the following part
TARGET:= /* <name> */
//////// Modify the above part
include ../ch32v003fun/ch32v003fun.mk
ifeq ($(OS),Windows_NT)
	RM = del
else
	RM = rm -f
endif
clean:
	$(RM) $(TARGET).elf $(TARGET).hex $(TARGET).bin $(TARGET).map $(TARGET).lst \
		$(TARGET).o $(TARGET).d $(TARGET).srec $(TARGET).sym $(TARGET).lss
	$(MAKE) -f Makefile-emulator clean

emulator:
	$(MAKE) -f Makefile-emulator
	$(MAKE) -f Makefile-emulator clean
