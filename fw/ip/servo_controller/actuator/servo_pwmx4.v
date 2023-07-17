module servo_pwmx4
 (
    clk,
    reset_n,
    core_en,
    prescaler,
    half_period,

    d0,
    d1,
    d2,
    d3,

    measurement_trigger,
    pwmx4,
    pwmx4_n
);

parameter [7:0] DEAD_TIME = 1;
parameter DWIDTH = 15;

input clk;
input reset_n;
input core_en;
input [DWIDTH-1:0] prescaler;
input [DWIDTH-1:0] half_period;
input [DWIDTH-1:0] d0;
input [DWIDTH-1:0] d1;
input [DWIDTH-1:0] d2;
input [DWIDTH-1:0] d3;

output measurement_trigger;
output [3:0] pwmx4;
output [3:0] pwmx4_n;

reg [DWIDTH-1:0] pwm_cnt;
reg pwm_stage;
reg [DWIDTH-1:0] reg_duty0;
reg [DWIDTH-1:0] reg_duty1;
reg [DWIDTH-1:0] reg_duty2;
reg [DWIDTH-1:0] reg_duty3;

reg [7:0] clk_cnt;
assign clk_pwm = (clk_cnt == prescaler)? 1'b1: 1'b0;

reg ovverun_last;
wire ovverun;
assign ovverun = (pwm_cnt_next == half_period)? 1'b1: 1'b0;
assign measurement_trigger = ovverun & ~ovverun_last;

always @(posedge clk) begin
    if (~reset_n) begin
        clk_cnt <= 0;
    end else begin
        ovverun_last <= ovverun;
        if (clk_cnt == prescaler) begin
            clk_cnt <= 0;
        end else begin
            clk_cnt <= clk_cnt + 1'b1;
        end
    end
end

wire pwm_cnt_next = pwm_stage? pwm_cnt + 1'b1: pwm_cnt - 1'b1;


wire pulse0, pulse1, pulse2, pulse3;

assign pulse0 = (reg_duty0 < pwm_cnt)? 1'b1: 1'b0;
assign pulse1 = (reg_duty1 < pwm_cnt)? 1'b1: 1'b0;
assign pulse2 = (reg_duty2 < pwm_cnt)? 1'b1: 1'b0;
assign pulse3 = (reg_duty3 < pwm_cnt)? 1'b1: 1'b0;

assign pulse0_n = ((reg_duty0 + DEAD_TIME) < pwm_cnt)? 1'b0: 1'b1;
assign pulse1_n = ((reg_duty1 + DEAD_TIME) < pwm_cnt)? 1'b0: 1'b1;
assign pulse2_n = ((reg_duty2 + DEAD_TIME) < pwm_cnt)? 1'b0: 1'b1;
assign pulse3_n = ((reg_duty3 + DEAD_TIME) < pwm_cnt)? 1'b0: 1'b1;

assign pwmx4 = {pulse3, pulse2, pulse1, pulse0};
assign pwmx4_n = {pulse0_n, pulse1_n, pulse2_n, pulse3_n};

always @(posedge clk or negedge reset_n) begin
    if (~reset_n) begin
        pwm_cnt <= 0;
        pwm_stage <= 1'b1;
        reg_duty0 <= 0;
        reg_duty1 <= 0;
        reg_duty2 <= 0;
        reg_duty3 <= 0;
    end else if(core_en) begin
        if (clk_pwm) begin
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
                reg_duty0 <= d0;
                reg_duty1 <= d1;
                reg_duty2 <= d2;
                reg_duty3 <= d3;
            end
            
        end
    end
end
endmodule