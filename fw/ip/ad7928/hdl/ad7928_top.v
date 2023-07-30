module ad7928_top
(
    clk,
    reset_n,
    core_en,
    spi_divisor,
    adc_init,

    spi_cs,
    spi_sclk,
    spi_mosi,
    spi_miso,

    filter_level,

    m_data_0,
    m_data_1,
    m_data_2,
    m_data_3,
    m_data_4,
    m_data_5,
    m_data_6,
    m_data_7,
    m_valid

);
    
parameter   CORE_FREQUENCY  =   50000000;

input  wire         clk;
input  wire         reset_n;
input  wire         core_en;

input  wire [7:0]   spi_divisor;
input  wire         adc_init;

    // spi
output wire         spi_cs;
output wire         spi_sclk;
output wire         spi_mosi;
input  wire         spi_miso;

input wire [3:0] filter_level;

// m
output wire  [11:0]  m_data_0;
output wire  [11:0]  m_data_1;
output wire  [11:0]  m_data_2;
output wire  [11:0]  m_data_3;
output wire  [11:0]  m_data_4;
output wire  [11:0]  m_data_5;
output wire  [11:0]  m_data_6;
output wire  [11:0]  m_data_7;
output wire m_valid;

wire data_valid; 
wire  [2:0]   adc_chanel;
wire  [11:0]  adc_data;

ad7928_raw
#(
    .CORE_FREQUENCY(CORE_FREQUENCY)
)
ad7928_raw_inst
(
    .clk(clk),
    .reset_n(reset_n),
    .core_en(core_en),
    .spi_divisor(spi_divisor),
    .adc_init(adc_init),

    // spi
    .spi_cs(spi_cs),
    .spi_sclk(spi_sclk),
    .spi_mosi(spi_mosi),
    .spi_miso(spi_miso),

    // result
    .m_adc_data(adc_data),
    .m_adc_chanel(adc_chanel),
    .m_axis_valid(data_valid)
   
);

ad7928_filters ad7928_filters_inst (
    .clk(clk),
    .reset_n(reset_n),
    .core_en(core_en),

    .filter_level(filter_level),

    // s
    .s_adc_data(adc_data),
    .s_adc_chanel(adc_chanel),
    .s_axis_valid(data_valid),

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
endmodule