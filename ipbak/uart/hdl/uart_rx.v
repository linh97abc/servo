`timescale 1ns / 1ps
//==========================================================================
// Module name : uart_rx.v
// Change Log  :
//      - V1.0: Write Basic UART RX Controller (5/2018)
//
// Description: 
// 1. Overview: This file contains the UART Receiver Controller
//
// 2.Features:
//  - Data Bit: 5,6,7,8 (configurable)
//  - Stop Bit: 1, 1.5, 2 (configurable)
// 
// 3.RX Sampling procedure:
//
//   Step 1. Wait until the RXD becomes 0 (the beginning of the start bit)
//           and then start counter s_reg. 
//   Step 2. When the incoming signal reaches the middle point of the start bit
//   Step    (s_reg = baud_divisor/2), clear the counter to 0 and restart. 
//   Step 3. When the s_reg = baud_divisor, the incoming signal progresses for one bit 
//   Step    and will reaches the middle of the next data bit.
//   Step    Retrieve its value, shift it into a register, and restart the counter. 
//   Step 4. Repeat step 3 N-1 more times to retrieve the remaining data bits. 
//   Step 5. If the optional parity bit is used, repeat step 3 one time to obtain 
//   Step    the parity bit. 
//   Step 6. Repeat step 3 M more times to obtain the stop bits.

//==========================================================================
// Module Declaration
//==========================================================================


module uart_rx#
    (
    parameter [3:0] DATA_BIT           = 8,                   // # data bits
    parameter [1:0] PARITY_BIT         = 0,                   // 00,01 <=> NONE, 11 <=> Odd, 10 <=> Even
    parameter [1:0] STOP_BIT           = 0,                    // 0 <=> 1 bit, 1 <=> 1,5 bit, 2 <=> 2 bit
    parameter [3:0] DATA_BIT_OUT       = 10                  // # data bits  (1 parity bit + 1 stop bit + 8 data bits)
    )
    (
    input  wire                           clk,                // system clock (150 MHz)
    input  wire                           reset,              // system reset
    input  wire  [15: 0]                  baud_divisor,       // baud_divisor
    input  wire                           rx,                 // rxd signal 
    output reg                            rx_done_tick,       // Indicate RX Controller receivers enough data (start,datas,stop bit)
    output wire  [DATA_BIT_OUT-1:0]       dout                // RX Data bits
    );

   // symbolic state declaration
   localparam [2:0]
      IDLE            = 3'd0,
      START           = 3'd1,
      DATA            = 3'd2,
      PARITY          = 3'd3,      
      STOP            = 3'd4;

//===================================================================================
// Wire/Register Declaration
//===================================================================================

   reg [2:0]             state_reg, state_next;                // state machine for RX
   reg [15:0]            s_reg, s_next;                        // baud rate count
   reg [2:0]             n_reg, n_next;                        // data bit count
   reg [DATA_BIT -1 :0]  b_reg, b_next;                        // RX shift register
   
   reg                   stop_bit_reg, stop_bit_next;
   reg                   parity_val_reg, parity_val_next;
   reg                   parity_chk_reg, parity_chk_next;
   reg                   parity_err_reg, parity_err_next;
   
   reg                   rx_d1, rx_d2, rx_d3;                  // to detect falling edge
   wire                  rx_falling_edge;
   
   reg  [15:0]           stop_bit_div;
   wire [15:0]           haft_baud_div;
   wire [15:0]           quart_baud_div;

//===================================================================================
// body
//===================================================================================
assign haft_baud_div   = {1'b0, baud_divisor[15:1]};
assign quart_baud_div  = {2'b00,baud_divisor[15:2]};

//-----------------------------------------------------------------------------------
// Delay Rx to avoid metastability
//-----------------------------------------------------------------------------------
   always @(posedge clk, posedge reset)
      if (reset)
         begin
            rx_d1 <= 0;
            rx_d2 <= 0;
            rx_d3 <= 0;
         end
      else
         begin
            rx_d1 <= rx;
            rx_d2 <= rx_d1;
            rx_d3 <= rx_d2;
         end
//-----------------------------------------------------------------------------------
// Detect the falling edge of RXD (for detecting start bit only)
//-----------------------------------------------------------------------------------
assign rx_falling_edge = (~rx_d2) & (rx_d3); 

//-----------------------------------------------------------------------------------
// Calculate the stop bit divisor
//-----------------------------------------------------------------------------------
// Multiplication is a complicated operation and synthesis of the multiplication 
// operator *  depends on synthesis software and target device technology, so we use 
// shift and + operation to calculate stop bit value

   always @*
      case (STOP_BIT)
         2'b00: stop_bit_div = baud_divisor;                  // 1   Stop Bit
         2'b01: stop_bit_div = baud_divisor + quart_baud_div; // 1.5 Stop Bit
         2'b10: stop_bit_div = baud_divisor + haft_baud_div;  // 2   Stop Bit
         2'b11: stop_bit_div = baud_divisor;
      endcase
      
//-----------------------------------------------------------------------------------
// FSMD state & data registers
//-----------------------------------------------------------------------------------
   always @(posedge clk, posedge reset)
      if (reset)
         begin
            state_reg          <= IDLE;
            s_reg              <= 0;
            n_reg              <= 0;
            b_reg              <= 0;
            parity_val_reg     <= 0;
            parity_chk_reg     <= 0;
            stop_bit_reg       <= 0;
            parity_err_reg     <= 0;
         end
      else
         begin
            state_reg          <= state_next;
            s_reg              <= s_next;
            n_reg              <= n_next;
            b_reg              <= b_next;
            parity_val_reg     <= parity_val_next;
            parity_chk_reg     <= parity_chk_next;
            stop_bit_reg       <= stop_bit_next;
            parity_err_reg     <= parity_err_next;
         end
//-----------------------------------------------------------------------------------
// FSMD next-state logic
//-----------------------------------------------------------------------------------
   always @*
   begin
      state_next          = state_reg;
      rx_done_tick        = 1'b0;
      s_next              = s_reg;
      n_next              = n_reg;
      b_next              = b_reg;
      parity_val_next     = parity_val_reg;
      parity_chk_next     = parity_chk_reg;
      parity_err_next     = parity_err_reg;
      stop_bit_next       = stop_bit_reg; 
      case (state_reg)
        IDLE:                                              // if RX changes from 1 to 0, changes to "stop state"
            if (rx_falling_edge)   
                begin
                    state_next = START;
                    s_next     = 0;
                end
         START:                                            // When the counter = baud_divisor/2, 
            if (s_reg == haft_baud_div)                    // the incoming signal reaches the middle point of the start bit
                begin                                      
                    if(rx_d2)                              // if RX is = 1 => noise => return IDLE state
                        begin                                
                            state_next = IDLE;                
                            s_next     = 0;                       
                        end                                  
                    else                                   // else changes to DATA state
                        begin                              
                            state_next      = DATA;             
                            s_next          = 0;                
                            n_next          = 0;           // Reset N counter
                            stop_bit_next   = 0;
                            parity_val_next = 0;
                            parity_chk_next = 0;
                            parity_err_next = 0;
                        end                     
                end
            else
                s_next = s_reg + 1;                        // s++ to increase baud rate count
         DATA:                                             // When the counter = baud_divisor, 
            if (s_reg == baud_divisor)                     // the incoming signal progresses for 1 bit
                begin                                      // and reaches the middle of the next data bit.
                    s_next = 0;                            
                    b_next = {rx, b_reg[DATA_BIT-1:1]};    // shift rx value to shift register
                    if (n_reg == (DATA_BIT-1))             // if n reachs DATA_BIT, changes to next state
                        if(PARITY_BIT[1] == 1'b0)          // if PARITY_BIT = NONE, go to STOP state 
                            state_next = STOP;
                        else
                            state_next = PARITY;           //    else go to PARITY state
                    else                                   
                        n_next = n_reg + 1;                // else n++ to increase number of data bits
                end                                        
            else                                           
                s_next = s_reg + 1;                        // s++ to increase baud rate count
         PARITY:
            if (s_reg == baud_divisor)                     // When the counter = baud_divisor
                begin
                    state_next = STOP;
                    s_next = 0;
                    parity_val_next = rx;                  // parity_val_reg stores Parity Bit value
                    parity_chk_next = ^b_reg;
                end
            else
                s_next = s_reg + 1; 
         STOP:   
            if (s_reg == stop_bit_div)                     // When the counter = stop_bit_div,
                begin
                    state_next    = IDLE;
                    stop_bit_next = rx;
                    if(((PARITY_BIT == 2'b10)&& (parity_val_reg != parity_chk_reg))||((PARITY_BIT == 2'b11)&& (parity_val_reg == parity_chk_reg)))
                        parity_err_next = 1'b1;            // error
                    else
                        parity_err_next = 1'b0;
                    s_next        = 0;
                    rx_done_tick  = 1'b1;                  // RX Controller receivers enough UART protocol 
                end
            else
                  s_next = s_reg + 1;
      endcase
   end
   // output
   assign dout = {parity_err_reg,~stop_bit_reg,{DATA_BIT_OUT-DATA_BIT{1'b0}},b_reg}; // PARITY_ERROR, STOP_ERROR, DATA_BIT
endmodule
