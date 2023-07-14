module servo_pwmx4 (
    clk_10M,
    reset_n,
    core_en,
    half_period,

    d0,
    d1,
    d2,
    d3,

    measurement_trigger,
    pulse
);

input clk_10M;
input reset_n;
input core_en;
input [15:0] half_period;
input [15:0] d0;
input [15:0] d1;
input [15:0] d2;
input [15:0] d3;

output measurement_trigger;
output [3:0] pulse;

reg [15:0] pwm_cnt;
reg pwm_stage;
reg [15:0] duty0;
reg [15:0] duty1;
reg [15:0] duty2;
reg [15:0] duty3;

wire pwm_cnt_next = pwm_stage? pwm_cnt + 1'b1: pwm_cnt - 1'b1;

assign measurement_trigger = (pwm_cnt_next == half_period)? 1'b1: 1'b0;

wire pulse0, pulse1, pulse2, pulse3;

assign pulse0 = (duty0 < pwm_cnt)? 1'b1: 1'b0;
assign pulse1 = (duty1 < pwm_cnt)? 1'b1: 1'b0;
assign pulse2 = (duty2 < pwm_cnt)? 1'b1: 1'b0;
assign pulse3 = (duty3 < pwm_cnt)? 1'b1: 1'b0;
assign pulse = {pulse3, pulse2, pulse1, pulse0};

always @(posedge clk_10M or negedge reset_n) begin
    if (~reset_n) begin
        pulse <= 0;
        pwm_cnt <= 0;
        pwm_stage <= 1'b1;
        duty0 <= 0;
        duty1 <= 0;
        duty2 <= 0;
        duty3 <= 0;
    end else begin
        if (pwm_cnt > half_period) begin
            pwm_cnt <= 0;
        end else begin
            pwm_cnt <= pwm_cnt_next;
        end

        if (pwm_cnt > half_period) begin
            pwm_stage <= 1'b0;
        end else if (pwm_cnt_next == half_period) begin    
            pwm_stage <= 1'b0;
        end else if (pwm_cnt_next == 0) begin
            pwm_stage <= 1'b1;
            duty0 <= d0;
            duty1 <= d1;
            duty2 <= d2;
            duty3 <= d3;
        end
    end
end
endmodule