################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BRTOS/drivers/drivers.c 

OBJS += \
./BRTOS/drivers/drivers.obj 

C_DEPS += \
./BRTOS/drivers/drivers.pp 

OBJS__QTD += \
".\BRTOS\drivers\drivers.obj" 

C_DEPS__QTD += \
".\BRTOS\drivers\drivers.pp" 

C_SRCS_QUOTED += \
"../BRTOS/drivers/drivers.c" 


# Each subdirectory must supply rules for building sources it contributes
BRTOS/drivers/drivers.obj: ../BRTOS/drivers/drivers.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/bin/cl430" -vmsp -g --include_path="D:/CODE_COMPOSER/ccsv4/msp430/include" --include_path="D:/CODE_COMPOSER/ccsv4/tools/compiler/msp430/include" --diag_warning=225 --preproc_with_compile --preproc_dependency="BRTOS/drivers/drivers.pp" --obj_directory="BRTOS/drivers" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


