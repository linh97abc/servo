module tmp101#
(
    parameter FREQ_CLK = 100_000_000;
    parameter BUS_CLK = 400_000;
    parameter [7:0] I2C_ADDR = 4;
)
 (
    clk,
    reset_n,
    ena,

    t_high,
    t_low,

    temperature,
    status,

    sda,
    scl
);
    
input clk;
input reset_n;
input ena;

input [11:0] t_high;
input [11:0] t_low;
output [11:0] temperature;
output [1:0] status; //i2c_busy, i2c_ack_err

inout sda;
inout scl;


wire tmp101_addr;
assign tmp101_addr = I2C_ADDR;

reg rw;
reg [7:0] i2c_dout;
wire [7:0] i2c_din;
wire i2c_busy;
wire ack_error;

assign status = {i2c_busy, ack_error};

i2c_master#
(
.input_clk(FREQ_CLK),
.bus_clk(BUS_CLK)
)
i2c_master_inst
(
.clk(clk),
.reset_n(reset_n),
.ena(ena),
.addr(tmp101_addr),
.rw(rw),
.data_wr(i2c_dout),
.busy(i2c_busy),
.data_rd(i2c_din),
.ack_error(ack_error),
.sda(sda),
.scl(scl)
)

endmodule