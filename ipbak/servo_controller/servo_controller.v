module servo_controller (
    clk,
    reset_n,
    core_en,

    spi_divisor,
    adc_init,
    spi_cs,
    spi_sclk,
    spi_mosi,
    spi_miso,

    servo0_cfg, //c0, c1, c2, sp
    servo1_cfg,
    servo2_cfg,
    servo3_cfg,

    half_period,



);

localparam DUTY_WIDTH = 12;

reg [DUTY_WIDTH-1:0] half_period;

wire [DUTY_WIDTH-1:0] d0;
wire [DUTY_WIDTH-1:0] d1;
wire [DUTY_WIDTH-1:0] d2;
wire [DUTY_WIDTH-1:0] d3;

wire measurement_trigger;
wire [3:0] pwm_pulse;
wire [3:0] filter_level;


wire [63:0] ap_return;

assign d0 = ap_return[11:0];
assign d1 = ap_return[27:16];
assign d2 = ap_return[43:32];
assign d3 = ap_return[59:48];

always @(posedge clk) begin
    if (current0 > current0_max) begin
    end
    if (current1 > current1_max) begin
    end
    if (current2 > current2_max) begin
    end
    if (current3 > current3_max) begin
    end
end

always @(posedge clk) begin
    if (~reset_n) begin
        
    end else begin
        if(ap_done) begin
            d0 <= ap_return[11:0];
            d1 <= ap_return[27:16];
            d2 <= ap_return[43:32];
            d3 <= ap_return[59:48];
        end
    end
end

servo_pwmx4 #(.CORE_FREQUENCY())
 servo_pwmx4_inst(
    .clk(clk),
    .reset_n(reset_n),
    .core_en(adc_data_valid),
    .half_period(half_period),

    .d0(d0),
    .d1(d1),
    .d2(d2),
    .d3(d3),

    .measurement_trigger(measurement_trigger),
    .pwm_pulse(pwm_pulse)
);


ad7928_top# (.CORE_FREQUENCY())
ad7928_top_inst
(
    .clk(clk),
    .reset_n(reset_n),
    .core_en(core_en),
    .spi_divisor(spi_divisor),
    .adc_init(adc_init),

    .spi_cs(spi_cs),
    .spi_sclk(spi_sclk),
    .spi_mosi(spi_mosi),
    .spi_miso(spi_miso),

    .filter_level(filter_level),

    .m_data_0(position_0),
    .m_data_1(position_1),
    .m_data_2(position_2),
    .m_data_3(position_3),
    .m_data_4(current0),
    .m_data_5(current1),
    .m_data_6(current2),
    .m_data_7(current3),
    .m_valid(adc_data_valid)

);

servo4x servo4x_inst (
        .ap_clk(clk),
        .ap_rst(~reset_n),
        .ap_start(measurement_trigger),
        .ap_done(ap_done),
        .ap_idle(),
        .ap_ready(),
        .s0({
            4'b0, servo0_sp,
            4'b0, position_0,
            servo0_c2, servo0_c1, servo0_c0,
            4'b0, half_period
            }),
        .s1({
            4'b0, servo1_sp,
            4'b0, position_1,
            servo1_c2, servo1_c1, servo1_c0,
            4'b0, half_period
            }),
        .s2({
            4'b0, servo2_sp,
            4'b0, position_2,
            servo2_c2, servo2_c1, servo2_c0,
            4'b0, half_period
            }),
        .s3({
            4'b0, servo3_sp,
            4'b0, position_3,
            servo3_c2, servo3_c1, servo3_c0,
            4'b0, half_period
            }),
        .ap_return(ap_return)
);
endmodule