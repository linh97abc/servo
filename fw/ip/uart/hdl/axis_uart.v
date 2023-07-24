`timescale 1 ns / 1 ps

module axis_uart
#(
    parameter [3:0] DATA_BIT  = 8, // number of bits in a word
    parameter [1:0] PARITY_BIT         = 0,                   // 00,01 <=> NONE, 11 <=> Odd, 10 <=> Even
    parameter [1:0] STOP_BIT           = 0,                    // 0 <=> 1 bit, 1 <=> 1,5 bit, 2 <=> 2 bit
    parameter RX_FIFO_DEPTH  = 4,
    parameter TX_FIFO_DEPTH  = 4
)
(
    input aclk,
    input aresetn,

    //config
    input  wire  [15: 0]                  baud_divisor,
    // input [15:0] rx_timeout,

    // axis

    input [DATA_BIT-1:0] s_axis_tdata,
    input s_axis_tvalid,
    output s_axis_tready,

    output [DATA_BIT-1:0] m_axis_tdata,
    output m_axis_tvalid,
    input m_axis_tready,

    // status
    output [RX_FIFO_DEPTH-1:0] rx_cout,
    output [TX_FIFO_DEPTH-1:0] tx_cout,
    output [4:0] status, //tx_empty, tx_full, rx_empty, rx_full, rx_idle

    // error
    output [2:0] err, // stop_err, parity_err, overrun_err


    // uart
    output wire txd,
    input wire rxd
);

wire [15:0] rx_timeout;
assign rx_timeout = {baud_divisor[11:0], 4'b0000}; // 16 baud

wire tx_empty, tx_full, rx_empty, rx_full, rx_idle;
assign status = {tx_empty, tx_full, rx_empty, rx_full, rx_idle};

wire [DATA_BIT-1:0] tx_din;
wire tx_valid, tx_start, tx_done_tick;
reg tx_not_empty;

wire [DATA_BIT+1:0] rx_dout;
wire parity_err;
wire stop_err;
wire rx_done_tick;

assign parity_err = rx_dout[DATA_BIT+1];
assign stop_err = rx_dout[DATA_BIT];

wire overrun_err;
wire rx_buff_ready;

assign overrun_err = ~parity_err & ~stop_err & rx_done_tick & ~rx_buff_ready;
assign err = {stop_err, parity_err, overrun_err};

assign tx_start = tx_valid & ~tx_not_empty;

    uart_tx
        #(
            .DATA_BIT(DATA_BIT),
            .PARITY_BIT(PARITY_BIT),
            .STOP_BIT(STOP_BIT),
            .DATA_BIT_IN(DATA_BIT)
        ) 
        uart_tx_inst
        (
        .clk(aclk),                // system clock
        .reset(~aresetn),              // system reset (active high)
        .baud_divisor(baud_divisor),       // baud_divisor
        .tx_start(tx_start),           // indicates that CPU writes new data
        .din(tx_din),                // TX Data bits
        .tx_done_tick(tx_done_tick),       // Indicate TX Controller trasmits enough data (start,datas,stop,parity)
        .tx(txd)
        );


    uart_rx#
        (
            .DATA_BIT(DATA_BIT),
            .PARITY_BIT(PARITY_BIT),
            .STOP_BIT(STOP_BIT),
            .DATA_BIT_OUT(DATA_BIT+2)                 // # data bits  (1 parity bit + 1 stop bit + 8 data bits)
        ) uart_rx_inst
        (
        .clk(aclk),                // system clock
        .reset(~aresetn),              // system reset (active high)
        .baud_divisor(baud_divisor),       // baud_divisor
        .rx(rxd),                 // rxd signal 
        .rx_done_tick(rx_done_tick),       // Indicate RX Controller receivers enough data (start,datas,stop bit)
        .dout(rx_dout)                // PARITY_ERROR, STOP_ERROR, DATA_BIT
        );

    uart_sfifo_nopipe
        #(
        .DATA_BIT(DATA_BIT),
        .ADDR_BIT(RX_FIFO_DEPTH)

        )
        rx_buff
        (
        .clk(aclk),
        .reset(~aresetn),

        .w_data(rx_dout[DATA_BIT-1:0]),
        .w_valid(~parity_err & ~stop_err & rx_done_tick),
        .w_ready(rx_buff_ready),

        .r_data(m_axis_tdata),
        .r_valid(m_axis_tvalid),
        .r_ready(m_axis_tready),

        .count_out(rx_cout),  
        .empty(rx_empty), 
        .full(rx_full)
        );

    uart_sfifo_nopipe
        #(
        .DATA_BIT(DATA_BIT),
        .ADDR_BIT(RX_FIFO_DEPTH)

        )
        tx_buff
        (
        .clk(aclk),
        .reset(~aresetn),

        .w_data(s_axis_tdata),
        .w_valid(s_axis_tvalid),
        .w_ready(s_axis_tready),

        .r_data(tx_din),
        .r_valid(tx_valid),
        .r_ready(~tx_not_empty),

        .count_out(tx_cout),  
        .empty(tx_empty), 
        .full(tx_full)
        );

always @(posedge aclk) begin
    if(~aresetn) begin
        tx_not_empty <= 1'b0;
    end else begin
        if (tx_start) begin
            tx_not_empty <= 1'b1;
        end else if (tx_done_tick) begin
            tx_not_empty <= 1'b0;
        end
    end
end

reg [15:0] rx_idle_cnt;

assign rx_idle = (rx_idle_cnt == rx_timeout)? 1'b1: 1'b0;
always @(posedge aclk) begin
    if(~aresetn) begin
        rx_idle_cnt <= 0;
    end else begin
        if (rx_done_tick) begin
            rx_idle_cnt <= 0;
        end else if (~rx_idle) begin
            rx_idle_cnt <= rx_idle_cnt + 1'b1;
        end
    end
end
endmodule