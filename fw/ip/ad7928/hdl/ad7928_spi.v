`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////


module ad7928_spi
    #(
        parameter MAX_NUM_DATA_BITS   =   24
    )
    (
        input  wire         clk,
        input  wire         reset_n,
        input  wire [7:0]   divisor,
        input  wire [7:0]   cs_idle_clocks,
        input  wire [7:0]   cs_to_sclk_clocks,
        input  wire [7:0]   sclk_to_cs_clocks,
        input  wire         start_send,
        input  wire [4:0]   num_data_bits,
        input  wire [MAX_NUM_DATA_BITS-1:0]     send_data,
        output wire [MAX_NUM_DATA_BITS-1:0]     received_data,
        output reg          transfer_done_tick,
        output reg          transfer_ready,
        output reg          spi_cs,
        output reg          spi_sclk,
        output wire         spi_mosi,
        input  wire         spi_miso
    );

    reg     [2:0]   state_reg;
    reg     [MAX_NUM_DATA_BITS-1:0] send_data_reg;
    reg     [MAX_NUM_DATA_BITS:0]   send_data_shift_reg;
    reg     [MAX_NUM_DATA_BITS-1:0] received_data_shift_reg;
    reg     [31:0]  clock_counter;
    reg             send_command_pending;

    localparam      STATE_IDLE      =   0,
                    STATE_SCLK_0    =   1,
                    STATE_SCLK_1    =   2;

    always @(posedge clk)
    begin
        if (~reset_n)
        begin
            state_reg   <=  STATE_IDLE;
            transfer_done_tick      <=  1'b0;
            transfer_ready          <=  1'b0;
            send_data_shift_reg     <=  0;
            clock_counter           <=  cs_idle_clocks;
            spi_cs                  <=  1'b1;
            spi_sclk                <=  1'b1;
            send_command_pending    <=  1'b0;
        end
        else
        begin
            if (clock_counter > 0)
            begin
                clock_counter   <=  clock_counter - 1'b1;
            end

            if (start_send)
            begin
                send_command_pending    <=  1'b1;
                send_data_reg           <=  send_data;
            end

            case (state_reg)
                STATE_IDLE:
                begin
                    if (clock_counter == 0)
                    begin
                        transfer_ready  <=  1'b1;
                    end

                    spi_sclk            <=  1'b1;
                    transfer_done_tick  <=  1'b0;

                    if (send_command_pending & (clock_counter == 0) & spi_cs)
                    begin
                        received_data_shift_reg     <=  0;
                        send_data_shift_reg         <=  (1 << (MAX_NUM_DATA_BITS - num_data_bits))|({send_data_reg, 1'b0});
                        clock_counter               <=  cs_to_sclk_clocks - 1'b1;
                        spi_cs                      <=  1'b0;
                    end
                    else if (send_command_pending & (clock_counter == 0) & ~spi_cs)
                    begin
                        state_reg   <=  STATE_SCLK_0;
                        spi_sclk    <=  1'b0;
                        received_data_shift_reg     <=  {received_data_shift_reg[MAX_NUM_DATA_BITS-2:0], spi_miso};
                        clock_counter   <=  divisor;
                        send_command_pending    <=  1'b0;
                    end
                end
                STATE_SCLK_0:
                begin
                    if (clock_counter == 0)
                    begin
                        if (send_data_shift_reg[MAX_NUM_DATA_BITS-1:0] != {MAX_NUM_DATA_BITS{1'b0}})
                        begin
                            send_data_shift_reg     <=  {send_data_shift_reg[MAX_NUM_DATA_BITS-1:0], 1'b0};
                            clock_counter           <=  divisor;
                            spi_sclk                <=  1'b1;
                            state_reg               <=  STATE_SCLK_1;
                        end
                        else
                        begin
                            clock_counter           <=  sclk_to_cs_clocks - 1'b1;
                            spi_sclk                <=  1'b1;
                            state_reg               <=  STATE_SCLK_1;
                        end
                    end
                end
                STATE_SCLK_1:
                begin
                    if (clock_counter == 0)
                    begin
                        if (send_data_shift_reg[MAX_NUM_DATA_BITS-1:0] != {MAX_NUM_DATA_BITS{1'b0}})
                        begin
                            received_data_shift_reg     <=  {received_data_shift_reg[MAX_NUM_DATA_BITS-2:0], spi_miso};
                            clock_counter           <=  divisor;
                            spi_sclk                <=  1'b0;
                            state_reg               <=  STATE_SCLK_0;
                        end
                        else
                        begin
                            transfer_done_tick      <=  1'b1;
                            clock_counter           <=  cs_idle_clocks - 1'b1;
                            spi_cs                  <=  1'b1;
                            state_reg               <=  STATE_IDLE;
                        end
                    end
                end
            endcase // state_reg
        end
    end

    assign  spi_mosi        =   send_data_shift_reg[MAX_NUM_DATA_BITS];
    assign  received_data   =   received_data_shift_reg;   
    
    

endmodule