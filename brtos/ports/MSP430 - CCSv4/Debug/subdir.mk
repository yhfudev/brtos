################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../main.c \
../mcu_init.c \
../tasks.c 

CMD_SRCS += \
../lnk_msp430f2274.cmd 

OBJS += \
./main.obj \
./mcu_init.obj \
./tasks.obj 

C_DEPS += \
./main.pp \
./mcu_init.pp \
./tasks.pp 

OBJS__QTD += \
".\main.obj" \
".\mcu_init.obj" \
".\tasks.obj" 

C_DEPS__QTD += \
".\main.pp" \
".\mcu_init.pp" \
".\tasks.pp" 

C_SRCS_QUOTED += \
"../main.c" \
"../mcu_init.c" \
"../tasks.c" 


# Each subdirectory must supply rules for building sources it contributes
main.obj: ../main.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/bin/cl430" -vmsp -g --include_path="D:/CODE_COMPOSER/ccsv4/msp430/include" --include_path="D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/include" --diag_warning=225 --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

mcu_init.obj: ../mcu_init.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/bin/cl430" -vmsp -g --include_path="D:/CODE_COMPOSER/ccsv4/msp430/include" --include_path="D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/include" --diag_warning=225 --preproc_with_compile --preproc_dependency="mcu_init.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

tasks.obj: ../tasks.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/bin/cl430" -vmsp -g --include_path="D:/CODE_COMPOSER/ccsv4/msp430/include" --include_path="D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/include" --diag_warning=225 --preproc_with_compile --preproc_dependency="tasks.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


