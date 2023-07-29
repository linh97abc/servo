`timescale 1 ns / 1 ps

module axis_uart
#(
    parameter OVERSAMPLING_RATE = 4,
    parameter [10-OVERSAMPLING_RATE:0] RX_TIMEOUT = 64, // in os_tick
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
    input  wire  [15: 0]   baud_divisor,

    // axis

    input [DATA_BIT-1:0] s_axis_tdata,
    input s_axis_tvalid,
    output s_axis_tready,

    output [DATA_BIT-1:0] m_axis_tdata,
    output m_axis_tvalid,
    input m_axis_tready,

    // status
    output [RX_FIFO_DEPTH:0] rx_cout,
    output [TX_FIFO_DEPTH:0] tx_cout,
    output [4:0] status, //tx_empty, tx_full, rx_empty, rx_full, rx_idle

    // error
    output [3:0] err, // start err, stop_err, parity_err, overrun_err


    // uart
    output wire txd,
    input wire rxd,

    // debug
    output wire dbg_os_pulse
);

wire tx_empty, tx_full, rx_empty, rx_full, rx_idle;
assign status = {tx_empty, tx_full, rx_empty, rx_full, rx_idle};

wire [DATA_BIT-1:0] tx_din;
wire tx_valid;


wire [DATA_BIT-1:0] rx_dout;
wire rx_start_err;
wire parity_err;
wire stop_err;
wire rx_done_tick;


wire overrun_err;
wire rx_buff_ready;

reg [1:0] rxd_sync;

wire tx_busy;
wire rx_busy;
reg rx_busy_last;
wire [2:0] rx_err;

assign rx_start_err = rx_err[2];
assign parity_err = rx_err[1];
assign stop_err = rx_err[0];

assign rx_done_tick = ~rx_busy & rx_busy_last;
assign overrun_err = ~(|rx_err) & ~rx_buff_ready;

assign err = {rx_start_err, stop_err, parity_err, overrun_err};

    uart#
        (
            .os_rate(OVERSAMPLING_RATE),
            .d_width(DATA_BIT),
            .parity(PARITY_BIT[1]),
            .c_parity_eo(PARITY_BIT[0])
            
        ) uart_inst
        (
            .clk(aclk),
            .reset_n(aresetn),
            .baud_pres(baud_divisor),
            // .tx_ena(tx_valid & ~tx_busy),
            .tx_ena(tx_valid),
            .tx_data(tx_din),
            .rx(rxd_sync[1]),
            .rx_busy(rx_busy),
            .rx_error(rx_err),
            .rx_data(rx_dout),
            .tx_busy(tx_busy),
            .tx(txd),
            .dbg_os_pulse(dbg_os_pulse)
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
        .r_ready(~tx_busy),

        .count_out(tx_cout),  
        .empty(tx_empty), 
        .full(tx_full)
        );


always @(posedge aclk) begin
    if(~aresetn) begin
        rxd_sync <= 2'b11;
        rx_busy_last <= 1'b0;
    end else begin
        rxd_sync <= {rxd_sync[0], rxd};
        rx_busy_last <= rx_busy;
    end
end



reg [10:0] rx_idle_cnt;

assign rx_idle = (rx_idle_cnt[10:OVERSAMPLING_RATE] == RX_TIMEOUT)? 1'b1: 1'b0;
always @(posedge aclk) begin
    if(~aresetn) begin
        rx_idle_cnt <= 0;
    end else begin
        if (rx_done_tick) begin
            rx_idle_cnt <= 0;
        end else if (~rx_idle & dbg_os_pulse) begin
            rx_idle_cnt <= rx_idle_cnt + 1'b1;
        end
    end
end
endmodule