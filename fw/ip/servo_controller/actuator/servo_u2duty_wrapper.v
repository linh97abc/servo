module servo_u2duty_wrapper
(
    clk,
    reset_n,

    half_period,
    // slave 
    u0, // -1 .. 1
    u1,
    u2,
    u3,
    s_valid,

    duty0,
    duty1,
    duty2,
    duty3,
    // m_valid,

    direction
);

localparam DUTY_WIDTH = 16;

input    clk;
input    reset_n;

input  [DUTY_WIDTH-2:0]  half_period;
    // slave 
input  [DUTY_WIDTH-1:0]  u0; // -1 .. 1
input  [DUTY_WIDTH-1:0]  u1;
input  [DUTY_WIDTH-1:0]  u2;
input  [DUTY_WIDTH-1:0]  u3;
input s_valid;

output wire  [DUTY_WIDTH-2:0]  duty0;
output wire  [DUTY_WIDTH-2:0]  duty1;
output wire  [DUTY_WIDTH-2:0]  duty2;
output wire  [DUTY_WIDTH-2:0]  duty3;
output wire [3:0] direction;
// output m_valid;

// localparam STEP_TIME = 4;
// reg [STEP_TIME-1:0] progress;

// assign m_valid = progress[STEP_TIME-1];

// always @(posedge clk) begin
//     progress <= {progress[STEP_TIME-2:0], s_valid}
// end

drv8320_mul_halfperiod_duty#
(
.A_DWIDTH(DUTY_WIDTH-1),
.B_DWIDTH(DUTY_WIDTH)
)
drv8320_mul_halfperiod_duty_inst0
(
.clk(clk), . rst(~reset_n), .a(half_period), .b(u0), .p(duty0), .sign(direction[0])
);

drv8320_mul_halfperiod_duty#
(
.A_DWIDTH(DUTY_WIDTH-1),
.B_DWIDTH(DUTY_WIDTH)
)
drv8320_mul_halfperiod_duty_inst1
(
.clk(clk), . rst(~reset_n), .a(half_period), .b(u1), .p(duty1), .sign(direction[1])
);

drv8320_mul_halfperiod_duty#
(
.A_DWIDTH(DUTY_WIDTH-1),
.B_DWIDTH(DUTY_WIDTH)
)
drv8320_mul_halfperiod_duty_inst2
(
.clk(clk), . rst(~reset_n), .a(half_period), .b(u2), .p(duty2), .sign(direction[2])
);

drv8320_mul_halfperiod_duty#
(
.A_DWIDTH(DUTY_WIDTH-1),
.B_DWIDTH(DUTY_WIDTH)
)
drv8320_mul_halfperiod_duty_inst3
(
.clk(clk), . rst(~reset_n), .a(half_period), .b(u3), .p(duty3), .sign(direction[3])
);


endmodule

module drv8320_mul_halfperiod_duty(clk, rst, a, b, p, sign);
  parameter A_DWIDTH = 1;
  parameter B_DWIDTH = 1;

input clk;
input rst;
input [A_DWIDTH - 1 : 0] a;
input signed [B_DWIDTH - 1 : 0] b;
output wire [A_DWIDTH - 1 : 0] p;
output reg sign;
wire ce;

reg [A_DWIDTH - 1 : 0] a_reg; 
reg signed [B_DWIDTH - 1 : 0] b_reg; 

reg signed [A_DWIDTH+B_DWIDTH : 0] p_reg; 
reg signed [A_DWIDTH+B_DWIDTH : 0] p_reg_tmp; 

wire [A_DWIDTH : 0] p_reg_tmp2;
reg [A_DWIDTH : 0] p_reg_tmp3;

assign ce = 1;

always @ (posedge clk) begin
    if (ce) begin
        a_reg <= a;
        b_reg <= b;
        p_reg_tmp <= $signed({1'b0, a_reg}) * b_reg;
        p_reg <= p_reg_tmp;
    end
end

assign p_reg_tmp2 = p_reg[A_DWIDTH+B_DWIDTH-1:B_DWIDTH-1];

always @ (posedge clk) begin
    if (ce) begin
        p_reg_tmp3 <= p_reg_tmp2[A_DWIDTH]? ({A_DWIDTH{1'b0}} - p_reg_tmp2): p_reg_tmp2;
        sign <= p_reg_tmp2[A_DWIDTH];
    end
end

assign p = p_reg_tmp3[A_DWIDTH-1:0];

endmodule