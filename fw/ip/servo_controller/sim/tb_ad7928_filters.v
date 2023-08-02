`timescale 1ns / 1ps

module tb_ad7928_filter;

reg           clk;
reg           reset_n;
reg           core_en;

reg  [3:0] filter_level;

// s
reg   [11:0]  s_adc_data;
reg   [2:0]   s_adc_chanel;
reg  s_axis_valid;

// m
wire  [11:0]  m_data_0;
wire  [11:0]  m_data_1;
wire  [11:0]  m_data_2;
wire  [11:0]  m_data_3;
wire  [11:0]  m_data_4;
wire  [11:0]  m_data_5;
wire  [11:0]  m_data_6;
wire  [11:0]  m_data_7;
wire m_valid;


ad7928_filters inst (
    .clk(clk),
    .reset_n(reset_n),
    .core_en(core_en),

    .filter_level(filter_level),

    // s
    .s_adc_data(s_adc_data),
    .s_adc_chanel(s_adc_chanel),
    .s_axis_valid(s_axis_valid),

    // m
    .m_data_0(m_data_0),
    .m_data_1(m_data_1),
    .m_data_2(m_data_2),
    .m_data_3(m_data_3),
    .m_data_4(m_data_4),
    .m_data_5(m_data_5),
    .m_data_6(m_data_6),
    .m_data_7(m_data_7),
    .m_valid(m_valid)
);



always #5 clk = ~clk;


task push_data;
input [11:0] data;
input [2:0]   chanel;
begin
    @(posedge clk);
    @(posedge clk);
    #1 s_axis_valid = 1;
    s_adc_data = data;
    s_adc_chanel = chanel;

    @(posedge clk);
    #1 s_axis_valid = 0;
end
endtask

integer testdata;

initial begin
    clk = 0;
    reset_n = 0;
    core_en = 1;
    filter_level = 2;
    s_adc_data = 10;
    s_adc_chanel = 0;
    s_axis_valid = 0;
    #15 reset_n = 1;
    @(posedge clk);

    // testdata = 0;

    // repeat (10) begin
    //     push_data(testdata, 0);
    //     push_data(testdata, 1);
    //     push_data(testdata, 2);
    //     push_data(testdata, 3);
    //     push_data(testdata, 4);
    //     push_data(testdata, 5);
    //     push_data(testdata, 6);
    //     push_data(testdata, 7);

    //     testdata = testdata + 10;
    // end

    // repeat (10) begin
    //     push_data(100, 0);
    //     push_data(100, 1);
    //     push_data(100, 2);
    //     push_data(100, 3);
    //     push_data(100, 4);
    //     push_data(100, 5);
    //     push_data(100, 6);
    //     push_data(100, 7);

    //     push_data('hFFFF9C, 0);
    //     push_data('hFFFF9C, 1);
    //     push_data('hFFFF9C, 2);
    //     push_data('hFFFF9C, 3);
    //     push_data('hFFFF9C, 4);
    //     push_data('hFFFF9C, 5);
    //     push_data('hFFFF9C, 6);
    //     push_data('hFFFF9C, 7);

    // end

    repeat (10) begin
        push_data(50, 0);
        push_data(50, 1);
        push_data(50, 2);
        push_data(50, 3);
        push_data(50, 4);
        push_data(50, 5);
        push_data(50, 6);
        push_data(50, 7);

        push_data('hFFFF9C, 0);
        push_data('hFFFF9C, 1);
        push_data('hFFFF9C, 2);
        push_data('hFFFF9C, 3);
        push_data('hFFFF9C, 4);
        push_data('hFFFF9C, 5);
        push_data('hFFFF9C, 6);
        push_data('hFFFF9C, 7);

    end


end

endmodule