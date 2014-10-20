################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HeartBeatController.cpp \
../src/Listener.cpp \
../src/MsgHandler.cpp \
../src/Server.cpp \
../src/main.cpp 

OBJS += \
./src/HeartBeatController.o \
./src/Listener.o \
./src/MsgHandler.o \
./src/Server.o \
./src/main.o 

CPP_DEPS += \
./src/HeartBeatController.d \
./src/Listener.d \
./src/MsgHandler.d \
./src/Server.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


