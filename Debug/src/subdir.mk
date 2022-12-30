################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/image_segmentation.c 

SRC_OBJS += \
./src/image_segmentation.doj 

C_DEPS += \
./src/image_segmentation.d 


# Each subdirectory must supply rules for building sources it contributes
src/image_segmentation.doj: ../src/image_segmentation.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="image_segmentation" -proc ADSP-21489 -flags-compiler --no_wrap_diagnostics -si-revision 0.2 -g -DCORE0 -D_DEBUG @includes-3a4f3662fc4c2399e763c0546c13be53.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "src/image_segmentation.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


