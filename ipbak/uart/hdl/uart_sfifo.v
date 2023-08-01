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

    output wire [ADDR_BIT:0]       count_out,  
    output wire                    empty, 
    output wire                    full
   );

//********************************************************************
//Internal connections & variables
//********************************************************************
   wire we;
   wire ram_rvalid;
   wire ram_rready;
   wire [DATA_BIT-1:0] ram_rdata;
   wire [ADDR_BIT-1:0] ram_count;
   wire ram_empty;

   reg [ADDR_BIT-1:0] waddr;
   reg [ADDR_BIT-1:0] raddr;
   reg [DATA_BIT-1:0] rx_pipeline_data;
   reg reg_rvalid;
   reg rx_pipeline_valid;
   reg first_writing;
   reg rx_pipeline_pending;
   reg r_ready_prev;

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
        .rdata(ram_rdata)
   );

   assign we = w_valid & w_ready;
   assign ram_count = waddr - raddr;

   assign ram_empty = ~(|ram_count);
   assign full = &ram_count;

   assign ram_rvalid = ~ram_empty & ~first_writing;
   assign r_valid = reg_rvalid;
   assign w_ready = ~full;
   assign ram_rready = r_ready | ~rx_pipeline_valid;
   assign r_data = rx_pipeline_valid? rx_pipeline_data: ram_rdata;
   assign count_out = ram_count + rx_pipeline_pending;
   assign empty = ram_empty & ~rx_pipeline_pending;

   always @(posedge clk ) begin
      if (reset) begin
         waddr <= 0;
         raddr <= 0;
         rx_pipeline_data <= 0;
         rx_pipeline_valid <= 1'b0;
         first_writing <= 1'b1;
         reg_rvalid <= 1'b0;
         rx_pipeline_pending <= 1'b0;
         r_ready_prev <= 1'b0;
      end else begin
         reg_rvalid <= ((rx_pipeline_valid | ram_rvalid) & ~ram_empty) | (reg_rvalid & ~r_ready);
         first_writing <= w_valid & w_ready & ram_empty;
         rx_pipeline_data <= (ram_rvalid & (r_ready | ~rx_pipeline_valid))? ram_rdata:rx_pipeline_data;
         r_ready_prev <= r_ready;

         rx_pipeline_valid <= (rx_pipeline_valid & ~r_ready) | 
                              (~r_ready_prev & ram_rvalid);

         rx_pipeline_pending <= (rx_pipeline_pending & ~r_ready) | 
                              (rx_pipeline_pending & ram_rvalid) |
                              (~(r_ready & reg_rvalid) & ram_rvalid);

         if (w_valid & w_ready) begin
            waddr <= waddr + 1'b1;
         end

         if (ram_rready & ram_rvalid) begin
            raddr <= raddr + 1'b1;
         end

      end
   end
endmodule
