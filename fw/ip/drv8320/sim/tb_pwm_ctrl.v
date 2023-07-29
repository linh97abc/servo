`timescale 1 ns / 1 ps

module tb_pwm_ctrl;

parameter CORE_FREQUENCY = 100_000_000;
parameter DUTY_WIDTH = 16;

localparam SERVO_NUM = 4;

reg clk;
reg reset_n;
reg core_en;
reg [DUTY_WIDTH-2:0] prescaler;
reg [DUTY_WIDTH-2:0] half_period;
reg [5:0] trig_rate;

reg [SERVO_NUM-1:0] start_servo;
reg [DUTY_WIDTH-1:0] u0;
reg [DUTY_WIDTH-1:0] u1;
reg [DUTY_WIDTH-1:0] u2;
reg [DUTY_WIDTH-1:0] u3;


reg [1:0] mode_0;
reg [1:0] mode_1;
reg [1:0] mode_2;
reg [1:0] mode_3;

reg [2:0] hall_0;
reg [2:0] hall_1;
reg [2:0] hall_2;
reg [2:0] hall_3;

wire [5:0]    phase_0;
wire [5:0]    phase_1;
wire [5:0]    phase_2;
wire [5:0]    phase_3;

wire measurement_trigger;

servo_pwm_control inst
 (
    clk,
    reset_n,
    core_en,
    prescaler,
    half_period,
    trig_rate,


    // start
    start_servo,

    // slave 
    u0, // -1 .. 1
    u1,
    u2,
    u3,


    measurement_trigger,

    mode_0,
    mode_1,
    mode_2,
    mode_3,


    // In
    hall_0, // a, b, c
    hall_1, // a, b, c
    hall_2, // a, b, c
    hall_3, // a, b, c
    // Out
    phase_0,
    phase_1,
    phase_2,
    phase_3
);

always #5 clk = ~clk;

initial begin
    clk = 0;
    reset_n = 0;
    core_en = 0;

    prescaler = 10;
    half_period = 100;

    u0 = 0;
    u1 = 0;
    u2 = 0;
    u3 = 0;
    trig_rate = 1;
    start_servo = 0;
    // trig_rate = 0;

    mode_0 = 0;
    mode_1 = 0;
    mode_2 = 0;
    mode_3 = 0;

    hall_0 = 1;
    hall_1 = 1;
    hall_2 = 1;
    hall_3 = 1;

    #15 reset_n = 1;
    // @(posedge clk);
    u0 = 16384;
    u1 = 16384;
    u2 = 16384;
    u3 = 16384;
    start_servo = 4'b1111;
    core_en = 1;

end
    
endmodule