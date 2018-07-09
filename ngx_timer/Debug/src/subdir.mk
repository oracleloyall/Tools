################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/test.cpp 

C_SRCS += \
../src/common.c \
../src/config.c \
../src/flog.c \
../src/ngx_event_timer.c \
../src/ngx_rbtree.c \
../src/zmalloc.c 

OBJS += \
./src/common.o \
./src/config.o \
./src/flog.o \
./src/ngx_event_timer.o \
./src/ngx_rbtree.o \
./src/test.o \
./src/zmalloc.o 

CPP_DEPS += \
./src/test.d 

C_DEPS += \
./src/common.d \
./src/config.d \
./src/flog.d \
./src/ngx_event_timer.d \
./src/ngx_rbtree.d \
./src/zmalloc.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


