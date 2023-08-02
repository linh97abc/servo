// ============================================================================
// Copyright (c) 2011 by Terasic Technologies Inc. 
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
//                     Terasic Technologies Inc
//                     356 Fu-Shin E. Rd Sec. 1. JhuBei City,
//                     HsinChu County, Taiwan
//                     302
//
//                     web: http://www.terasic.com/
//                     email: support@terasic.com
//
// ============================================================================
// Major Functions/Design Description:
//
//   Please refer to DE0_NANO_SDRAM_Nios_Test_User_manual.pdf in DE0_NANO_SDRAM_Nios_Test system CD.
//
// ============================================================================
// Revision History:
// ============================================================================
//   Ver.: |Author:   |Mod. Date:    |Changes Made:
//   V1.0  |EricChen  |02/01/2011    |
// ============================================================================

//=======================================================
//  This code is generated by Terasic System Builder
//=======================================================

module DE0_NANO_SDRAM_Nios_Test (

    //////////// CLOCK //////////
    CLOCK_50,

    //////////// LED //////////
    LED,

    //////////// KEY //////////
    KEY,

    //////////// SW //////////
    SW,

    //////////// SDRAM //////////
    DRAM_ADDR,
    DRAM_BA,
    DRAM_CAS_N,
    DRAM_CKE,
    DRAM_CLK,
    DRAM_CS_N,
    DRAM_DQ,
    DRAM_DQM,
    DRAM_RAS_N,
    DRAM_WE_N,

    //////////// EPCS //////////
    EPCS_ASDO,
    EPCS_DATA0,
    EPCS_DCLK,
    EPCS_NCSO,

    //////////// Accelerometer and EEPROM //////////
    G_SENSOR_CS_N,
    G_SENSOR_INT,
    I2C_SCLK,
    I2C_SDAT,

    //////////// ADC //////////
    ADC_CS_N,
    ADC_SADDR,
    ADC_SCLK,
    ADC_SDAT,

    //////////// 2x13 GPIO Header //////////
    GPIO_2,
    GPIO_2_IN,

    //////////// GPIO_0, GPIO_0 connect to GPIO Default //////////
    GPIO_0_D,
    GPIO_0_IN,

    //////////// GPIO_0, GPIO_1 connect to GPIO Default //////////
    GPIO_1_D,
    GPIO_1_IN

);

  //=======================================================
  //  PARAMETER declarations
  //=======================================================


  //=======================================================
  //  PORT declarations
  //=======================================================

  //////////// CLOCK //////////
  input CLOCK_50;

  //////////// LED //////////
  output [7:0] LED;

  //////////// KEY //////////
  input [1:0] KEY;

  //////////// SW //////////
  input [3:0] SW;

  //////////// SDRAM //////////
  output [12:0] DRAM_ADDR;
  output [1:0] DRAM_BA;
  output DRAM_CAS_N;
  output DRAM_CKE;
  output DRAM_CLK;
  output DRAM_CS_N;
  inout [15:0] DRAM_DQ;
  output [1:0] DRAM_DQM;
  output DRAM_RAS_N;
  output DRAM_WE_N;

  //////////// EPCS //////////
  output EPCS_ASDO;
  input EPCS_DATA0;
  output EPCS_DCLK;
  output EPCS_NCSO;

  //////////// Accelerometer and EEPROM //////////
  output G_SENSOR_CS_N;
  input G_SENSOR_INT;
  output I2C_SCLK;
  inout I2C_SDAT;

  //////////// ADC //////////
  output ADC_CS_N;
  output ADC_SADDR;
  output ADC_SCLK;
  input ADC_SDAT;

  //////////// 2x13 GPIO Header //////////
  inout [12:0] GPIO_2;
  input [2:0] GPIO_2_IN;

  //////////// GPIO_0, GPIO_0 connect to GPIO Default //////////
  inout [33:0] GPIO_0_D;
  input [1:0] GPIO_0_IN;

  //////////// GPIO_0, GPIO_1 connect to GPIO Default //////////
  inout [33:0] GPIO_1_D;
  input [1:0] GPIO_1_IN;


  wire UART_LOOPBACK_TX;
wire UART_LOOPBACK_RX;
wire UART_DEBUG_PULSE;

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


assign SV1_HALL_A = GPIO_0_D[0];
assign SV1_HALL_B = GPIO_0_D[2];
assign SV1_HALL_C = GPIO_0_D[4];

assign SV2_HALL_A = GPIO_0_D[6];
assign SV2_HALL_B = GPIO_0_D[8];
assign SV2_HALL_C = GPIO_0_D[10];

assign SV3_HALL_A = GPIO_0_D[12];
assign SV3_HALL_B = GPIO_0_D[14];
assign SV3_HALL_C = GPIO_0_D[16];

assign SV4_HALL_A = GPIO_0_D[18];
assign SV4_HALL_B = GPIO_0_D[20];
assign SV4_HALL_C = GPIO_0_D[22];

assign GPIO_1_D[0] = DRV8320_EN[0];
assign GPIO_1_D[2] = SV1_PHASE[5];
assign GPIO_1_D[4] = SV1_PHASE[4];
assign GPIO_1_D[6] = SV1_PHASE[3];
assign GPIO_1_D[8] = SV1_PHASE[2];
assign GPIO_1_D[10] = SV1_PHASE[1];
assign GPIO_1_D[12] = SV1_PHASE[0];

