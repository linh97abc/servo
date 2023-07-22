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
wire UART_RS485_TX;
wire UART_RS485_RX;

assign UART_DEBUG_RX = UART_DEBUG_TX;
assign UART_RS485_RX = UART_RS485_TX;

//=======================================================
//  REG/WIRE declarations
//=======================================================
DE0_CV_QSYS u0(
		.clk_clk(CLOCK_50),                        //                     clk.clk
		.reset_reset_n(1'b1),                  //                   reset.reset_n

		.uart_debug_conduit_end_rxd(UART_DEBUG_RX),                  //           uart_debug_conduit_end.rxd
		.uart_debug_conduit_end_txd(UART_DEBUG_TX),                  //                                 .txd
		.uart_rs485_conduit_end_rxd(UART_RS485_RX),                  //           uart_rs485_conduit_end.rxd
		.uart_rs485_conduit_end_txd(UART_RS485_TX),                   //                                 .txd

            //
            .servo_controllerv1_0_conduit_end_spi_sclk(GPIO_0[2]), // servo_controllerv1_0_conduit_end.spi_sclk
		.servo_controllerv1_0_conduit_end_spi_cs(GPIO_0[3]),   //                                 .spi_cs
		.servo_controllerv1_0_conduit_end_spi_miso(GPIO_0[4]), //                                 .spi_miso
		.servo_controllerv1_0_conduit_end_spi_mosi(GPIO_0[5]), //                                 .spi_mosi
		.servo_controllerv1_0_conduit_end_hall_0(GPIO_0[8:6]),   //                                 .hall_0
		.servo_controllerv1_0_conduit_end_hall_1(GPIO_0[11:9]),   //                                 .hall_1
		.servo_controllerv1_0_conduit_end_hall_2(GPIO_0[14:12]),   //                                 .hall_2
		.servo_controllerv1_0_conduit_end_hall_3(GPIO_0[17:15]),   //                                 .hall_3
		.servo_controllerv1_0_conduit_end_phase_0(GPIO_1[5:0]),  //                                 .phase_0
		.servo_controllerv1_0_conduit_end_phase_1(GPIO_1[11:6]),  //                                 .phase_1
		.servo_controllerv1_0_conduit_end_phase_2(GPIO_1[17:12]),  //                                 .phase_2
		.servo_controllerv1_0_conduit_end_phase_3(GPIO_1[23:18]),   //                                 .phase_3
            .servo_controllerv1_0_conduit_end_nFault(GPIO_0[23:18]),     //                                 .nFault
		.servo_controllerv1_0_conduit_end_drv8320_en(GPIO_0[27:24]), //                                 .drv8320_en

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
