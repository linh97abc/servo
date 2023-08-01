`timescale 1 ns / 1 ps

module tb_u2duty;

localparam DUTY_WIDTH = 16;

reg    clk;
reg    reset_n;

reg  [DUTY_WIDTH-2:0]  half_period;
    // slave 
reg  [DUTY_WIDTH-1:0]  u0; // -1 .. 1
reg  [DUTY_WIDTH-1:0]  u1;
reg  [DUTY_WIDTH-1:0]  u2;
reg  [DUTY_WIDTH-1:0]  u3;

wire  [DUTY_WIDTH-2:0]  duty0;
wire  [DUTY_WIDTH-2:0]  duty1;
wire  [DUTY_WIDTH-2:0]  duty2;
wire  [DUTY_WIDTH-2:0]  duty3;
wire [3:0] direction;


servo_u2duty_wrapper inst
(
    clk,
    reset_n,

    half_period,
    // slave 
    u0, // -1 .. 1
    u1,
    u2,
    u3,

    duty0,
    duty1,
    duty2,
    duty3,

    direction
);

always #5 clk = ~clk;

initial begin
    clk = 0;
    reset_n = 0;
    half_period = 100;
    u0 = 0;
    u1 = 0;
    u2 = 0;
    u3 = 0;

    #15 reset_n = 1;
    @(posedge clk);

    u0 = 16'hFFFF;
    u1 = 16'hFFFF;
    u2 = 16'hFFFF;
    u3 = 16'hFFFF;
    repeat(10)
     @(posedge clk);

    u0 = 'h1000;
    u1 = 'h1000;
    u2 = 'h1000;
    u3 = 'h1000;
    repeat(10)
@(posedge clk);

    u0 = 16'hF000;
    u1 = 16'hF000;
    u2 = 16'hF000;
    u3 = 16'hF000;
    repeat(10)
@(posedge clk);

    u0 = 'h1000;
    u1 = 'hF000;
    u2 = 'hFF00;
    u3 = 'h1FF0;
    repeat(10)
@(posedge clk);

    half_period = {DUTY_WIDTH-1{1'b1}};
    u0 = 'h7FFF;
    u1 = 'hFFFF;
    u2 = 'h8000;
    u3 = 'h0000;
end
    
endmodule