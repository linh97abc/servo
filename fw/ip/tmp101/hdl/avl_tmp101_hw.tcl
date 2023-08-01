
package require -exact qsys 14.1
source $env(QUARTUS_ROOTDIR)/../ip/altera/sopc_builder_ip/common/embedded_ip_hwtcl_common.tcl


# 
# module avl_tmp101
# 
set_module_property DESCRIPTION ""
set_module_property NAME avl_tmp101
set_module_property DISPLAY_NAME avl_tmp101
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property AUTHOR linh
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
set_fileset_property QUARTUS_SYNTH TOP_LEVEL avl_tmp101
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE false
add_fileset_file avl_tmp101.v VERILOG PATH avl_tmp101.v TOP_LEVEL_FILE
add_fileset_file i2c_master.vhd VHDL PATH i2c_master.vhd
add_fileset_file tmp101.v VERILOG PATH tmp101.v

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

add_parameter BUS_CLK INTEGER 100000
set_parameter_property BUS_CLK DEFAULT_VALUE 100000
set_parameter_property BUS_CLK DISPLAY_NAME BUS_CLK
set_parameter_property BUS_CLK TYPE INTEGER
set_parameter_property BUS_CLK UNITS None
set_parameter_property BUS_CLK HDL_PARAMETER true

add_parameter UPDATE_FREQ INTEGER 2
set_parameter_property UPDATE_FREQ DEFAULT_VALUE 2
# set_parameter_property UPDATE_FREQ AFFECTS_GENERATION {1}
set_parameter_property UPDATE_FREQ DISPLAY_NAME UPDATE_FREQ
set_parameter_property UPDATE_FREQ TYPE INTEGER
set_parameter_property UPDATE_FREQ UNITS None
set_parameter_property UPDATE_FREQ HDL_PARAMETER true


add_parameter I2C_ADDR INTEGER 0
set_parameter_property I2C_ADDR DEFAULT_VALUE 0
set_parameter_property I2C_ADDR DISPLAY_NAME I2C_ADDR
set_parameter_property I2C_ADDR TYPE INTEGER
set_parameter_property I2C_ADDR UNITS None
set_parameter_property I2C_ADDR HDL_PARAMETER true


# 
# display items
# 
# add_display_item "Input Clock Frequency (Hz):" FREQ_CLK PARAMETER ""
# add_display_item "Baudrate:" BAUDRATE PARAMETER ""
# add_display_item "Databits (5,6,7,8):" DATA_BIT PARAMETER ""
# add_display_item "Parity Bit (0 <=> NONE, 2 <=> Even, 3 <=> Odd):" PARITY_BIT PARAMETER ""
# add_display_item "Stop Bit (0 <=> 1 bit, 1 <=> 1,5 bit, 2 <=> 2 bit):" STOP_BIT PARAMETER ""
# add_display_item "RX_FIFO_DEPTH:" RX_FIFO_DEPTH PARAMETER ""
# add_display_item "TX_FIFO_DEPTH:" TX_FIFO_DEPTH PARAMETER ""

# display group
add_display_item {} {Input Clock Frequency (Hz)} GROUP
add_display_item {} {Fixed Parameter} GROUP

# group parameter
add_display_item {Input Clock Frequency (Hz)} FREQ_CLK PARAMETER


add_display_item {Fixed Parameter} BUS_CLK PARAMETER
add_display_item {Fixed Parameter} UPDATE_FREQ PARAMETER
add_display_item {Fixed Parameter} I2C_ADDR PARAMETER


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
add_interface_port s1 address address Input 4
add_interface_port s1 writedata writedata Input 32
# add_interface_port s1 begintransfer begintransfer Input 1
add_interface_port s1 write_n write_n Input 1
add_interface_port s1 read_n read_n Input 1
add_interface_port s1 readdata readdata Output 32
set_interface_assignment s1 embeddedsw.configuration.isFlash 0
set_interface_assignment s1 embeddedsw.configuration.isMemoryDevice 0
set_interface_assignment s1 embeddedsw.configuration.isNonVolatileStorage 0


# 
# connection point i2c
# 
add_interface i2c conduit end
set_interface_property i2c associatedClock clock
set_interface_property i2c associatedReset reset
set_interface_property i2c ENABLED true
set_interface_property i2c EXPORT_OF ""
set_interface_property i2c PORT_NAME_MAP ""
set_interface_property i2c CMSIS_SVD_VARIABLES ""
set_interface_property i2c SVD_ADDRESS_GROUP ""

add_interface_port i2c sda_t sda_t Output 1
add_interface_port i2c scl_t scl_t Output 1
add_interface_port i2c sda_i sda_i Input 1
add_interface_port i2c scl_i scl_i Input 1



proc validate {} {
    set clockRate [ get_parameter_value FREQ_CLK ]
    set busClk [ get_parameter_value BUS_CLK ]
    set updateFreq [ get_parameter_value UPDATE_FREQ ]
    set i2cAddr [ get_parameter_value I2C_ADDR ]


    set_module_assignment embeddedsw.CMacro.FREQ $clockRate	
    set_module_assignment embeddedsw.CMacro.BUS_CLK $busClk	
    set_module_assignment embeddedsw.CMacro.UPDATE_FREQ $updateFreq	
    set_module_assignment embeddedsw.CMacro.I2C_ADDR $i2cAddr

}
