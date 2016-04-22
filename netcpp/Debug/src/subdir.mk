################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Connector.cpp \
../src/ConnectorManager_Client.cpp \
../src/ConnectorPool.cpp \
../src/Net.cpp \
../src/Util.cpp 

OBJS += \
./src/Connector.o \
./src/ConnectorManager_Client.o \
./src/ConnectorPool.o \
./src/Net.o \
./src/Util.o 

CPP_DEPS += \
./src/Connector.d \
./src/ConnectorManager_Client.d \
./src/ConnectorPool.d \
./src/Net.d \
./src/Util.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


