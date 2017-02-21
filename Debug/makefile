################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := F:/ti/ccsv5/tools/compiler/msp430_4.1.5

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./c_files/wdt.obj" \
"./c_files/ucs.obj" \
"./c_files/sfr_sys.obj" \
"./c_files/rtc.obj" \
"./c_files/power.obj" \
"./c_files/pmm.obj" \
"./c_files/initialization.obj" \
"./c_files/graph_tab.obj" \
"./c_files/gpio.obj" \
"./c_files/clock.obj" \
"./c_files/LCD.obj" \
"./c_files/KeyBoardDriver.obj" \
"./c_files/DA.obj" \
"./c_files/AD.obj" \
"./main.obj" \
"../lnk_msp430f5438a.cmd" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include c_files/subdir_vars.mk
-include subdir_rules.mk
-include c_files/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: transistor_tester.out

# Tool invocations
transistor_tester.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: MSP430 Linker'
	"F:/ti/ccsv5/tools/compiler/msp430_4.1.5/bin/cl430" -vmspx --abi=coffabi -g --define=__MSP430F5438A__ --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal -z -m"transistor_tester.map" --stack_size=160 --heap_size=160 --use_hw_mpy=F5 -i"F:/ti/ccsv5/ccs_base/msp430/include" -i"F:/ti/ccsv5/tools/compiler/msp430_4.1.5/lib" -i"F:/ti/ccsv5/tools/compiler/msp430_4.1.5/include" --reread_libs --warn_sections --xml_link_info="transistor_tester_linkInfo.xml" --rom_model -o "transistor_tester.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(MSP430_EXECUTABLE_OUTPUTS__QUOTED) "transistor_tester.out"
	-$(RM) "main.pp" "c_files\AD.pp" "c_files\DA.pp" "c_files\KeyBoardDriver.pp" "c_files\LCD.pp" "c_files\clock.pp" "c_files\gpio.pp" "c_files\graph_tab.pp" "c_files\initialization.pp" "c_files\pmm.pp" "c_files\power.pp" "c_files\rtc.pp" "c_files\sfr_sys.pp" "c_files\ucs.pp" "c_files\wdt.pp" 
	-$(RM) "main.obj" "c_files\AD.obj" "c_files\DA.obj" "c_files\KeyBoardDriver.obj" "c_files\LCD.obj" "c_files\clock.obj" "c_files\gpio.obj" "c_files\graph_tab.obj" "c_files\initialization.obj" "c_files\pmm.obj" "c_files\power.obj" "c_files\rtc.obj" "c_files\sfr_sys.obj" "c_files\ucs.obj" "c_files\wdt.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
