module servo_pwm_control (
    clk,
    reset_n,
    core_en,
    prescaler,
    half_period,

    // start
    start_servo,
    protected_en,

    // slave 
    u0, // -1 .. 1
    u1,
    u2,
    u3,
    s_valid,

    measurement_trigger,

    //
    i0,
    i1,
    i2,
    i3,

    i0_max,
    i1_max,
    i2_max,
    i3_max,

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
    phase_3,

    stop
);

parameter CORE_FREQUENCY = 100_000_000;
parameter [7:0] DEAD_TIME = 1;
parameter DUTY_WIDTH = 16;
parameter ADC_WIDTH = 12;

localparam SERVO_NUM = 4;

input clk;
input reset_n;
input core_en;
input [DUTY_WIDTH-2:0] prescaler;
input [DUTY_WIDTH-2:0] half_period;
input [SERVO_NUM-1:0] start_servo;
input [SERVO_NUM-1:0] protected_en;
input [DUTY_WIDTH-1:0] u0;
input [DUTY_WIDTH-1:0] u1;
input [DUTY_WIDTH-1:0] u2;
input [DUTY_WIDTH-1:0] u3;
input s_valid;

input [ADC_WIDTH-1:0]    i0;
input [ADC_WIDTH-1:0]    i1;
input [ADC_WIDTH-1:0]    i2;
input [ADC_WIDTH-1:0]    i3;

input [ADC_WIDTH-2:0]    i0_max;
input [ADC_WIDTH-2:0]    i1_max;
input [ADC_WIDTH-2:0]    i2_max;
input [ADC_WIDTH-2:0]    i3_max;

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
output reg [SERVO_NUM-1:0] stop;

wire [SERVO_NUM-1:0] signal_overload;
wire [SERVO_NUM-1:0] pwmx4;
wire [SERVO_NUM-1:0] pwmx4_n;

wire [DUTY_WIDTH-2:0] d0;
wire [DUTY_WIDTH-2:0] d1;
wire [DUTY_WIDTH-2:0] d2;
wire [DUTY_WIDTH-2:0] d3;

wire [SERVO_NUM-1:0] direction;

servo_protect#(
    .ADC_WIDTH(ADC_WIDTH)
)
 (
    .clk(clk),
    .reset_n(reset_n),
    .i0(i0),
    .i1(i1),
    .i2(i2),
    .i3(i3),

    .i0_max(i0_max),
    .i1_max(i1_max),
    .i2_max(i2_max),
    .i3_max(i3_max),

    .stop(signal_overload)
);



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
    .s_valid(s_valid),

    .duty0(d0),
    .duty1(d1),
    .duty2(d2),
    .duty3(d3),

    .direction(direction)
);

servo_pwmx4 #(
        .DEAD_TIME(DEAD_TIME),
        .DWIDTH(DUTY_WIDTH-1)
    ) servo_pwmx4_inst
 (
    .clk(clk),
    .reset_n(reset_n),
    .core_en(core_en),
    .prescaler(prescaler),
    .half_period(half_period),

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

always @(posedge clk) begin
    if (~reset_n) begin
        stop <= {SERVO_NUM{1'b1}};
    end else begin
        stop <= (stop | signal_overload) & ~start_servo & protected_en;
        
    end
end

endmodule