assign GPIO_1_D[16] = DRV8320_EN[1];
assign GPIO_1_D[18] = SV2_PHASE[5];
assign GPIO_1_D[20] = SV2_PHASE[4];
assign GPIO_1_D[22] = SV2_PHASE[3];
assign GPIO_1_D[24] = SV2_PHASE[2];
assign GPIO_1_D[26] = SV2_PHASE[1];
assign GPIO_1_D[28] = SV2_PHASE[0];

assign GPIO_1_D[1] = DRV8320_EN[2];
assign GPIO_1_D[3] = SV3_PHASE[5];
assign GPIO_1_D[5] = SV3_PHASE[4];
assign GPIO_1_D[7] = SV3_PHASE[3];
assign GPIO_1_D[9] = SV3_PHASE[2];
assign GPIO_1_D[11] = SV3_PHASE[1];
assign GPIO_1_D[13] = SV3_PHASE[0];

assign GPIO_1_D[17] = DRV8320_EN[3];
assign GPIO_1_D[19] = SV4_PHASE[5];
assign GPIO_1_D[21] = SV4_PHASE[4];
assign GPIO_1_D[23] = SV4_PHASE[3];
assign GPIO_1_D[25] = SV4_PHASE[2];
assign GPIO_1_D[27] = SV4_PHASE[1];
assign GPIO_1_D[29] = SV4_PHASE[0];

assign SV_NFAULT = {GPIO_1_D[31], GPIO_1_D[15], GPIO_1_D[30], GPIO_1_D[14]};

  DE0_NANO_QSYS u0 (
      .clk_clk                     (CLOCK_50),    //                      clk.clk
      .reset_reset_n               (1'b1),        //                    reset.reset_n
      .altpll_sdram_clk            (DRAM_CLK),    //             altpll_sdram.clk
      .altpll_areset_conduit_export(1'b0),        //    altpll_areset_conduit.export
      .sdram_wire_addr             (DRAM_ADDR),   //               sdram_wire.addr
      .sdram_wire_ba               (DRAM_BA),     //                         .ba
      .sdram_wire_cas_n            (DRAM_CAS_N),  //                         .cas_n
      .sdram_wire_cke              (DRAM_CKE),    //                         .cke
      .sdram_wire_cs_n             (DRAM_CS_N),   //                         .cs_n
      .sdram_wire_dq               (DRAM_DQ),     //                         .dq
      .sdram_wire_dqm              (DRAM_DQM),    //                         .dqm
      .sdram_wire_ras_n            (DRAM_RAS_N),  //                         .ras_n
      .sdram_wire_we_n             (DRAM_WE_N),   //                         .we_n

      .epcs_flash_controller_0_external_dclk(EPCS_DCLK),  //   epcs_flash_controller_0_external.dclk
      .epcs_flash_controller_0_external_sce(EPCS_NCSO),  //                                   .sce
      .epcs_flash_controller_0_external_sdo(EPCS_ASDO),  //                                   .sdo
      .epcs_flash_controller_0_external_data0(EPCS_DATA0),  //  


      // .avl_fifo_uart_0_conduit_end_rxd          (UART_LOOPBACK_TX),          //      avl_fifo_uart_0_conduit_end.rxd
		  // .avl_fifo_uart_0_conduit_end_txd          (UART_LOOPBACK_RX),          //                                 .txd
		  // .avl_fifo_uart_0_conduit_end_dbg_os_pulse (UART_DEBUG_PULSE),  //                                 .dbg_os_pulse
		  
      // .avl_fifo_uart_1_conduit_end_rxd          (UART_LOOPBACK_RX),          //      avl_fifo_uart_0_conduit_end.rxd
		  // .avl_fifo_uart_1_conduit_end_txd          (UART_LOOPBACK_TX),          //                                 .txd
		  // .avl_fifo_uart_1_conduit_end_dbg_os_pulse ()  //                                 .dbg_os_pulse

    .servo4x_conduit_end_spi_sclk           (),           //              servo4x_conduit_end.spi_sclk
		.servo4x_conduit_end_spi_cs             (),             //                                 .spi_cs
		.servo4x_conduit_end_spi_miso           (),           //                                 .spi_miso
		.servo4x_conduit_end_spi_mosi           (),           //                                 .spi_mosi
		.servo4x_conduit_end_hall_0             ({SV1_HALL_A, SV1_HALL_B, SV1_HALL_C}),             //                                 .hall_0
		.servo4x_conduit_end_hall_1             ({SV2_HALL_A, SV2_HALL_B, SV2_HALL_C}),             //                                 .hall_1
		.servo4x_conduit_end_hall_2             ({SV3_HALL_A, SV3_HALL_B, SV3_HALL_C}),             //                                 .hall_2
		.servo4x_conduit_end_hall_3             ({SV4_HALL_A, SV4_HALL_B, SV4_HALL_C}),             //                                 .hall_3
		.servo4x_conduit_end_phase_0            (SV1_PHASE),            //                                 .phase_0
		.servo4x_conduit_end_phase_1            (SV2_PHASE),            //                                 .phase_1
		.servo4x_conduit_end_phase_2            (SV3_PHASE),            //                                 .phase_2
		.servo4x_conduit_end_phase_3            (SV4_PHASE),            //                                 .phase_3
		.servo4x_conduit_end_nFault             (SV_NFAULT),             //                                 .nFault
		.servo4x_conduit_end_drv8320_en         (DRV8320_EN)
  );


endmodule
