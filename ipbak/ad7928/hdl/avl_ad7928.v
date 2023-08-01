module avl_ad7928 (
    //common
    clk,
    reset_n,

    address,
    writedata,
    write_n,
    read_n,    
    readdata,  

    // spi
    spi_cs,
    spi_sclk,
    spi_mosi,
    spi_miso,

    irq
);
    
parameter FREQ_CLK = 100_000_000;
parameter SPI_SPEED = 1_000_000;

localparam SPI_PRESCALER = FREQ_CLK/SPI_SPEED-1;

    //avalon mm interface
input wire [5:0] address;
input wire [31:0] writedata;
input wire write_n;
input wire read_n;    
output reg [31:0] readdata;  
output irq;


    // spi
output wire         spi_cs;
output wire         spi_sclk;
output wire         spi_mosi;
input  wire         spi_miso;

reg         core_en;
reg [7:0]   spi_divisor;
reg         adc_init;
reg [3:0] filter_level;

// m
wire  [ADC_WIDTH-1:0]  i0;
wire  [ADC_WIDTH-1:0]  i1;
wire  [ADC_WIDTH-1:0]  i2;
wire  [ADC_WIDTH-1:0]  i3;
wire  [ADC_WIDTH-1:0]  pos0;
wire  [ADC_WIDTH-1:0]  pos1;
wire  [ADC_WIDTH-1:0]  pos2;
wire  [ADC_WIDTH-1:0]  pos3;
wire adc_data_valid;

reg [ADC_WIDTH-2:0]    i0_max;
reg [ADC_WIDTH-2:0]    i1_max;
reg [ADC_WIDTH-2:0]    i2_max;
reg [ADC_WIDTH-2:0]    i3_max;


ad7928_top #(
    .CORE_FREQUENCY(FREQ_CLK)
)
 measure_inst
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

    .m_data_0(i0),
    .m_data_1(i1),
    .m_data_2(i2),
    .m_data_3(i3),
    .m_data_4(pos0),
    .m_data_5(pos1),
    .m_data_6(pos2),
    .m_data_7(pos3),
    .m_valid(adc_data_valid)  

);


localparam CR_EN_BIT = 0,
    CR_FILTER_LEVEL_BIT = 1,
    CR_PROTECT_EN_BIT = 5,
    CR_DRV_EN_BIT = 9;

localparam TR_U_VALID_BIT = 0,
    TR_ADC_INIT_BIT = 1,
    TR_START_SERVO_BIT = 2,
    TR_RESET_BIT = 6;

localparam 
    FLAG_DRV8320_FAULT_BIT = 7,
    FLAG_STOP_BIT = 3,
    FLAG_ADC_VALID_BIT = 2,
    FLAG_MEA_TRIG_BIT = 1,
    FLAG_REAL_TIME_BIT = 0;

localparam CR_OFFSET = 0, // protect_en , filter_level, en
        TR_OFFSET = 1, // reset, start_servo, adc_init, u_valid
        IE_OFFSET = 2,
        FLAG_OFFSET = 3,

        SPI_PRES_OFFSET = 4,

        I0_MAX_OFFSET = 15,
        I1_MAX_OFFSET = I0_MAX_OFFSET+1,
        I2_MAX_OFFSET = I0_MAX_OFFSET+2,
        I3_MAX_OFFSET = I0_MAX_OFFSET+3,

        I0_OFFSET = 19,
        I1_OFFSET = I0_OFFSET+1,
        I2_OFFSET = I0_OFFSET+2,
        I3_OFFSET = I0_OFFSET+3,

        POS0_OFFSET = 23,
        POS1_OFFSET = POS0_OFFSET+1,
        POS2_OFFSET = POS0_OFFSET+2,
        POS3_OFFSET = POS0_OFFSET+3;

//avalon write data 
    always @(posedge clk or negedge reset_n)             //write down to reg
    begin
        if(~reset_n) begin
            core_reset <= 1'b0;
            core_en <= 1'b0;
            spi_divisor <= SPI_PRESCALER;
            adc_init <= 1'b0;
            filter_level <= 1'b0;
            protected_en <= {SERVO_NUM{1'b1}};
            ie <= 0;
        end
        else begin 
            core_reset <= ((address == TR_OFFSET) && ~write_n)? writedata[TR_RESET_BIT]: 1'b0;
            adc_init <= ((address == TR_OFFSET) && ~write_n)? writedata[TR_ADC_INIT_BIT]: 1'b0;

            if(~write_n) begin
                case (address)
                CR_OFFSET: begin
                    core_en <= writedata[CR_EN_BIT];
                    if(~writedata[0] | ~core_en) begin
                        protected_en <= writedata[CR_PROTECT_EN_BIT+SERVO_NUM-1:CR_PROTECT_EN_BIT];
                        filter_level <= writedata[CR_FILTER_LEVEL_BIT+3:CR_FILTER_LEVEL_BIT];
                    end
                end
                IE_OFFSET: ie <= writedata;
                SPI_PRES_OFFSET: begin
                   if (core_en) begin
                        spi_divisor <= writedata;
                   end 
                end 

                I0_MAX_OFFSET: begin
                   if (core_en) begin
                        i0_max <= writedata[14:16-ADC_WIDTH];
                   end 
                end
                I1_MAX_OFFSET: begin
                   if (core_en) begin
                        i1_max <= writedata[14:16-ADC_WIDTH];
                   end 
                end
                I2_MAX_OFFSET: begin
                   if (core_en) begin
                        i2_max <= writedata[14:16-ADC_WIDTH];
                   end 
                end
                I3_MAX_OFFSET: begin
                   if (core_en) begin
                        i3_max <= writedata[14:16-ADC_WIDTH];
                   end 
                end


                endcase 
            end
            
        end
    end
    
    //avalon read data
    always @(posedge clk or negedge reset_n)            //read reg value      
    begin
        if(~reset_n) begin
            readdata <= 0;
        end
        else begin
            if(~read_n) begin
                case (address)
                    CR_OFFSET: readdata <= {drv8320_en, protected_en, filter_level, core_en};
                    IE_OFFSET: readdata <= ie;
                    FLAG_OFFSET: readdata <= flag;
                    SPI_PRES_OFFSET: readdata <= spi_divisor;
                    I0_MAX_OFFSET: readdata <= {i0_max, {16-ADC_WIDTH{1'b0}}};
                    I1_MAX_OFFSET: readdata <= {i1_max, {16-ADC_WIDTH{1'b0}}};
                    I2_MAX_OFFSET: readdata <= {i2_max, {16-ADC_WIDTH{1'b0}}};
                    I3_MAX_OFFSET: readdata <= {i3_max, {16-ADC_WIDTH{1'b0}}};
                    I0_OFFSET: readdata <= {i0, {16-ADC_WIDTH{1'b0}}};
                    I1_OFFSET: readdata <= {i1, {16-ADC_WIDTH{1'b0}}};
                    I2_OFFSET: readdata <= {i2, {16-ADC_WIDTH{1'b0}}};
                    I3_OFFSET: readdata <= {i3, {16-ADC_WIDTH{1'b0}}};
                    POS0_OFFSET: readdata <= {pos0, {16-ADC_WIDTH{1'b0}}};
                    POS1_OFFSET: readdata <= {pos1, {16-ADC_WIDTH{1'b0}}};
                    POS2_OFFSET: readdata <= {pos2, {16-ADC_WIDTH{1'b0}}};
                    POS3_OFFSET: readdata <= {pos3, {16-ADC_WIDTH{1'b0}}};

                    default: readdata <= 0;
                endcase
            end
        end
    end   

endmodule