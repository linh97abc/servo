module tb_uart;

    parameter OVERSAMPLING_RATE = 4;
    parameter [3:0] DATA_BIT  = 8; // number of bits in a word
    parameter [1:0] PARITY_BIT         = 0;                   // 00,01 <=> NONE, 11 <=> Odd, 10 <=> Even


reg [15:0] baud_divisor;
reg clk, reset_n;
reg tx_valid;
reg [7:0] tx_din;
wire txd;

     uart#
        (
            .os_rate(OVERSAMPLING_RATE),
            .d_width(DATA_BIT),
            .parity(PARITY_BIT[1]),
            .c_parity_eo(PARITY_BIT[0])
            
        ) uart_inst
        (
            .clk(clk),
            .reset_n(reset_n),
            .baud_pres(baud_divisor),
            .tx_ena(tx_valid),
            .tx_data(tx_din),
            .rx(txd),
            .rx_busy(),
            .rx_error(),
            .rx_data(),
            .tx_busy(),
            .tx(txd),
            .dbg_os_pulse()
        );

always #5 clk = ~clk;

initial begin
    clk = 0;
    reset_n = 0;
// baud_divisor = 108;
baud_divisor = 111;
tx_valid = 1;
tx_din = 5;
    #15
    reset_n = 1;

    @(posedge clk);
end
endmodule