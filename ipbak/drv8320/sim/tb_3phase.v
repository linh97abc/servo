`timescale 1 ns / 1 ps

module tb_3phase;

reg [1:0] mode; //0-6x, 1-3x, 2-1x
reg stop;
reg direction;
reg [2:0] hall;
reg pwm;
wire pwm_n;
wire [5:0] phase;

servo_3phase_pwm inst
 (
    mode,
    stop,
    direction,

    hall, // a, b, c
    pwm,
    pwm_n,

    // ha, la, hb, lb, hc, lc
    phase
);

assign pwm_n = ~pwm;

initial begin
    mode = 0;
    pwm = 1;
    stop = 1;
    direction = 0;
    hall = 0;

    #10
    hall = 3'b001;

    #10
    stop = 0;

    
    #10 hall = 3'b001;
    #10 hall = 3'b011;
    #10 hall = 3'b010;
    #10 hall = 3'b110;
    #10 hall = 3'b100;
    #10 hall = 3'b101;
    





end
    
endmodule