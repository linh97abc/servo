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
input    s_valid;

output reg  [DUTY_WIDTH-2:0]  duty0;
output reg  [DUTY_WIDTH-2:0]  duty1;
output reg  [DUTY_WIDTH-2:0]  duty2;
output reg  [DUTY_WIDTH-2:0]  duty3;
output reg [3:0] direction;

wire  [95:0] ap_return;



servo_u_to_duty 
servo_u_to_duty_inst (
        .ap_clk(clk),
        .ap_rst(~reset_n),
        .ap_start(s_valid),
        .ap_done(),
        .ap_idle(),
        .ap_ready(),
        .half_period({1'b0, half_period}),
        .in_r({u3, u2, u1, u0}),
        .ap_return(ap_return)
);

always @(posedge clk) begin
    if (~reset_n) begin
        duty0 <= 0;
        duty1 <= 0;
        duty2 <= 0;
        duty3 <= 0;
        direction <= 0;
    end else begin
        duty0 <= ap_return[14:0];
        duty1 <= ap_return[30:16];
        duty2 <= ap_return[46:32];
        duty3 <= ap_return[62:48];
        direction <= {ap_return[88], ap_return[80], ap_return[72], ap_return[64]};
    end
end

endmodule