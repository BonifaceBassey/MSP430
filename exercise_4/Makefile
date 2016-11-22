# Makefile for msp430 Examples

TARGET  = firmware
MCU     = msp430g2553
DEVICE  = mspdebug rf2500

# Source files
SOURCES = main.c   

# Include are located in the Include directory
INCLUDES = -IInclude

# Add or subtract whatever MSPGCC flags you want. There are plenty more
#######################################################################################
#CFLAGS   = -mmcu=$(MCU) -g -Os -Wall -Wunused $(INCLUDES)
CFLAGS   = -mmcu=$(MCU) -g -Os -Wunused $(INCLUDES)
ASFLAGS  = -mmcu=$(MCU) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS  = -mmcu=$(MCU) -Wl,-Map=$(TARGET).map
########################################################################################
CC       = msp430-gcc
LD       = msp430-ld
AR       = msp430-ar
AS       = msp430-gcc
OBJCOPY  = msp430-objcopy
SIZE     = msp430-size
RM       = rm -f
MKDIR	 = mkdir -p

########################################################################################

# the file which will include dependencies
DEPEND = $(SOURCES:.c=.d)

# all the object files
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET).elf $(TARGET).hex 
$(TARGET).elf: $(OBJECTS)
	echo "Linking $@"
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@
	echo "\n>>>> Size of Firmware <<<<"
	$(SIZE) $(TARGET).elf
	echo
%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@
%.o: %.c
	echo "Compiling $<"
	$(CC) -c $(CFLAGS) -o $@ $<

# rule for making assembler source listing, to see the code
%.lst: %.c
	$(CC) -c $(ASFLAGS) -Wa,-anlhd $< > $@

# dependencies file...includes also considered, since some of these are our own (otherwise use -MM instead of -M)
%.d: %.c
	echo "Generating dependencies $@ from $<"
	$(CC) -M ${CFLAGS} $< >$@

.SILENT:
.PHONY:	clean
clean:
	-$(RM) $(OBJECTS) $(TARGET).* $(SOURCES:.c=.lst) $(DEPEND)
.PHONY:flash
flash: all
	echo prog $(TARGET).elf | $(DEVICE)
.PHONY:erase
erase:
	echo erase | $(DEVICE)
.PHONY: reset
reset:
	echo reset | $(DEVICE)

# include the dependencies unless we're going to clean, then forget about them.
ifneq ($(MAKECMDGOALS), clean)
-include $(DEPEND)
endif

