// ============================================================================
// Copyright (c) 2014 by Terasic Technologies Inc.
// ============================================================================
//
// Permission:
//
//   Terasic grants permission to use and modify this code for use
//   in synthesis for all Terasic Development Boards and Altera Development 
//   Kits made by Terasic.  Other use of this code, including the selling 
//   ,duplication, or modification of any portion is strictly prohibited.
//
// Disclaimer:
//
//   This VHDL/Verilog or C/C++ source code is intended as a design reference
//   which illustrates how these types of functions can be implemented.
//   It is the user's responsibility to verify their design for
//   consistency and functionality through the use of formal
//   verification methods.  Terasic provides no warranty regarding the use 
//   or functionality of this code.
//
// ============================================================================
//           
//  Terasic Technologies Inc
//  9F., No.176, Sec.2, Gongdao 5th Rd, East Dist, Hsinchu City, 30070. Taiwan
//  
//  
//                     web: http://www.terasic.com/  
//                     email: support@terasic.com
//
// ============================================================================
//   Ver  :| Author            :| Mod. Date :| Changes Made:
//   V1.0 :| Yue Yang          :| 08/25/2014:| Initial Revision
// ============================================================================


module DE0_CV_SDRAM_Nios_Test(
      ///////// CLOCK2 /////////
      input              CLOCK2_50,

      ///////// CLOCK3 /////////
      input              CLOCK3_50,

      ///////// CLOCK4 /////////
      inout              CLOCK4_50,

      ///////// CLOCK /////////
      input              CLOCK_50,

      ///////// DRAM /////////
      output      [12:0] DRAM_ADDR,
      output      [1:0]  DRAM_BA,
      output             DRAM_CAS_N,
      output             DRAM_CKE,
      output             DRAM_CLK,
      output             DRAM_CS_N,
      inout       [15:0] DRAM_DQ,
      output             DRAM_LDQM,
      output             DRAM_RAS_N,
      output             DRAM_UDQM,
      output             DRAM_WE_N,

      ///////// GPIO /////////
      inout       [35:0] GPIO_0,
      inout       [35:0] GPIO_1,

      ///////// HEX0 /////////
      output      [6:0]  HEX0,

      ///////// HEX1 /////////
      output      [6:0]  HEX1,

      ///////// HEX2 /////////
      output      [6:0]  HEX2,

      ///////// HEX3 /////////
      output      [6:0]  HEX3,

      ///////// HEX4 /////////
      output      [6:0]  HEX4,

      ///////// HEX5 /////////
      output      [6:0]  HEX5,

      ///////// KEY /////////
      input       [3:0]  KEY,

      ///////// LEDR /////////
      output      [9:0]  LEDR,

      ///////// PS2 /////////
      inout              PS2_CLK,
      inout              PS2_CLK2,
      inout              PS2_DAT,
      inout              PS2_DAT2,

      ///////// RESET /////////
      input              RESET_N,

      ///////// SD /////////
      output             SD_CLK,
      inout              SD_CMD,
      inout       [3:0]  SD_DATA,

      ///////// SW /////////
      input       [9:0]  SW,

      ///////// VGA /////////
      output      [3:0]  VGA_B,
      output      [3:0]  VGA_G,
      output             VGA_HS,
      output      [3:0]  VGA_R,
      output             VGA_VS
);

wire UART_DEBUG_TX;
wire UART_DEBUG_RX;

wire UART_RX_A;
wire UART_TX_A;
wire UART_PWREN;
wire UART_SUSPEND;

wire ADC_CLK, ADC_DIN, ADC_DOUT, ADC_CS;

wire SV1_HALL_A, SV1_HALL_B, SV1_HALL_C;
wire SV2_HALL_A, SV2_HALL_B, SV2_HALL_C;
wire SV3_HALL_A, SV3_HALL_B, SV3_HALL_C;
wire SV4_HALL_A, SV4_HALL_B, SV4_HALL_C;

wire [5:0] SV1_PHASE;
wire [5:0] SV2_PHASE;
wire [5:0] SV3_PHASE;
wire [5:0] SV4_PHASE;

wire [3:0] SV_NFAULT;
wire [3:0] DRV8320_EN;

wire TMP_SCL_T;
wire TMP_SCL_I;
wire TMP_SDA_T;
wire TMP_SDA_I;

wire LTC2992_SCL_T;
wire LTC2992_SCL_I;
wire LTC2992_SDA_T;
wire LTC2992_SDA_I;

assign UART_RX_A = GPIO_1[11];
assign GPIO_1[17] = UART_TX_A;
assign GPIO_1[13] = UART_PWREN;
assign GPIO_1[15] = UART_SUSPEND;

assign UART_PWREN = 1'b0;
assign UART_SUSPEND = 1'b1;

assign GPIO_0[14] = ADC_CLK;
//assign ADC_DIN = GPIO_0[17];
assign GPIO_0[17] = ADC_DIN;
assign GPIO_0[15] = ADC_CS;
//assign GPIO_0[16] = ADC_DOUT;
assign ADC_DOUT = GPIO_0[16];


