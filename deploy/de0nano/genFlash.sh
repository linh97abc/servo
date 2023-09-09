
SOF=DE0_NANO_SDRAM_Nios_Test.sof
ELF=test_de0nano.elf
mkdir -p build
sof2flash --input=${SOF} --output=build/hw.flash --epcs
elf2flash --input=${ELF} --output=build/sw.flash --epcs --after=build/hw.flash
cat build/hw.flash build/sw.flash > build/boot.flash
