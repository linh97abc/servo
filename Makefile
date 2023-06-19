
SOF=fw/DE0_CV_SDRAM_Nios_Test.sof
ELF=sw/app/app.elf

all: build/Makefile

build/Makefile: build/boot.flash build/boot.sof build/nios2-flash-override.txt
	echo  "flash:" > build/Makefile
	echo  "	nios2-configure-sof boot.sof" >> build/Makefile
	echo  "	nios2-flash-programmer --go --epcs --base=0x00001000 --override=nios2-flash-override.txt  boot.flash" >> build/Makefile

build/nios2-flash-override.txt: nios2-flash-override.txt
	mkdir -p build
	cp nios2-flash-override.txt build/nios2-flash-override.txt

build/boot.sof: ${SOF}
	mkdir -p build
	cp ${SOF} build/boot.sof

build/hw.flash: ${SOF}
	mkdir -p build
	sof2flash --input=${SOF} --output=build/hw.flash --epcs

build/sw.flash: ${ELF}
	mkdir -p build
	elf2flash --input=${ELF} --output=build/sw.flash --epcs --after=build/hw.flash

build/boot.flash: build/hw.flash build/sw.flash
	cat build/hw.flash build/sw.flash > build/boot.flash

clean:
	rm -rf build

flash: build/Makefile
	make -C build flash
