`timescale 1 ns / 1 ps

module avl_fifo_uart
#(
    parameter FREQ_CLK  = 100_000_000,
    parameter BAUDRATE  = 921600,
    // parameter [15:0] RX_TIMEOUT_US = 500,
    parameter C_RX_THRESHOLD  = 32,
    parameter DATA_BIT  = 8, // number of bits in a word
    parameter [1:0] PARITY_BIT         = 0,                   // 00,01 <=> NONE, 11 <=> Odd, 10 <=> Even
    parameter [1:0] STOP_BIT           = 0,                    // 0 <=> 1 bit, 1 <=> 1,5 bit, 2 <=> 2 bit
    parameter RX_FIFO_DEPTH  = 4,
    parameter TX_FIFO_DEPTH  = 4
)
(
    //clk, reset
    input wire clk,
    input wire reset_n,
    
    //avalon mm interface
    input wire [3:0] address,
    input wire [31:0] writedata,
    input wire write_n,
    input wire read_n,    
    output reg [31:0] readdata,  

    // uart
    input wire rxd,
    output wire txd,

    //irq
    output wire irq
);

    // localparam [15:0] RX_TIMEOUT_PRES = (FREQ_CLK/1000_000) * RX_TIMEOUT_US;
    localparam [15:0] BAUD_PRES = (FREQ_CLK/BAUDRATE) - 1;

    localparam CR_REG = 0;
    localparam IE_REG = 1;
    localparam IE_DIS_REG = 2;
    localparam FLAG_REG = 3;

    localparam TX_COUNT_REG = 4;
    localparam RX_COUNT_REG = 5;

    localparam RX_THRESHOLD_REG = 6;
    localparam TX_REG = 7;
    localparam RX_REG = 10;



// | 23 .. 8 |7..2       |1   |  0 |
// baudrate, reserved,   reset, en
wire [31:0] control_reg; 
reg [15:0] baudrate;
reg core_en;
reg core_reset;
assign control_reg = {baudrate, 7'b0, core_en};

wire tx_ready, rx_valid;
wire [DATA_BIT-1:0] rx_data;

reg [RX_FIFO_DEPTH-1:0] rx_threshold_cnt;
wire rx_threshold;

wire [RX_FIFO_DEPTH-1:0] rx_cout;
wire [TX_FIFO_DEPTH-1:0] tx_cout;

wire [4:0] uart_status;
wire [2:0] uart_err;
reg [2:1] uart_err_reg;

wire [10:0] core_status;
assign rx_threshold = ((rx_threshold_cnt <= rx_cout) || (uart_status[0] & rx_valid))? 1'b1: 1'b0;
assign core_status = {uart_err_reg, tx_ready, rx_valid, rx_threshold, uart_status};

reg [10:0] ie;

assign irq = |(ie & core_status);



axis_uart
#(
.DATA_BIT(DATA_BIT),
.PARITY_BIT(PARITY_BIT),
.STOP_BIT(STOP_BIT),
.RX_FIFO_DEPTH(RX_FIFO_DEPTH),
.TX_FIFO_DEPTH(TX_FIFO_DEPTH)
)
axis_uart_inst
(
    .aclk(clk),
    .aresetn(reset_n & ~core_reset),

    //config
    .baud_divisor(baudrate),
    // .rx_timeout(RX_TIMEOUT_PRES),

    // axis

    .s_axis_tdata(writedata[DATA_BIT-1:0]),
    .s_axis_tvalid(core_en & ~write_n & (address == TX_REG)),
    .s_axis_tready(tx_ready),

    .m_axis_tdata(rx_data),
    .m_axis_tvalid(rx_valid),
    .m_axis_tready(core_en & ~read_n & (address == RX_REG)),

    // status
    .rx_cout(rx_cout),
    .tx_cout(tx_cout),
    .status(uart_status), //tx_empty, tx_full, rx_empty, rx_full, rx_idle

    // error
    .err(uart_err), // stop_err, parity_err, overrun_err


    // uart
    .txd(txd),
    .rxd(rxd)
);

    always @(posedge clk or negedge reset_n)             //write down to reg
    begin
       if(~reset_n) begin
            uart_err_reg <= 0;
       end
       else begin


        uart_err_reg <= uart_err | 
            (
                uart_err_reg &  ((~write_n && (address == FLAG_REG))? ~writedata[10:8]: 3'b111)
                );
        end
    end

 //avalon write data 
    always @(posedge clk or negedge reset_n)             //write down to reg
    begin
        if(~reset_n) begin
            rx_threshold_cnt <= C_RX_THRESHOLD;
            core_reset <= 1'b0;
            core_en <= 1'b1;
            baudrate <= BAUD_PRES;
            ie <= 0;
        end
        else begin 
            core_reset <= (address == CR_REG)? (~write_n & writedata[1]): 1'b0;

            if(~write_n) begin
                case (address)
                CR_REG: begin
                    core_en <= writedata[0];
                    if(writedata[0] | ~core_en) begin
                        baudrate       <= writedata[23:8];
                    end
                end
                RX_THRESHOLD_REG: rx_threshold_cnt <= writedata;
                IE_REG: ie <= ie | writedata[10:0];
                IE_DIS_REG: ie <= ie & ~writedata[10:0]; 


                endcase 
            end
            
        end
    end
    
    //avalon read data
    always @(*)            //read reg value      
    begin
        if(~read_n) begin
            case (address)
                CR_REG: readdata <= control_reg;
                RX_REG: readdata <= {rx_valid, uart_status[2], {30-DATA_BIT{1'b0}}, rx_data};
                FLAG_REG: readdata <= core_status;
                TX_COUNT_REG: readdata <= tx_cout;
                RX_COUNT_REG: readdata <= rx_cout;
                RX_THRESHOLD_REG: readdata <= rx_threshold_cnt;
                IE_REG: readdata <= ie;
                default: readdata <= 0;
            endcase
        end

    end    
endmodule