assign SV1_HALL_A = GPIO_1[6];
assign SV1_HALL_B = GPIO_1[8];
assign SV1_HALL_C = GPIO_1[10];

assign SV2_HALL_A = GPIO_1[12];
assign SV2_HALL_B = GPIO_1[14];
assign SV2_HALL_C = GPIO_1[16];

assign SV3_HALL_A = GPIO_1[18];
assign SV3_HALL_B = GPIO_1[20];
assign SV3_HALL_C = GPIO_1[22];

assign SV4_HALL_A = GPIO_1[24];
assign SV4_HALL_B = GPIO_1[26];
assign SV4_HALL_C = GPIO_1[28];

assign GPIO_1[7] = DRV8320_EN[0];
assign GPIO_1[4] = SV1_PHASE[5];
assign GPIO_1[5] = SV1_PHASE[4];
assign GPIO_1[2] = SV1_PHASE[3];
assign GPIO_1[3] = SV1_PHASE[2];
assign GPIO_1[0] = SV1_PHASE[1];
assign GPIO_1[1] = SV1_PHASE[0];

assign GPIO_0[7] = DRV8320_EN[1];
assign GPIO_0[4] = SV2_PHASE[5];
assign GPIO_0[5] = SV2_PHASE[4];
assign GPIO_0[2] = SV2_PHASE[3];
assign GPIO_0[3] = SV2_PHASE[2];
assign GPIO_0[0] = SV2_PHASE[1];
assign GPIO_0[1] = SV2_PHASE[0];

assign GPIO_0[25] = DRV8320_EN[2];
assign GPIO_0[22] = SV3_PHASE[5];
assign GPIO_0[23] = SV3_PHASE[4];
assign GPIO_0[20] = SV3_PHASE[3];
assign GPIO_0[21] = SV3_PHASE[2];
assign GPIO_0[18] = SV3_PHASE[1];
assign GPIO_0[19] = SV3_PHASE[0];

assign GPIO_0[35] = DRV8320_EN[3];
assign GPIO_0[32] = SV4_PHASE[5];
assign GPIO_0[33] = SV4_PHASE[4];
assign GPIO_0[30] = SV4_PHASE[3];
assign GPIO_0[31] = SV4_PHASE[2];
assign GPIO_0[28] = SV4_PHASE[1];
assign GPIO_0[29] = SV4_PHASE[0];

assign SV_NFAULT = {GPIO_0[34], GPIO_0[24], GPIO_0[6], GPIO_1[9]};

assign GPIO_0[11] = TMP_SCL_T? 1'bz: 1'b0;
assign GPIO_0[10] = TMP_SDA_T? 1'bz: 1'b0;
assign TMP_SCL_I = GPIO_0[11];
assign TMP_SDA_I = GPIO_0[10];

assign GPIO_1[34] = LTC2992_SCL_T;
assign GPIO_1[32] = LTC2992_SDA_T;
assign LTC2992_SCL_I = GPIO_1[34];
assign LTC2992_SDA_I = GPIO_1[32];


//(* keep *) reg clock_debug = 0;
//reg[7:0] clock_debug_cnt = 0;
//always @(posedge CLOCK_50)
//begin 
//   if (clock_debug_cnt < 25) begin 
//		clock_debug_cnt <= clock_debug_cnt + 1'b1;
//	end else begin
//		clock_debug_cnt <= 0;
//		clock_debug <= ~clock_debug;
//	end
//	
//end
(* keep *) wire CLK_5M;

