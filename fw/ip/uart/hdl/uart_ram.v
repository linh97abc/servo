`timescale 1 ns / 1 ps

module uart_ram
   #(
     parameter DATA_BIT  = 8, // number of bits in a word
               ADDR_BIT  = 4  // number of address bits          
               
   )
   (
        input clk,
        input we,

        input [ADDR_BIT-1:0] waddr,
        input [DATA_BIT-1:0] wdata,
        input [ADDR_BIT-1:0] raddr,
        output reg [DATA_BIT-1:0] rdata


   );

    reg  [DATA_BIT-1:0]    array_reg [0:2**ADDR_BIT-1]; // register array


   always @(posedge clk) begin
    if (we) begin
        array_reg[waddr] <= wdata;
    end

    rdata <= array_reg[raddr];
   end



endmodule