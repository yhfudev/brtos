# makfile configuration
SHELL		    = cmd.exe
NAME            = brtos
MCU             = msp430x2274

#switch the compiler (for the internal make rules)
########################################################################################

CC       = msp430-gcc
LD       = msp430-ld
AR       = msp430-ar
AS       = msp430-gcc
GASP     = msp430-gasp
NM       = msp430-nm
OBJCOPY  = msp430-objcopy
RANLIB   = msp430-ranlib
STRIP    = msp430-strip
SIZE     = msp430-size
READELF  = msp430-readelf
CP       = cp -p
RM       = DEL
#RM      = rm -f
MV       = mv

########################################################################################


CFLAGS  = -mmcu=${MCU} -O2 -Wall -g -I./ -I./BRTOS/ -I./BRTOS/HAL/ -I./BRTOS/includes/

#CFLAGS   = -mmcu=$(MCU) -g -O2 -Wall -Wcast-align -Wcast-qual -Wimplicit \
#	   -Wmissing-declarations -Wmissing-prototypes -Wnested-externs \
#	   -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow \
#	   -Wstrict-prototypes -Wswitch -Wunused
ASFLAGS  = -mmcu=$(MCU) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS  = -mmcu=$(MCU) -Wl,-Map=$*.map,--cref


# Setup paths to source code
SOURCE_PATH = ./BRTOS
PORT_PATH = ./BRTOS/HAL
DRIVERS_PATH = ./BRTOS/drivers

#
# Source files that can be built to THUMB mode.
#
SRC = \
main.c \
mcu_init.c \
tasks.c \
$(DRIVERS_PATH)/drivers.c \
$(SOURCE_PATH)/BRTOS.c \
$(PORT_PATH)/HAL.c \
$(SOURCE_PATH)/event.c \
$(SOURCE_PATH)/queue.c \
$(SOURCE_PATH)/mbox.c \
$(SOURCE_PATH)/semaphore.c\
$(SOURCE_PATH)/mutex.c \
$(SOURCE_PATH)/OS_RTC.c \

#
# Define all object files.
#
OBJECTS = $(SRC:.c=.o)


$(OBJECTS) : %.o : %.c makefile
	$(CC) -c $(CFLAGS) $< -o $@

#.PHONY: all FORCE clean download download-jtag download-bsl dist

#all should be the first target. it's built when make is run without args
all: $(NAME).elf ${NAME}.a43 ${NAME}.lst

#$(NAME).elf: $(OBJECTS) makefile
#	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

#additional rules for files
${NAME}.elf: ${OBJECTS}
	${CC} -mmcu=${MCU} -o $@ ${OBJECTS}

${NAME}.a43: ${NAME}.elf
	msp430-objcopy -O ihex $^ $@

${NAME}.lst: ${NAME}.elf
	msp430-objdump -dSt $^ >$@

#configure the next line if you want to use the serial download
#download: download-jtag
#download: download-bsl
download-jtag: all
	msp430-jtag -e ${NAME}.elf

download-bsl: all
	msp430-bsl -e ${NAME}.elf

clean:
	-$(RM) *.o ${NAME}.elf ${NAME}.a43 ${NAME}.lst 
	-$(RM) BRTOS\*.o
	-$(RM) BRTOS\HAL\*.o
	-$(RM) BRTOS\drivers\*.o	


#backup archive
dist:
	tar czf dist.tgz *.c *.h *.txt makefile

#dummy target as dependecy if something has to be build everytime
FORCE:



#automatic collection of dependencies in the source files.
#it's only updated the first time, after that it must be done maually
#with "make depend"
#the dependecies are included from a separate file:
-include dependencies.in
#target to update the file, it's removed first
depend: rmdepend dependencies.in
#remove the file
rmdepend:
	-$(RM) dependencies.in
#build the file that contains the dependencies. no deps in this rule.
#if there were deps it would be rebuilt every chnage, which is unneded:
dependencies.in:
	$(CC) -MM ${CFLAGS} $(patsubst %.o,%.c,$(OBJECTS)) >$@

