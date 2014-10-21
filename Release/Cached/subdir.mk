################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Cached/Cached.cpp \
../Cached/CachedMgr.cpp 

OBJS += \
./Cached/Cached.o \
./Cached/CachedMgr.o 

CPP_DEPS += \
./Cached/Cached.d \
./Cached/CachedMgr.d 


# Each subdirectory must supply rules for building sources it contributes
Cached/%.o: ../Cached/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -std=c++0x -g -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


