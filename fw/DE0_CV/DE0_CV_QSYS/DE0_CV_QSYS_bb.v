
module DE0_CV_QSYS (
	clk_clk,
	clk_5m_clk,
	clk_sdram_clk,
	ltc2992_i2c_sda_t,
	ltc2992_i2c_scl_t,
	ltc2992_i2c_sda_i,
	ltc2992_i2c_scl_i,
	pll_locked_export,
	reset_reset_n,
	sdram_wire_addr,
	sdram_wire_ba,
	sdram_wire_cas_n,
	sdram_wire_cke,
	sdram_wire_cs_n,
	sdram_wire_dq,
	sdram_wire_dqm,
	sdram_wire_ras_n,
	sdram_wire_we_n,
	servo_controllerv1_0_conduit_end_spi_sclk,
	servo_controllerv1_0_conduit_end_spi_cs,
	servo_controllerv1_0_conduit_end_spi_miso,
	servo_controllerv1_0_conduit_end_spi_mosi,
	servo_controllerv1_0_conduit_end_hall_0,
	servo_controllerv1_0_conduit_end_hall_1,
	servo_controllerv1_0_conduit_end_hall_2,
	servo_controllerv1_0_conduit_end_hall_3,
	servo_controllerv1_0_conduit_end_phase_0,
	servo_controllerv1_0_conduit_end_phase_1,
	servo_controllerv1_0_conduit_end_phase_2,
	servo_controllerv1_0_conduit_end_phase_3,
	servo_controllerv1_0_conduit_end_nFault,
	servo_controllerv1_0_conduit_end_drv8320_en,
	spi_ad7928_external_MISO,
	spi_ad7928_external_MOSI,
	spi_ad7928_external_SCLK,
	spi_ad7928_external_SS_n,
	tmp101_i2c_sda_t,
	tmp101_i2c_scl_t,
	tmp101_i2c_sda_i,
	tmp101_i2c_scl_i,
	uart_rs485_conduit_end_rxd,
	uart_rs485_conduit_end_txd,
	uart_rs485_conduit_end_dbg_os_pulse);	

	input		clk_clk;
	output		clk_5m_clk;
	output		clk_sdram_clk;
	output		ltc2992_i2c_sda_t;
	output		ltc2992_i2c_scl_t;
	input		ltc2992_i2c_sda_i;
	input		ltc2992_i2c_scl_i;
	output		pll_locked_export;
	input		reset_reset_n;
	output	[12:0]	sdram_wire_addr;
	output	[1:0]	sdram_wire_ba;
	output		sdram_wire_cas_n;
	output		sdram_wire_cke;
	output		sdram_wire_cs_n;
	inout	[15:0]	sdram_wire_dq;
	output	[1:0]	sdram_wire_dqm;
	output		sdram_wire_ras_n;
	output		sdram_wire_we_n;
	output		servo_controllerv1_0_conduit_end_spi_sclk;
	output		servo_controllerv1_0_conduit_end_spi_cs;
	input		servo_controllerv1_0_conduit_end_spi_miso;
	output		servo_controllerv1_0_conduit_end_spi_mosi;
	input	[2:0]	servo_controllerv1_0_conduit_end_hall_0;
	input	[2:0]	servo_controllerv1_0_conduit_end_hall_1;
	input	[2:0]	servo_controllerv1_0_conduit_end_hall_2;
	input	[2:0]	servo_controllerv1_0_conduit_end_hall_3;
	output	[5:0]	servo_controllerv1_0_conduit_end_phase_0;
	output	[5:0]	servo_controllerv1_0_conduit_end_phase_1;
	output	[5:0]	servo_controllerv1_0_conduit_end_phase_2;
	output	[5:0]	servo_controllerv1_0_conduit_end_phase_3;
	input	[3:0]	servo_controllerv1_0_conduit_end_nFault;
	output	[3:0]	servo_controllerv1_0_conduit_end_drv8320_en;
	input		spi_ad7928_external_MISO;
	output		spi_ad7928_external_MOSI;
	output		spi_ad7928_external_SCLK;
	output		spi_ad7928_external_SS_n;
	output		tmp101_i2c_sda_t;
	output		tmp101_i2c_scl_t;
	input		tmp101_i2c_sda_i;
	input		tmp101_i2c_scl_i;
	input		uart_rs485_conduit_end_rxd;
	output		uart_rs485_conduit_end_txd;
	output		uart_rs485_conduit_end_dbg_os_pulse;
endmodule
