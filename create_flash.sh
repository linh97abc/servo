SOF=fw/DE0_CV/DE0_CV_SDRAM_Nios_Test.sof
ELF=fw/DE0_CV/software/test_fw/test_fw.elf

mkdir -p build
cp scripts/nios2-flash-override.txt build/nios2-flash-override.txt
cp scripts/dow_fw.bat build/dow_fw.bat
cp scripts/dow_fw.sh build/dow_fw.sh
cp scripts/dow_sw.bat build/dow_sw.bat
cp scripts/dow_sw.sh build/dow_sw.sh
cp ${SOF} build/boot.sof

python scripts/gen_flash.py build --elf=${ELF} --sof=${SOF}  --sw-offset=0x400000