################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BRTOS/HAL/HAL.c 

OBJS += \
./BRTOS/HAL/HAL.obj 

C_DEPS += \
./BRTOS/HAL/HAL.pp 

OBJS__QTD += \
".\BRTOS\HAL\HAL.obj" 

C_DEPS__QTD += \
".\BRTOS\HAL\HAL.pp" 

C_SRCS_QUOTED += \
"../BRTOS/HAL/HAL.c" 


# Each subdirectory must supply rules for building sources it contributes
BRTOS/HAL/HAL.obj: ../BRTOS/HAL/HAL.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/bin/cl430" -vmsp -g --include_path="D:/CODE_COMPOSER/ccsv4/msp430/include" --include_path="D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/include" --diag_warning=225 --preproc_with_compile --preproc_dependency="BRTOS/HAL/HAL.pp" --obj_directory="BRTOS/HAL" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


