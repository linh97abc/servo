`timescale 1 ns / 1 ps
module tb_avl_fifo_uart;

    parameter CLK_FREQ_HZ  = 1000_000;
    parameter BAUDRATE  = 100_0000;
    // parameter [15:0] BAUDRATE  = 16'd200_000;
    parameter [15:0] RX_TIMEOUT_US = 100;
    parameter DATA_BIT  = 8; // number of bits in a word
    parameter [1:0] PARITY_BIT         = 0;                   // 00,01 <=> NONE, 11 <=> Odd, 10 <=> Even
    parameter [1:0] STOP_BIT           = 0;                    // 0 <=> 1 bit, 1 <=> 1,5 bit, 2 <=> 2 bit
    parameter RX_FIFO_DEPTH  = 4;
    parameter TX_FIFO_DEPTH  = 4;

    //clk, reset
    reg clk;
    reg reset_n;
    
    //avalon mm interface
    reg [4:0] address;
    reg [31:0] writedata;
    reg write_n;
    reg read_n;    
    wire [31:0] readdata;  

    // uart
    wire rxd;
     wire txd;

    //irq
     wire irq;

     assign rxd = txd;

avl_fifo_uart
#(
     CLK_FREQ_HZ ,
      BAUDRATE  ,
    //   RX_TIMEOUT_US ,
     DATA_BIT  , // number of bits in a word
      PARITY_BIT        ,                   // 00,01 <=> NONE, 11 <=> Odd, 10 <=> Even
      STOP_BIT          ,                    // 0 <=> 1 bit, 1 <=> 1,5 bit, 2 <=> 2 bit
     RX_FIFO_DEPTH  ,
     TX_FIFO_DEPTH  
)
inst
(
    //clk, reset
     clk,
     reset_n,
    
    //avalon mm interface
      address,
      writedata,
     write_n,
     read_n,    
     readdata,  

    // uart
     rxd,
   txd,

    //irq
   irq
);

always #5 clk = ~clk;

task avl_write;
input [31:0] addr;
input [31:0] data;
begin
    @(posedge clk);
    #1
    address = addr;
    writedata = data;
    write_n = 0;
    @(posedge clk);
    #1
    write_n = 1;
end
endtask

reg [31:0] avl_read_d;
wire [DATA_BIT-1:0] rx_data;

assign rx_data = avl_read_d[DATA_BIT-1:0];

task avl_read;
input [31:0] addr;
begin
    @(posedge clk);
    #1
    address = addr;
    read_n = 0;
    @(posedge clk);
    #1
    read_n = 1;

    avl_read_d = readdata;
end
endtask

initial begin
    clk = 0;
    reset_n = 0;
    address = 0;
    writedata = 0;
    read_n = 1;
    write_n = 1;

    #15
    reset_n = 1;
    @(posedge clk);

    avl_write(inst.CR_REG, {16'd9 , 6'b0 ,1'b1, 1'b0});
    avl_write(inst.CR_REG, {16'd9 , 6'b0 ,1'b0, 1'b1});


    avl_write(inst.RX_THRESHOLD_REG, 8);
    avl_write(inst.IE_REG, 1 << 5);

    avl_write(inst.TX_REG, 0);
    avl_write(inst.TX_REG, 1);
    avl_write(inst.TX_REG, 2);
    avl_write(inst.TX_REG, 3);

    wait(irq == 1'b1);

    // avl_read(inst.RX_REG);
    // avl_read(inst.RX_REG);
    // avl_read(inst.RX_REG);
    // avl_read(inst.RX_REG);

end

endmodule

