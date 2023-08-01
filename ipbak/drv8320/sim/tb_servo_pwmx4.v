`timescale 1 ns / 1 ps

module tb_pwmx4;

parameter [1:0] DEAD_TIME = 1;
parameter DWIDTH = 15;

reg clk;
reg reset_n;
reg core_en;
reg [DWIDTH-1:0] prescaler;
reg [DWIDTH-1:0] half_period;
reg [5:0] trig_rate;
reg [DWIDTH-1:0] d0;
reg [DWIDTH-1:0] d1;
reg [DWIDTH-1:0] d2;
reg [DWIDTH-1:0] d3;


wire measurement_trigger;
wire [3:0] pwmx4;
wire [3:0] pwmx4_n;

servo_pwmx4 inst
 (
    clk,
    reset_n,
    core_en,
    prescaler,
    half_period,

    trig_rate,
    d0,
    d1,
    d2,
    d3,

    measurement_trigger,
    pwmx4,
    pwmx4_n
);

always #5 clk = ~clk;

initial begin
    clk = 0;
    reset_n = 0;
    core_en = 0;

    prescaler = 10;
    half_period = 100;

    d0 = 0;
    d1 = 0;
    d2 = 0;
    d3 = 0;
    // trig_rate = 1;
    trig_rate = 0;

    #15 reset_n = 1;
    // @(posedge clk);
    d0 = 50;
    d1 = 50;
    d2 = 50;
    d3 = 50;
//     repeat(500)
// @(posedge clk);

//         repeat(500)
//      @(posedge clk);
     core_en = 1;
     
    // repeat(500)
    //  @(posedge clk);

    // d0 = 15'h7FFF;
    // d1 = 15'h7FFF;
    // d2 = 15'h7FFF;
    // d3 = 15'h7FFF;
//     repeat(500)
//      @(posedge clk);

    // d0 = 0;
    // d1 = 0;
    // d2 = 0;
    // d3 = 0;
//     repeat(500)
// @(posedge clk);

    // d0 = 100;
    // d1 = 100;
    // d2 = 100;
    // d3 = 100;
//     repeat(500)
// @(posedge clk);

    // d0 = 99;
    // d1 = 98;
    // d2 = 0;
    // d3 = 1;

    // d0 = 102;
    // d1 = 101;
    // d2 = 99;
    // d3 = 98;

//     repeat(500)
// @(posedge clk);

    // half_period = {DWIDTH-1{1'b1}};
    // d0 = {DWIDTH-1{1'b1}};
    // d1 = {DWIDTH-1{1'b1}}-1;
    // d2 = {DWIDTH-1{1'b1}}-2;
    // d3 = 0;
end
    
endmodule