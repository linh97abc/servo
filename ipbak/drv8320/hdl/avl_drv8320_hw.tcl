
package require -exact qsys 14.1


# 
# module avl_drv8320
# 
set_module_property DESCRIPTION ""
set_module_property NAME avl_drv8320
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property AUTHOR linh
set_module_property DISPLAY_NAME avl_drv8320
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false
set_module_property REPORT_HIERARCHY false
set_module_property VALIDATION_CALLBACK validate


# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL avl_drv8320
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE false
add_fileset_file avl_drv8320.v VERILOG PATH avl_drv8320.v TOP_LEVEL_FILE

add_fileset_file servo_3phase_pwm.v VERILOG PATH servo_3phase_pwm.v
add_fileset_file servo_pwmx4.v VERILOG PATH servo_pwmx4.v
add_fileset_file servo_pwm_control.v VERILOG PATH servo_pwm_control.v
add_fileset_file servo_u2duty_wrapper.v VERILOG PATH servo_u2duty_wrapper.v


# 
# parameters
# 
add_parameter FREQ_CLK INTEGER 100000000
set_parameter_property FREQ_CLK DEFAULT_VALUE 100000000
set_parameter_property FREQ_CLK DISPLAY_NAME FREQ_CLK
set_parameter_property FREQ_CLK TYPE INTEGER
set_parameter_property FREQ_CLK UNITS None
set_parameter_property FREQ_CLK HDL_PARAMETER true
set_parameter_property FREQ_CLK SYSTEM_INFO {CLOCK_RATE clock}

add_parameter PWM_BASE_FREQ INTEGER 10000000
set_parameter_property PWM_BASE_FREQ DEFAULT_VALUE 10000000
set_parameter_property PWM_BASE_FREQ DISPLAY_NAME PWM_BASE_FREQ
set_parameter_property PWM_BASE_FREQ TYPE INTEGER
set_parameter_property PWM_BASE_FREQ UNITS None
set_parameter_property PWM_BASE_FREQ HDL_PARAMETER true


add_parameter PWM_FREQ INTEGER 1000
set_parameter_property PWM_FREQ DEFAULT_VALUE 1000
set_parameter_property PWM_FREQ DISPLAY_NAME PWM_FREQ
set_parameter_property PWM_FREQ TYPE INTEGER
set_parameter_property PWM_FREQ UNITS None
set_parameter_property PWM_FREQ HDL_PARAMETER true

add_parameter C_MODE0 INTEGER 0
set_parameter_property C_MODE0 DEFAULT_VALUE 0
set_parameter_property C_MODE0 DISPLAY_NAME C_MODE0
set_parameter_property C_MODE0 TYPE INTEGER
set_parameter_property C_MODE0 UNITS None
set_parameter_property C_MODE0 HDL_PARAMETER true
set_parameter_property C_MODE0 ALLOWED_RANGES {0:6x 1:3x 2:1x}

add_parameter C_MODE1 INTEGER 0
set_parameter_property C_MODE1 DEFAULT_VALUE 0
set_parameter_property C_MODE1 DISPLAY_NAME C_MODE1
set_parameter_property C_MODE1 TYPE INTEGER
set_parameter_property C_MODE1 UNITS None
set_parameter_property C_MODE1 HDL_PARAMETER true
set_parameter_property C_MODE1 ALLOWED_RANGES {0:6x 1:3x 2:1x}

add_parameter C_MODE2 INTEGER 0
set_parameter_property C_MODE2 DEFAULT_VALUE 0
set_parameter_property C_MODE2 DISPLAY_NAME C_MODE2
set_parameter_property C_MODE2 TYPE INTEGER
set_parameter_property C_MODE2 UNITS None
set_parameter_property C_MODE2 HDL_PARAMETER true
set_parameter_property C_MODE2 ALLOWED_RANGES {0:6x 1:3x 2:1x}

add_parameter C_MODE3 INTEGER 0
set_parameter_property C_MODE3 DEFAULT_VALUE 0
set_parameter_property C_MODE3 DISPLAY_NAME C_MODE3
set_parameter_property C_MODE3 TYPE INTEGER
set_parameter_property C_MODE3 UNITS None
set_parameter_property C_MODE3 HDL_PARAMETER true
set_parameter_property C_MODE3 ALLOWED_RANGES {0:6x 1:3x 2:1x}


# 
# connection point clock
# 
add_interface clock clock end
set_interface_property clock clockRate 0
set_interface_property clock ENABLED true
set_interface_property clock EXPORT_OF ""
set_interface_property clock PORT_NAME_MAP ""
set_interface_property clock CMSIS_SVD_VARIABLES ""
set_interface_property clock SVD_ADDRESS_GROUP ""

