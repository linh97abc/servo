all: DE0_CV_SDRAM_Nios_Test.sof

DE0_CV_QSYS.sopcinfo:
	qsys-generate --synthesis=VERILOG -bsf DE0_CV_QSYS.qsys

DE0_CV_SDRAM_Nios_Test.sof: DE0_CV_SDRAM_Nios_Test.fit.rpt
	quartus_asm --read_settings_files=off --write_settings_files=off DE0_CV_SDRAM_Nios_Test -c DE0_CV_SDRAM_Nios_Test

DE0_CV_SDRAM_Nios_Test.map.rpt: DE0_CV_QSYS.sopcinfo
	quartus_map --read_settings_files=on --write_settings_files=off DE0_CV_SDRAM_Nios_Test -c DE0_CV_SDRAM_Nios_Test

DE0_CV_SDRAM_Nios_Test.fit.rpt: DE0_CV_SDRAM_Nios_Test.map.rpt
	quartus_fit --read_settings_files=off --write_settings_files=off DE0_CV_SDRAM_Nios_Test -c DE0_CV_SDRAM_Nios_Test


clean:
	rm *.rpt DE0_CV_QSYS.sopcinfo

