module servo_pwmx4
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

parameter DWIDTH = 15;

input clk;
input reset_n;
input core_en;
input [DWIDTH-1:0] prescaler;
input [DWIDTH-1:0] half_period;
input [5:0] trig_rate;
input [DWIDTH-1:0] d0;
input [DWIDTH-1:0] d1;
input [DWIDTH-1:0] d2;
input [DWIDTH-1:0] d3;


output reg measurement_trigger;
output reg [3:0] pwmx4;
output reg [3:0] pwmx4_n;

reg [DWIDTH-1:0] pwm_cnt;
reg pwm_stage;
reg [DWIDTH-1:0] reg_duty0;
reg [DWIDTH-1:0] reg_duty1;
reg [DWIDTH-1:0] reg_duty2;
reg [DWIDTH-1:0] reg_duty3;

reg [7:0] clk_cnt;
assign clk_pwm = (clk_cnt == prescaler)? 1'b1: 1'b0;


wire ovverun;
reg ovverun_last;
wire [DWIDTH-1:0] pwm_cnt_next;
assign ovverun = (pwm_cnt_next == half_period)? 1'b1: 1'b0;


reg [5:0] trig_level_cnt;

always @(posedge clk) begin
    if (~reset_n) begin
        clk_cnt <= 0;
        measurement_trigger <= 1'b0;
        ovverun_last <= 1'b0;
        trig_level_cnt <= 0;
    end else begin
        ovverun_last <= ovverun;

        if (trig_rate == 0) begin
            measurement_trigger <= ovverun & ~ovverun_last;
        end else begin
            if (trig_level_cnt == trig_rate) begin
                trig_level_cnt <= 0;
                measurement_trigger <= 1'b1;
            end else begin
                measurement_trigger <= 1'b0;
                if (ovverun & ~ovverun_last) begin
                        trig_level_cnt <= trig_level_cnt + 1'b1;
                end
            end
            
        end




        if(core_en) begin
            if (clk_cnt == prescaler) begin
                clk_cnt <= 0;
            end else begin
                clk_cnt <= clk_cnt + 1'b1;
            end
        end
    end
end

assign pwm_cnt_next = pwm_stage? pwm_cnt + 1'b1: pwm_cnt - 1'b1;


wire pulse0, pulse1, pulse2, pulse3;

assign pulse0 = (reg_duty0  > pwm_cnt)? core_en: 1'b0;
assign pulse1 = (reg_duty1  > pwm_cnt)? core_en: 1'b0;
assign pulse2 = (reg_duty2  > pwm_cnt)? core_en: 1'b0;
assign pulse3 = (reg_duty3  > pwm_cnt)? core_en: 1'b0;


always @(posedge clk or negedge reset_n) begin
    if (~reset_n) begin
        pwmx4 <= 0;
        pwmx4_n <= 0;
    end else begin
        if (clk_pwm) begin
            pwmx4 <= {pulse3, pulse2, pulse1, pulse0} & ~pwmx4_n;
            pwmx4_n <= ~{pulse3, pulse2, pulse1, pulse0} & ~pwmx4;
            
        end
    end
end

always @(posedge clk or negedge reset_n) begin
    if (~reset_n) begin
        pwm_cnt <= 0;
        pwm_stage <= 1'b1;
        reg_duty0 <= 0;
        reg_duty1 <= 0;
        reg_duty2 <= 0;
        reg_duty3 <= 0;
    end else begin
        if(core_en) begin
            
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
end
endmodule