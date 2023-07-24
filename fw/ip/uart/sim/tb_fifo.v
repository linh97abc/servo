`timescale 1 ns / 1 ps

module tb_fifo;

localparam DATA_BIT = 8;
localparam ADDR_BIT = 4;

    reg                     clk; 
    reg                     reset;

    reg  [DATA_BIT-1:0]     w_data;
    reg                     w_valid;
    wire                    w_ready;

    wire [DATA_BIT-1:0]     r_data;
    wire                    r_valid;
    reg                    r_ready;

    wire [ADDR_BIT:0]       count_out;  
    wire                    empty; 
    wire                    full;

// uart_sfifo
uart_sfifo_nopipe
   #(DATA_BIT, ADDR_BIT)
   inst
   (
    clk, 
    reset,

     w_data,
     w_valid,
     w_ready,

     r_data,
    r_valid,
     r_ready,

      count_out,  
     empty, 
     full
   );

   always #5 clk = ~clk;

   initial begin
    clk = 0;
    reset = 1;
    w_data = 0;
    w_valid = 0;
    r_ready = 0;

    #15 
    reset = 0;
    @(posedge clk);
    #1 w_valid = 1;

    repeat(20) begin
        @(posedge clk);
        #1 w_data = w_data + 1;
    end

    @(posedge clk);
    #1 r_ready = 1;

    repeat(5) begin
        @(posedge clk);
        #1 w_data = w_data + 1;
    end

    #1 w_valid = 0;

    repeat(50) @(posedge clk);

    #1 w_valid = 1;
    w_data = w_data + 1;
    repeat(5) begin
        @(posedge clk);
        #1 w_data = w_data + 1;
    end

    #1 w_valid = 0;

    repeat(5) @(posedge clk);
    #1 r_ready = 0;

    repeat(5) begin
        @(posedge clk);
        #1 w_data = w_data + 1;
        w_valid = 1;
        @(posedge clk);
        #1 w_valid = 0;
    end

    repeat(4) begin
        @(posedge clk);
        #1 r_ready = 1;
        @(posedge clk);
        #1 r_ready = 0;
    end

    @(posedge clk);
    #1 r_ready = 1;
    w_data = w_data + 1;
    w_valid = 1;
    @(posedge clk);
    #1 r_ready = 0;
    w_valid = 0;

    repeat(5) begin
        @(posedge clk);
        #1 r_ready = 1;
        @(posedge clk);
        #1 r_ready = 0;
    end
   end

endmodule