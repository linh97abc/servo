nios2-configure-sof boot.sof
nios2-flash-programmer --epcs --base=0x800b000 --override=nios2-flash-override.txt  hw.flash
