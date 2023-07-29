module servo_pwm_control (
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

parameter DUTY_WIDTH = 16;

localparam SERVO_NUM = 4;

input clk;
input reset_n;
input core_en;
input [DUTY_WIDTH-2:0] prescaler;
input [DUTY_WIDTH-2:0] half_period;
input [5:0] trig_rate;

input [SERVO_NUM-1:0] start_servo;
input [DUTY_WIDTH-1:0] u0;
input [DUTY_WIDTH-1:0] u1;
input [DUTY_WIDTH-1:0] u2;
input [DUTY_WIDTH-1:0] u3;


input [1:0] mode_0;
input [1:0] mode_1;
input [1:0] mode_2;
input [1:0] mode_3;

input [2:0] hall_0;
input [2:0] hall_1;
input [2:0] hall_2;
input [2:0] hall_3;

output [5:0]    phase_0;
output [5:0]    phase_1;
output [5:0]    phase_2;
output [5:0]    phase_3;

output measurement_trigger;
wire [SERVO_NUM-1:0] stop;

wire [SERVO_NUM-1:0] pwmx4;
wire [SERVO_NUM-1:0] pwmx4_n;

wire [DUTY_WIDTH-2:0] d0;
wire [DUTY_WIDTH-2:0] d1;
wire [DUTY_WIDTH-2:0] d2;
wire [DUTY_WIDTH-2:0] d3;

wire [SERVO_NUM-1:0] direction;

assign stop = ~start_servo;

servo_u2duty_wrapper servo_u2duty_wrapper_inst
(
    .clk(clk),
    .reset_n(reset_n),

    .half_period(half_period),
    // slave 
    .u0(u0), // -1 .. 1
    .u1(u1),
    .u2(u2),
    .u3(u3),
    

    .duty0(d0),
    .duty1(d1),
    .duty2(d2),
    .duty3(d3),
    

    .direction(direction)
);

servo_pwmx4 #(
        .DWIDTH(DUTY_WIDTH-1)
    ) servo_pwmx4_inst
 (
    .clk(clk),
    .reset_n(reset_n),
    .core_en(core_en),
    .prescaler(prescaler),
    .half_period(half_period),
    .trig_rate(trig_rate),

    .d0(d0),
    .d1(d1),
    .d2(d2),
    .d3(d3),

    .measurement_trigger(measurement_trigger),
    .pwmx4(pwmx4),
    .pwmx4_n(pwmx4_n)
);

servo_3phase_pwm servo_3phase_pwm_inst_0
 (
    .mode(mode_0),
    .stop(stop[0]),
    .direction(direction[0]),

    .hall(hall_0), // a, b, c
    .pwm(pwmx4[0]),
    .pwm_n(pwmx4_n[0]),

    //
    .phase(phase_0)
);

servo_3phase_pwm servo_3phase_pwm_inst_1
 (
    .mode(mode_1),
    .stop(stop[1]),
    .direction(direction[1]),

    .hall(hall_1), // a, b, c
    .pwm(pwmx4[1]),
    .pwm_n(pwmx4_n[1]),

    //
    .phase(phase_1)
);

servo_3phase_pwm servo_3phase_pwm_inst_2
 (
    .mode(mode_2),
    .stop(stop[2]),
    .direction(direction[2]),

    .hall(hall_2), // a, b, c
    .pwm(pwmx4[2]),
    .pwm_n(pwmx4_n[2]),

    //
    .phase(phase_2)
);

servo_3phase_pwm servo_3phase_pwm_inst_3
 (
    .mode(mode_3),
    .stop(stop[3]),
    .direction(direction[3]),

    .hall(hall_3), // a, b, c
    .pwm(pwmx4[3]),
    .pwm_n(pwmx4_n[3]),

    //
    .phase(phase_3)
);

endmodule