`timescale 1 ns / 1 ps

module uart_sfifo_nopipe
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

   //  output reg [ADDR_BIT-1:0]     count_out,  
    output reg [ADDR_BIT:0]     count_out,  
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

   assign empty = ~(|count_out);
   // assign full = &count_out;
   assign full = count_out[ADDR_BIT];

   assign r_valid = ~empty & ~reading;
   assign w_ready = ~full;
   

   always @(posedge clk ) begin
      if (reset) begin
         waddr <= 0;
         raddr <= 0;
         reading <= 1'b0;
         count_out <= 0;
      end else begin

         reading <= (r_ready & r_valid) | (w_valid & w_ready & empty);

         if (w_valid & w_ready) begin
            waddr <= waddr + 1'b1;
         end

         if (r_ready & r_valid) begin
            raddr <= raddr + 1'b1;
         end

         if (w_valid & w_ready & ~(r_ready & r_valid)) begin
            count_out <= count_out + 1'b1;
         end else if (~(w_valid & w_ready) & r_ready & r_valid) begin
            count_out <= count_out - 1'b1;
         end

      end
   end
endmodule
