################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Common/Common.cpp \
../Common/Lock.cpp \
../Common/threadpool.cpp \
../Common/tinystr.cpp \
../Common/tinyxml.cpp \
../Common/tinyxmlerror.cpp \
../Common/tinyxmlparser.cpp \
../Common/xmltest.cpp 

OBJS += \
./Common/Common.o \
./Common/Lock.o \
./Common/threadpool.o \
./Common/tinystr.o \
./Common/tinyxml.o \
./Common/tinyxmlerror.o \
./Common/tinyxmlparser.o \
./Common/xmltest.o 

CPP_DEPS += \
./Common/Common.d \
./Common/Lock.d \
./Common/threadpool.d \
./Common/tinystr.d \
./Common/tinyxml.d \
./Common/tinyxmlerror.d \
./Common/tinyxmlparser.d \
./Common/xmltest.d 


# Each subdirectory must supply rules for building sources it contributes
Common/%.o: ../Common/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -std=c++0x -g -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


