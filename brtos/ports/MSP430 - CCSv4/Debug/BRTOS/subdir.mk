################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BRTOS/BRTOS.c \
../BRTOS/OS_RTC.c \
../BRTOS/debug_stack.c \
../BRTOS/event.c \
../BRTOS/mbox.c \
../BRTOS/mutex.c \
../BRTOS/queue.c \
../BRTOS/semaphore.c 

OBJS += \
./BRTOS/BRTOS.obj \
./BRTOS/OS_RTC.obj \
./BRTOS/debug_stack.obj \
./BRTOS/event.obj \
./BRTOS/mbox.obj \
./BRTOS/mutex.obj \
./BRTOS/queue.obj \
./BRTOS/semaphore.obj 

C_DEPS += \
./BRTOS/BRTOS.pp \
./BRTOS/OS_RTC.pp \
./BRTOS/debug_stack.pp \
./BRTOS/event.pp \
./BRTOS/mbox.pp \
./BRTOS/mutex.pp \
./BRTOS/queue.pp \
./BRTOS/semaphore.pp 

OBJS__QTD += \
".\BRTOS\BRTOS.obj" \
".\BRTOS\OS_RTC.obj" \
".\BRTOS\debug_stack.obj" \
".\BRTOS\event.obj" \
".\BRTOS\mbox.obj" \
".\BRTOS\mutex.obj" \
".\BRTOS\queue.obj" \
".\BRTOS\semaphore.obj" 

C_DEPS__QTD += \
".\BRTOS\BRTOS.pp" \
".\BRTOS\OS_RTC.pp" \
".\BRTOS\debug_stack.pp" \
".\BRTOS\event.pp" \
".\BRTOS\mbox.pp" \
".\BRTOS\mutex.pp" \
".\BRTOS\queue.pp" \
".\BRTOS\semaphore.pp" 

C_SRCS_QUOTED += \
"../BRTOS/BRTOS.c" \
"../BRTOS/OS_RTC.c" \
"../BRTOS/debug_stack.c" \
"../BRTOS/event.c" \
"../BRTOS/mbox.c" \
"../BRTOS/mutex.c" \
"../BRTOS/queue.c" \
"../BRTOS/semaphore.c" 


# Each subdirectory must supply rules for building sources it contributes
BRTOS/BRTOS.obj: ../BRTOS/BRTOS.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/bin/cl430" -vmsp -g --include_path="D:/CODE_COMPOSER/ccsv4/msp430/include" --include_path="D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/include" --diag_warning=225 --preproc_with_compile --preproc_dependency="BRTOS/BRTOS.pp" --obj_directory="BRTOS" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

BRTOS/OS_RTC.obj: ../BRTOS/OS_RTC.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/bin/cl430" -vmsp -g --include_path="D:/CODE_COMPOSER/ccsv4/msp430/include" --include_path="D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/include" --diag_warning=225 --preproc_with_compile --preproc_dependency="BRTOS/OS_RTC.pp" --obj_directory="BRTOS" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

BRTOS/debug_stack.obj: ../BRTOS/debug_stack.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/bin/cl430" -vmsp -g --include_path="D:/CODE_COMPOSER/ccsv4/msp430/include" --include_path="D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/include" --diag_warning=225 --preproc_with_compile --preproc_dependency="BRTOS/debug_stack.pp" --obj_directory="BRTOS" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

BRTOS/event.obj: ../BRTOS/event.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/bin/cl430" -vmsp -g --include_path="D:/CODE_COMPOSER/ccsv4/msp430/include" --include_path="D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/include" --diag_warning=225 --preproc_with_compile --preproc_dependency="BRTOS/event.pp" --obj_directory="BRTOS" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

BRTOS/mbox.obj: ../BRTOS/mbox.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/bin/cl430" -vmsp -g --include_path="D:/CODE_COMPOSER/ccsv4/msp430/include" --include_path="D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/include" --diag_warning=225 --preproc_with_compile --preproc_dependency="BRTOS/mbox.pp" --obj_directory="BRTOS" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

BRTOS/mutex.obj: ../BRTOS/mutex.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/bin/cl430" -vmsp -g --include_path="D:/CODE_COMPOSER/ccsv4/msp430/include" --include_path="D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/include" --diag_warning=225 --preproc_with_compile --preproc_dependency="BRTOS/mutex.pp" --obj_directory="BRTOS" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

BRTOS/queue.obj: ../BRTOS/queue.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/bin/cl430" -vmsp -g --include_path="D:/CODE_COMPOSER/ccsv4/msp430/include" --include_path="D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/include" --diag_warning=225 --preproc_with_compile --preproc_dependency="BRTOS/queue.pp" --obj_directory="BRTOS" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

BRTOS/semaphore.obj: ../BRTOS/semaphore.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/bin/cl430" -vmsp -g --include_path="D:/CODE_COMPOSER/ccsv4/msp430/include" --include_path="D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/include" --diag_warning=225 --preproc_with_compile --preproc_dependency="BRTOS/semaphore.pp" --obj_directory="BRTOS" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


