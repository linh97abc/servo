
package require -exact qsys 14.1


# 
# module avl_fifo_uart
# 
set_module_property DESCRIPTION ""
set_module_property NAME avl_fifo_uart
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property AUTHOR linh
set_module_property DISPLAY_NAME avl_fifo_uart
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false
set_module_property REPORT_HIERARCHY false


# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL avl_fifo_uart
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE false
add_fileset_file avl_fifo_uart.v VERILOG PATH avl_fifo_uart.v TOP_LEVEL_FILE
add_fileset_file axis_uart.v VERILOG PATH axis_uart.v
add_fileset_file uart_ram.v VERILOG PATH uart_ram.v
add_fileset_file uart_rx.v VERILOG PATH uart_rx.v
add_fileset_file uart_tx.v VERILOG PATH uart_tx.v
add_fileset_file uart_sfifo.v VERILOG PATH uart_sfifo.v


# 
# parameters
# 
add_parameter FREQ_CLK INTEGER 1000000
set_parameter_property FREQ_CLK DEFAULT_VALUE 1000000
set_parameter_property FREQ_CLK DISPLAY_NAME FREQ_CLK
set_parameter_property FREQ_CLK TYPE INTEGER
set_parameter_property FREQ_CLK UNITS None
set_parameter_property FREQ_CLK HDL_PARAMETER true
set_parameter_property FREQ_CLK SYSTEM_INFO {CLOCK_RATE clock}

add_parameter BAUDRATE INTEGER 921600
set_parameter_property BAUDRATE DEFAULT_VALUE 921600
set_parameter_property BAUDRATE DISPLAY_NAME BAUDRATE
set_parameter_property BAUDRATE TYPE INTEGER
set_parameter_property BAUDRATE UNITS None
set_parameter_property BAUDRATE HDL_PARAMETER true

add_parameter DATA_BIT INTEGER 8
set_parameter_property DATA_BIT DEFAULT_VALUE 8
set_parameter_property DATA_BIT DISPLAY_NAME DATA_BIT
set_parameter_property DATA_BIT TYPE INTEGER
set_parameter_property DATA_BIT UNITS None
set_parameter_property DATA_BIT HDL_PARAMETER true

add_parameter PARITY_BIT INTEGER 0
set_parameter_property PARITY_BIT DEFAULT_VALUE 0
set_parameter_property PARITY_BIT DISPLAY_NAME PARITY_BIT
set_parameter_property PARITY_BIT TYPE INTEGER
set_parameter_property PARITY_BIT UNITS None
set_parameter_property PARITY_BIT HDL_PARAMETER true

add_parameter STOP_BIT INTEGER 0
set_parameter_property STOP_BIT DEFAULT_VALUE 0
set_parameter_property STOP_BIT DISPLAY_NAME STOP_BIT
set_parameter_property STOP_BIT TYPE INTEGER
set_parameter_property STOP_BIT UNITS None
set_parameter_property STOP_BIT HDL_PARAMETER true

add_parameter RX_FIFO_DEPTH INTEGER 8
set_parameter_property RX_FIFO_DEPTH DEFAULT_VALUE 8
set_parameter_property RX_FIFO_DEPTH DISPLAY_NAME RX_FIFO_DEPTH
set_parameter_property RX_FIFO_DEPTH TYPE INTEGER
set_parameter_property RX_FIFO_DEPTH UNITS None
set_parameter_property RX_FIFO_DEPTH HDL_PARAMETER true

add_parameter TX_FIFO_DEPTH INTEGER 8
set_parameter_property TX_FIFO_DEPTH DEFAULT_VALUE 8
set_parameter_property TX_FIFO_DEPTH DISPLAY_NAME TX_FIFO_DEPTH
set_parameter_property TX_FIFO_DEPTH TYPE INTEGER
set_parameter_property TX_FIFO_DEPTH UNITS None
set_parameter_property TX_FIFO_DEPTH HDL_PARAMETER true

# 
# display items
# 
add_display_item "Input Clock Frequency (Hz):" FREQ_CLK PARAMETER ""
add_display_item "Baudrate:" BAUDRATE PARAMETER ""
add_display_item "Databits (5,6,7,8):" DATA_BIT PARAMETER ""
add_display_item "Parity Bit (00,01 <=> NONE, 11 <=> Odd, 10 <=> Even):" PARITY_BIT PARAMETER ""
add_display_item "Stop Bit (0 <=> 1 bit, 1 <=> 1,5 bit, 2 <=> 2 bit):" STOP_BIT PARAMETER ""
add_display_item "RX_FIFO_DEPTH:" RX_FIFO_DEPTH PARAMETER ""
add_display_item "TX_FIFO_DEPTH:" TX_FIFO_DEPTH PARAMETER ""


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
# connection point avalon_slave_0
# 
add_interface avalon_slave_0 avalon end
set_interface_property avalon_slave_0 addressUnits WORDS
set_interface_property avalon_slave_0 associatedClock clock
set_interface_property avalon_slave_0 associatedReset reset
set_interface_property avalon_slave_0 bitsPerSymbol 8
set_interface_property avalon_slave_0 burstOnBurstBoundariesOnly false
set_interface_property avalon_slave_0 burstcountUnits WORDS
set_interface_property avalon_slave_0 explicitAddressSpan 0
set_interface_property avalon_slave_0 holdTime 0
set_interface_property avalon_slave_0 linewrapBursts false
set_interface_property avalon_slave_0 maximumPendingReadTransactions 0
set_interface_property avalon_slave_0 maximumPendingWriteTransactions 0
set_interface_property avalon_slave_0 readLatency 0
set_interface_property avalon_slave_0 readWaitTime 1
set_interface_property avalon_slave_0 setupTime 0
set_interface_property avalon_slave_0 timingUnits Cycles
set_interface_property avalon_slave_0 writeWaitTime 0
set_interface_property avalon_slave_0 ENABLED true
set_interface_property avalon_slave_0 EXPORT_OF ""
set_interface_property avalon_slave_0 PORT_NAME_MAP ""
set_interface_property avalon_slave_0 CMSIS_SVD_VARIABLES ""
set_interface_property avalon_slave_0 SVD_ADDRESS_GROUP ""

# add_interface_port avalon_slave_0 chipselect chipselect Input 1
add_interface_port avalon_slave_0 address address Input 5
add_interface_port avalon_slave_0 writedata writedata Input 32
# add_interface_port avalon_slave_0 begintransfer begintransfer Input 1
add_interface_port avalon_slave_0 write_n write_n Input 1
add_interface_port avalon_slave_0 read_n read_n Input 1
add_interface_port avalon_slave_0 readdata readdata Output 32
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isFlash 0
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isMemoryDevice 0
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isNonVolatileStorage 0
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isPrintableDevice 0


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

add_interface_port conduit_end rxd rxd Input 1
add_interface_port conduit_end txd txd Output 1

# 
# connection point interrupt_sender
# 
add_interface interrupt_sender interrupt end
set_interface_property interrupt_sender associatedAddressablePoint avalon_slave_0
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
