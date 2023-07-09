//==========================================================================
// Module name : uart_tx.v
// Change Log  :
//      - V1.0: Write Basic UART TX Controller (5/2018)
//
// Description: 
// 1. Overview: This file contains the UART Transmitter Controller
//
// 2.Features:
//  - Data Bit: 5,6,7,8 (configurable)
//  - Stop Bit: 1, 1.5, 2 (configurable)
//===================================================================================
// Module Declaration
//===================================================================================
`timescale 1 ns / 1 ps

module uart_tx
   #(
     parameter [3:0] DATA_BIT           = 8,                  // # data bits
     parameter [1:0] PARITY_BIT         = 0,                  // 00,01 <=> NONE, 11 <=> Odd, 10 <=> Even
     parameter [1:0] STOP_BIT           = 0,                  // 0 <=> 1 bit, 1 <=> 1,5 bit, 2 <=> 2 bit
     parameter [3:0] DATA_BIT_IN        = 8                   // # data bits out
   )
   (
    input wire                            clk,                // system clock
    input wire                            reset,              // system reset (active high)
    input wire  [15: 0]                   baud_divisor,       // baud_divisor
    input wire                            tx_start,           // indicates that CPU writes new data
    input wire  [DATA_BIT_IN - 1:0]       din,                // TX Data bits
    output reg                            tx_done_tick,       // Indicate TX Controller trasmits enough data (start,datas,stop,parity)
    output wire                           tx
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
   // signal declaration
    reg [2:0]             state_reg, state_next;   // state machine regiters
    reg [15:0]            s_reg, s_next;           // baud rate count regiters
    reg [2:0]             n_reg, n_next;           // data bit count regiters
    reg [DATA_BIT - 1:0]  b_reg, b_next;           // TX byte shift regiters
    reg                   tx_reg, tx_next;         // TX bit regiters
    reg                   p_reg, p_next;           // Parity regiters
    

    reg  [15: 0]          stop_bit_div;
    wire [15: 0]          haft_baud_div;
    wire [15: 0]          quart_baud_div;

//===================================================================================
// body
//===================================================================================
assign haft_baud_div   = {1'b0, baud_divisor[15:1]};
assign quart_baud_div  = {2'b00,baud_divisor[15:2]};

// Calculate the stop bit divisor

// Multiplication is a complicated operation and synthesis of the multiplication 
// operator *  depends on synthesis software and target device technology, so we use 
// shift and + operation to calculate stop bit value
//   assign stop_bit_status   = STOP_BIT;
//    assign parity_bit_status = PARITY_BIT;
                       
   always @(baud_divisor,quart_baud_div,haft_baud_div)
       case (STOP_BIT)
           2'b00: stop_bit_div = baud_divisor;                  // 1   Stop Bit
           2'b01: stop_bit_div = baud_divisor + haft_baud_div; // 1.5 Stop Bit
           2'b10: stop_bit_div = {baud_divisor[14:0],1'b0};    // 2   Stop Bit
           2'b11: stop_bit_div = baud_divisor;
       endcase                           
//-----------------------------------------------------------------------------------
// FSMD state & data registers
//-----------------------------------------------------------------------------------
    always @(posedge clk, posedge reset)
       if (reset)
          begin
            state_reg <= IDLE;
            s_reg     <= 0;
            n_reg     <= 0;
            b_reg     <= 0;
            tx_reg    <= 1'b1; // in IDLE state: TXD signal = 1  
            p_reg     <= 0;
          end
       else
          begin
            state_reg <= state_next;
            s_reg     <= s_next;
            n_reg     <= n_next;
            b_reg     <= b_next;
            tx_reg    <= tx_next;
            p_reg     <= p_next;
          end
//- ----------------------------------------------------------------------------------
//  FSMD next-state logic & functional units
//- ----------------------------------------------------------------------------------
    always @*
    begin
        state_next   = state_reg;
        tx_done_tick = 1'b0;
        s_next       = s_reg;
        n_next       = n_reg;
        b_next       = b_reg;
        tx_next      = tx_reg;
        p_next       = p_reg;
        case (state_reg)
        IDLE:                      
            begin
                tx_next = 1'b1;                        // IDLE: TXD = 1, 
                if (tx_start)                          // if having new data (tx_start = 1), change to "START state"
                    begin
                        state_next = START;
                        s_next = 0;
                        b_next = din[DATA_BIT-1:0];    // copy TX data to b register
                        p_next = ^din[DATA_BIT-1:0]; 
                    end                                
            end                                        
        START:                                         // START: TXD = 0
           begin                                       
                tx_next = 1'b0;                        
                if (s_reg == baud_divisor)             // when s counter = baud_divisor, changes to "data state"
                    begin                          
                        state_next = DATA;         
                        s_next = 0;                
                        n_next = 0;                
                    end                            
                else                               
                    s_next = s_reg + 1;            
           end                                         
        DATA:                                          
            begin                                      
                tx_next = b_reg[0];                    // DATA: TXD = LSB of Data In (UART Standard)
                if (s_reg== baud_divisor)              // when s counter = baud_divisor, shift to new data
                    begin                              
                        s_next = 0;                    
                        b_next = b_reg >> 1;           
                        if (n_reg == (DATA_BIT-1))     // when TX controller transmits all data, change to "stop state"
                            state_next = PARITY;
                        else
                            n_next = n_reg + 1;
                    end
                else
                    s_next = s_reg + 1;
            end
        PARITY:
            begin
                if (PARITY_BIT[1] == 0)                // Parity Bit = None
                    state_next = STOP;                 
                else                                   
                    begin                              
                        if (PARITY_BIT[0] == 0)        // Parity Bit = Even
                            tx_next = p_reg;
                        else                           // Parity Bit = Odd
                            tx_next = ~p_reg;
                            
                        if (s_reg== baud_divisor)
                            begin
                                state_next = STOP;
                                s_next = 0;
                            end
                        else
                            s_next = s_reg + 1;
                    end
              end
        STOP:  
           begin
                tx_next = 1'b1;                        //STOP: TXD signal = 1
                    if (s_reg == stop_bit_div)      
                        begin                           
                            state_next = IDLE;         // when counter = baud_divisor, tx_done_tick = 1
                            tx_done_tick = 1'b1;       // and change to "IDLE state"
                        end
                    else
                        s_next = s_reg + 1;
           end
        endcase
    end
    // output
    assign tx = tx_reg;

endmodule