//=======================================================
//  REG/WIRE declarations
//=======================================================
DE0_CV_QSYS u0(
		.clk_clk(CLOCK_50),                        //                     clk.clk
		.reset_reset_n(1'b1),                  //                   reset.reset_n
		.clk_5m_clk(CLK_5M),

		//.uart_debug_conduit_end_rxd(UART_DEBUG_RX),                  //           uart_debug_conduit_end.rxd
		//.uart_debug_conduit_end_txd(UART_DEBUG_TX),                  //                                 .txd
		//.uart_debug_conduit_end_dbg_os_pulse(),
      .uart_rs485_conduit_end_rxd(UART_RX_A),                  //           uart_rs485_conduit_end.rxd
		.uart_rs485_conduit_end_txd(UART_TX_A),                   //                                 .txd
      .uart_rs485_conduit_end_dbg_os_pulse(),

		//.spi_ad7928_external_MISO                    (ADC_DIN),                    //              spi_ad7928_external.MISO
		//.spi_ad7928_external_MOSI                    (ADC_DOUT),                    //                                 .MOSI
		//.spi_ad7928_external_SCLK                    (ADC_CLK),                    //                                 .SCLK
		//.spi_ad7928_external_SS_n                    (ADC_CS),                     // 

            //
      //.servo_controllerv1_0_conduit_end_spi_sclk(), // servo_controllerv1_0_conduit_end.spi_sclk
		//.servo_controllerv1_0_conduit_end_spi_cs(),   //                                 .spi_cs
		//.servo_controllerv1_0_conduit_end_spi_miso(1'b1), //                                 .spi_miso
		//.servo_controllerv1_0_conduit_end_spi_mosi(), //                                 .spi_mosi
      .servo_controllerv1_0_conduit_end_spi_sclk(ADC_CLK), // servo_controllerv1_0_conduit_end.spi_sclk
		.servo_controllerv1_0_conduit_end_spi_cs(ADC_CS),   //                                 .spi_cs
		.servo_controllerv1_0_conduit_end_spi_miso(ADC_DOUT), //                                 .spi_miso
		.servo_controllerv1_0_conduit_end_spi_mosi(ADC_DIN), //                                 .spi_mosi
		.servo_controllerv1_0_conduit_end_hall_0({SV1_HALL_A, SV1_HALL_B, SV1_HALL_C}),   //                                 .hall_0
		.servo_controllerv1_0_conduit_end_hall_1({SV2_HALL_A, SV2_HALL_B, SV2_HALL_C}),   //                                 .hall_1
		.servo_controllerv1_0_conduit_end_hall_2({SV3_HALL_A, SV3_HALL_B, SV3_HALL_C}),   //                                 .hall_2
		.servo_controllerv1_0_conduit_end_hall_3({SV4_HALL_A, SV4_HALL_B, SV4_HALL_C}),   //                                 .hall_3
		.servo_controllerv1_0_conduit_end_phase_0(SV1_PHASE),  //                                 .phase_0
		.servo_controllerv1_0_conduit_end_phase_1(SV2_PHASE),  //                                 .phase_1
		.servo_controllerv1_0_conduit_end_phase_2(SV3_PHASE),  //                                 .phase_2
		.servo_controllerv1_0_conduit_end_phase_3(SV4_PHASE),   //                                 .phase_3
            .servo_controllerv1_0_conduit_end_nFault(SV_NFAULT),     //                                 .nFault
		.servo_controllerv1_0_conduit_end_drv8320_en(DRV8320_EN), //                                 .drv8320_en

		.tmp101_i2c_sda_t                            (TMP_SDA_T),                            //                       tmp101_i2c.sda_t
		.tmp101_i2c_scl_t                            (TMP_SCL_T),                            //                                 .scl_t
		.tmp101_i2c_sda_i                            (TMP_SDA_I),                            //                                 .sda_i
		.tmp101_i2c_scl_i                            (TMP_SCL_I),                             //  
            
            .ltc2992_i2c_sda_t                           (LTC2992_SDA_T),                           //                      ltc2992_i2c.sda_t
		.ltc2992_i2c_scl_t                           (LTC2992_SCL_T),                           //                                 .scl_t
		.ltc2992_i2c_sda_i                           (LTC2992_SDA_I),                           //                                 .sda_i
		.ltc2992_i2c_scl_i                           (LTC2992_SCL_I),                            //   
            
            // .avl_drv8320_0_conduit_end_hall_0(GPIO_0[8:6]),     // avl_drv8320_0_conduit_end.hall_0
		// .avl_drv8320_0_conduit_end_hall_1(GPIO_0[11:9]),     //                          .hall_1
		// .avl_drv8320_0_conduit_end_hall_2(GPIO_0[14:12]),     //                          .hall_2
		// .avl_drv8320_0_conduit_end_hall_3(GPIO_0[17:15]),     //                          .hall_3
		// .avl_drv8320_0_conduit_end_phase_0(GPIO_1[5:0]),    //                          .phase_0
		// .avl_drv8320_0_conduit_end_phase_1(GPIO_1[11:6]),    //                          .phase_1
		// .avl_drv8320_0_conduit_end_phase_2(GPIO_1[17:12]),    //                          .phase_2
		// .avl_drv8320_0_conduit_end_phase_3(GPIO_1[23:18]),    //                          .phase_3
		// .avl_drv8320_0_conduit_end_nFault(GPIO_0[23:18]),     //                          .nFault
		// .avl_drv8320_0_conduit_end_stop(),       //                          .stop
		// .avl_drv8320_0_conduit_end_drv8320_en(GPIO_0[27:24]),


		//SDRAM
		.clk_sdram_clk(DRAM_CLK),                  //               clk_sdram.clk
	   .sdram_wire_addr(DRAM_ADDR),                //              sdram_wire.addr
		.sdram_wire_ba(DRAM_BA),                  //                        .ba
		.sdram_wire_cas_n(DRAM_CAS_N),               //                        .cas_n
		.sdram_wire_cke(DRAM_CKE),                 //                        .cke
		.sdram_wire_cs_n(DRAM_CS_N),                //                        .cs_n
		.sdram_wire_dq(DRAM_DQ),                  //                        .dq
		.sdram_wire_dqm({DRAM_UDQM,DRAM_LDQM}),                 //                        .dqm
		.sdram_wire_ras_n(DRAM_RAS_N),               //                        .ras_n
		.sdram_wire_we_n(DRAM_WE_N)                 //                        .we_n
	);

endmodule
