`timescale 1 ns / 1 ps

module tb_tmp101;

parameter FREQ_CLK = 100_000_000;
parameter BUS_CLK = 400_000;
parameter UPDATE_FREQ = 2;
parameter [2:0] I2C_ADDR = 0;

reg clk;
reg reset_n;
reg ena;

wire [11:0] temperature;
wire [1:0] status; //i2c_busy, i2c_ack_err

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
        // @(negedge scl_t);
        // @(negedge scl_t);
        // @(negedge scl_t);
        // @(negedge scl_t);
        // @(negedge scl_t);
    end else begin
        repeat(8)    @(posedge scl_t);
        @(negedge scl_t);
        #1 sda_i_s = 0;
        @(posedge scl_t);
        @(negedge scl_t);
        #1 sda_i_s = 1;
    end

    wait(stop_cond == 1);
end
endtask

tmp101#(
.FREQ_CLK(FREQ_CLK),
.BUS_CLK(BUS_CLK),
.UPDATE_FREQ(UPDATE_FREQ),
.I2C_ADDR(I2C_ADDR)
) inst
 (
    .clk(clk),
    .reset_n(reset_n),
    .ena(ena),

    .temperature(temperature),
    .status(status),

    .sda_t(sda_t),
    .sda_i(sda_i),
    .scl_t(scl_t),
    .scl_i(scl_i)
);

always #5 clk = ~clk;

initial
forever begin
    i2c_tmp101();
end

initial begin
clk = 0;
reset_n = 0;
ena = 0;
#15 reset_n = 1;

@(posedge clk);
ena = 1;


end
    
endmodule