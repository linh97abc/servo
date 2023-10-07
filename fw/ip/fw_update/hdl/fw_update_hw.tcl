
package require -exact qsys 14.1
source $env(QUARTUS_ROOTDIR)/../ip/altera/sopc_builder_ip/common/embedded_ip_hwtcl_common.tcl


# 
# module fw_update
# 
set_module_property DESCRIPTION ""
set_module_property NAME fw_update
set_module_property DISPLAY_NAME fw_update
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
set_fileset_property QUARTUS_SYNTH TOP_LEVEL fw_update
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE false
add_fileset_file fw_update.v VERILOG PATH fw_update.v TOP_LEVEL_FILE

# 
# parameters
# 

add_parameter PRODUCT_ID INTEGER 0
set_parameter_property PRODUCT_ID DEFAULT_VALUE 0
set_parameter_property PRODUCT_ID DISPLAY_NAME PRODUCT_ID
set_parameter_property PRODUCT_ID TYPE INTEGER
set_parameter_property PRODUCT_ID UNITS None
set_parameter_property PRODUCT_ID HDL_PARAMETER true

add_parameter VERSION_MAJOR INTEGER 1
set_parameter_property VERSION_MAJOR DEFAULT_VALUE 1
set_parameter_property VERSION_MAJOR DISPLAY_NAME VERSION_MAJOR
set_parameter_property VERSION_MAJOR TYPE INTEGER
set_parameter_property VERSION_MAJOR UNITS None
set_parameter_property VERSION_MAJOR HDL_PARAMETER true


add_parameter VERSION_MINOR INTEGER 0
set_parameter_property VERSION_MINOR DEFAULT_VALUE 0
set_parameter_property VERSION_MINOR DISPLAY_NAME VERSION_MINOR
set_parameter_property VERSION_MINOR TYPE INTEGER
set_parameter_property VERSION_MINOR UNITS None
set_parameter_property VERSION_MINOR HDL_PARAMETER true

add_parameter LOCK_PWD INTEGER 1234532
set_parameter_property LOCK_PWD DEFAULT_VALUE 1234532
set_parameter_property LOCK_PWD DISPLAY_NAME LOCK_PWD
set_parameter_property LOCK_PWD TYPE INTEGER
set_parameter_property LOCK_PWD UNITS None
set_parameter_property LOCK_PWD HDL_PARAMETER true

# display group

add_display_item {} {Fixed Parameter} GROUP

add_display_item {Fixed Parameter} PRODUCT_ID PARAMETER
add_display_item {Fixed Parameter} VERSION_MAJOR PARAMETER
add_display_item {Fixed Parameter} VERSION_MINOR PARAMETER
add_display_item {Fixed Parameter} LOCK_PWD PARAMETER


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
add_interface_port s1 address address Input 3
add_interface_port s1 writedata writedata Input 32
# add_interface_port s1 begintransfer begintransfer Input 1
add_interface_port s1 write_n write_n Input 1
add_interface_port s1 read_n read_n Input 1
add_interface_port s1 readdata readdata Output 32
set_interface_assignment s1 embeddedsw.configuration.isFlash 0
set_interface_assignment s1 embeddedsw.configuration.isMemoryDevice 0
set_interface_assignment s1 embeddedsw.configuration.isNonVolatileStorage 0



proc validate {} {
    set productID [ get_parameter_value PRODUCT_ID ]
    set verMajor [ get_parameter_value VERSION_MAJOR ]
    set verMinor [ get_parameter_value VERSION_MINOR ]


    set_module_assignment embeddedsw.CMacro.PRODUCT_ID $productID	
    set_module_assignment embeddedsw.CMacro.VERSION_MAJOR $verMajor	
    set_module_assignment embeddedsw.CMacro.VERSION_MINOR $verMinor	

}
