//###################################################################
// Function : Synchonous FIFO
// Coder    : Pong Chu
// Date     : 15/May/2005
//
// Description:
//             - Counter_out
// Version Managerment:
//      Version      Author          Changelog                           Date
//        1.0      Pong Chu           Create                            15/May/2005
//###################################################################
`timescale 1 ns / 1 ps

module uart_sfifo
   #(
     parameter DATA_BIT  = 8, // number of bits in a word
               ADDR_BIT  = 4  // number of address bits          
               
   )
   (
    input wire                     clk, 
    input wire                     reset,

    input wire  [DATA_BIT-1:0]     w_data,
    input wire                     w_valid,
    output wire                    w_ready,

    output wire [DATA_BIT-1:0]     r_data,
    output wire                    r_valid,
    input  wire                    r_ready,

    output wire [ADDR_BIT-1:0]       count_out,  
    output wire                    empty, 
    output wire                    full
   );

//********************************************************************
//Internal connections & variables
//********************************************************************
   wire we;
   reg [ADDR_BIT-1:0] waddr;
   reg [ADDR_BIT-1:0] raddr;

   uart_ram #(
      .DATA_BIT(DATA_BIT), 
      .ADDR_BIT(ADDR_BIT)
      )
      uart_ram_inst
   (
        .clk(clk),
        .we(we),

        .waddr(waddr),
        .wdata(w_data),
        .raddr(raddr),
        .rdata(r_data)
   );

   reg reading;

   assign we = w_valid & w_ready;
   assign count_out = waddr - raddr;
   assign empty = (count_out == 0)? 1'b1: 1'b0;
   assign full = (count_out == (2**ADDR_BIT - 1))? 1'b1: 1'b0;

   assign r_valid = ~empty & ~reading;
   assign w_ready = ~full;
   

   always @(posedge clk ) begin
      if (reset) begin
         waddr <= 0;
         raddr <= 0;
         reading <= 1'b0;
      end else begin

         reading <= (r_ready & r_valid) | (w_valid & w_ready & empty);

         if (w_valid & w_ready) begin
            waddr <= waddr + 1'b1;
         end

         if (r_ready & r_valid & ~reading) begin
            raddr <= raddr + 1'b1;
         end

      end
   end
endmodule
