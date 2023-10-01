SOF=fw/DE0_CV/DE0_CV_SDRAM_Nios_Test.sof
ELF=fw/DE0_CV/software/test_fw/test_fw.elf

mkdir -p build
cp scripts/nios2-flash-override.txt build/nios2-flash-override.txt
cp scripts/dow_hw.bat build/dow_hw.bat
cp scripts/dow_hw.sh build/dow_hw.sh
cp scripts/dow_sw.bat build/dow_sw.bat
cp scripts/dow_sw.sh build/dow_sw.sh
cp ${SOF} build/boot.sof
sof2flash --input=${SOF} --output=build/hw.flash --epcs
python scripts/gen_flash.py ${ELF} build/sw 0x400000