add_interface_port clock clk clk Input 1


# 
# connection point reset
# 
add_interface reset reset end
set_interface_property reset associatedClock clock
set_interface_property reset synchronousEdges DEASSERT
set_interface_property reset ENABLED true
set_interface_property reset EXPORT_OF ""
set_interface_property reset PORT_NAME_MAP ""
set_interface_property reset CMSIS_SVD_VARIABLES ""
set_interface_property reset SVD_ADDRESS_GROUP ""

add_interface_port reset reset_n reset_n Input 1


# 
# connection point s1
# 
add_interface s1 avalon end
set_interface_property s1 addressUnits WORDS
set_interface_property s1 associatedClock clock
set_interface_property s1 associatedReset reset
set_interface_property s1 bitsPerSymbol 8
set_interface_property s1 burstOnBurstBoundariesOnly false
set_interface_property s1 burstcountUnits WORDS
set_interface_property s1 explicitAddressSpan 0
set_interface_property s1 holdTime 0
set_interface_property s1 linewrapBursts false
set_interface_property s1 maximumPendingReadTransactions 0
set_interface_property s1 maximumPendingWriteTransactions 0
set_interface_property s1 readLatency 0
set_interface_property s1 readWaitTime 0
set_interface_property s1 setupTime 0
set_interface_property s1 timingUnits Cycles
set_interface_property s1 writeWaitTime 0
set_interface_property s1 ENABLED true
set_interface_property s1 EXPORT_OF ""
set_interface_property s1 PORT_NAME_MAP ""
set_interface_property s1 CMSIS_SVD_VARIABLES ""
set_interface_property s1 SVD_ADDRESS_GROUP ""

# add_interface_port s1 chipselect chipselect Input 1
add_interface_port s1 address address Input 5
add_interface_port s1 writedata writedata Input 32
# add_interface_port s1 begintransfer begintransfer Input 1
add_interface_port s1 write_n write_n Input 1
add_interface_port s1 read_n read_n Input 1
add_interface_port s1 readdata readdata Output 32
set_interface_assignment s1 embeddedsw.configuration.isFlash 0
set_interface_assignment s1 embeddedsw.configuration.isMemoryDevice 0
set_interface_assignment s1 embeddedsw.configuration.isNonVolatileStorage 0
set_interface_assignment s1 embeddedsw.configuration.isPrintableDevice 0


# 
# connection point conduit_end
# 
add_interface conduit_end conduit end
set_interface_property conduit_end associatedClock clock
set_interface_property conduit_end associatedReset reset
set_interface_property conduit_end ENABLED true
set_interface_property conduit_end EXPORT_OF ""
set_interface_property conduit_end PORT_NAME_MAP ""
set_interface_property conduit_end CMSIS_SVD_VARIABLES ""
set_interface_property conduit_end SVD_ADDRESS_GROUP ""

add_interface_port conduit_end hall_0 hall_0 Input 3
add_interface_port conduit_end hall_1 hall_1 Input 3
add_interface_port conduit_end hall_2 hall_2 Input 3
add_interface_port conduit_end hall_3 hall_3 Input 3

add_interface_port conduit_end phase_0 phase_0 Output 6
add_interface_port conduit_end phase_1 phase_1 Output 6
add_interface_port conduit_end phase_2 phase_2 Output 6
add_interface_port conduit_end phase_3 phase_3 Output 6

add_interface_port conduit_end nFault nFault Input 4
add_interface_port conduit_end stop stop Input 4
add_interface_port conduit_end drv8320_en drv8320_en Output 4

# 
# connection point interrupt_sender
# 
add_interface interrupt_sender interrupt end
set_interface_property interrupt_sender associatedAddressablePoint s1
set_interface_property interrupt_sender associatedClock clock
set_interface_property interrupt_sender associatedReset reset
set_interface_property interrupt_sender bridgedReceiverOffset ""
set_interface_property interrupt_sender bridgesToReceiver ""
set_interface_property interrupt_sender ENABLED true
set_interface_property interrupt_sender EXPORT_OF ""
set_interface_property interrupt_sender PORT_NAME_MAP ""
set_interface_property interrupt_sender CMSIS_SVD_VARIABLES ""
set_interface_property interrupt_sender SVD_ADDRESS_GROUP ""

add_interface_port interrupt_sender irq irq Output 1

proc validate {} {
    set clockRate [ get_parameter_value FREQ_CLK ]

    set_module_assignment embeddedsw.CMacro.FREQ $clockRate	
}
