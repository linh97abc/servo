`timescale 1 ns / 1 ps

module tb_ltc2992;

parameter FREQ_CLK = 100_000_000;
parameter BUS_CLK = 400_000;
parameter UPDATE_FREQ = 2;
parameter [2:0] I2C_ADDR = 0;

reg clk = 0;
reg reset_n = 1;

reg [3:0] address = 0;
reg [31:0] writedata = 0;
reg write_n = 1;
reg read_n = 1;    
wire [31:0] readdata; 
wire irq;

wire sda_t;
wire scl_t;
wire sda_i;
wire scl_i;

reg sda_i_s = 1;

assign scl_i = scl_t;
assign sda_i = sda_i_s & sda_t;

reg start_cond, stop_cond;
reg sda_i_last;

always @(posedge clk) begin
    sda_i_last <= sda_i;

    start_cond <= scl_i & (~sda_i & sda_i_last);
    stop_cond <= scl_i & (sda_i & ~sda_i_last);
end

task do_i2c_ack;
begin
            repeat(8)    @(posedge scl_t);
        @(negedge scl_t);
        #1 sda_i_s = 0;
        @(posedge scl_t);
        @(negedge scl_t);
        #1 sda_i_s = 1;
end
endtask

task i2c_tmp101;
reg rw;
begin
    sda_i_s = 1;

    // wait(scl_t == 1);
    // @(negedge sda_t);
    // while(scl_t == 0) @(negedge sda_t);

    wait(start_cond == 1);

    $display("new i2c trans");
    repeat(8)    @(posedge scl_t);
    rw = sda_t;

    // $stop;
    $display("i2c ack");
    @(negedge scl_t);
    #1 sda_i_s = 0;
    @(posedge scl_t);
    @(negedge scl_t);
    #1 sda_i_s = 1;

    $display("i2c data");
    if (rw) begin
        @(negedge scl_t);
        sda_i_s = 1;
        @(negedge scl_t);
        sda_i_s = 0;
        @(negedge scl_t);
        sda_i_s = 1;

    end else begin
        do_i2c_ack();

        do_i2c_ack();
        do_i2c_ack();
    end

    wait(stop_cond == 1);
end
endtask

avl_ltc2992#(
.FREQ_CLK(FREQ_CLK),
.BUS_CLK(BUS_CLK)
) inst
 (
    .clk(clk),
    .reset_n(reset_n),

    .address(address),
    .writedata(writedata),
    .write_n(write_n),
    .read_n(read_n),
    .readdata(readdata),


    .sda_t(sda_t),
    .sda_i(sda_i),
    .scl_t(scl_t),
    .scl_i(scl_i),

    .irq(irq)
);

task avl_write;
input [31:0] addr;
input [31:0] data;
begin
    @(posedge clk);
    #1
    address <= addr;
    writedata <= data;
    write_n <= 0;
    @(posedge clk);
    #1
    write_n <= 1;
end
endtask

reg [31:0] avl_read_d;

task avl_read;
input [31:0] addr;
begin
    @(posedge clk);
    #1
    address <= addr;
    read_n <= 0;
    @(posedge clk);
    #1
    read_n <= 1;

    avl_read_d <= readdata;
end
endtask

always #5 clk = ~clk;

initial
forever begin
    i2c_tmp101();
end

reg core_en = 0;
reg transmit_rw = 0;
reg[2:0] transmit_len = 0;
reg [6:0] reg_i2c_addr = 0;
reg[7:0] i2c_command = 0;

wire [31:0] i2c_control;
reg [31:0] i2c_control_tmp;
assign i2c_control = {core_en,
 4'b0, 
 transmit_rw, 
 5'b0,
  transmit_len, 
  1'b0, 
  reg_i2c_addr, 
  i2c_command};

initial begin
clk = 0;
reset_n = 0;
#15 reset_n = 1;

@(posedge clk);

core_en = 1;
transmit_rw = 0;
transmit_len = 2;
reg_i2c_addr = 1;
i2c_command = 1;


@(posedge clk);

avl_write(inst.CR_REG, i2c_control | {1'b1, 31'b0});
avl_write(inst.CR_REG, i2c_control);

avl_write(inst.IE_REG, 1);
avl_write(inst.TX_REG, 5);
avl_write(inst.CR_REG, i2c_control | {1'b1, 30'b0});
wait(irq == 1'b1);
avl_write(inst.FLAG_REG, 1);

transmit_rw = 1;
@(posedge clk);
i2c_control_tmp = i2c_control | {1'b1, 30'b0};
// $display("Set i2c_control=%d, i2c_control_tmp=%d",
//     i2c_control,
//         i2c_control_tmp);

// $display("Set transmit_rw=%d, i2c_control[24]=%d, i2c_control_tmp[24]=%d",
// transmit_rw,
//     i2c_control[24],
//         i2c_control_tmp[24]);

avl_write(inst.CR_REG, i2c_control_tmp);

wait(irq == 1'b1);
avl_write(inst.FLAG_REG, 1);

end
    
endmodule