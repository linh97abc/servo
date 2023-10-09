`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////


module ad7928_raw #(
    parameter CORE_FREQUENCY = 50000000
) (
    input wire       clk,
    input wire       reset_n,
    input wire       core_en,
    input wire [7:0] spi_divisor,
    input wire       adc_init,

    // spi
    output wire spi_cs,
    output wire spi_sclk,
    output wire spi_mosi,
    input  wire spi_miso,

    // result
    output reg [11:0] m_adc_data,
    output reg [2:0] m_adc_chanel,
    output reg m_axis_valid

);

  localparam CS_IDLE_TIME = 50,  //nanosecond
  CS_TO_SCLK_TIME = 10, SCLK_TO_CS_TIME = 20;

  localparam NANOSECOND_PER_SECOND = 1000000000;

  localparam  [63:0]  CS_IDLE_CLOCKS      =   4*CS_IDLE_TIME*CORE_FREQUENCY/NANOSECOND_PER_SECOND,
                        CS_TO_SCLK_CLOCKS   =   20*CS_TO_SCLK_TIME*CORE_FREQUENCY/NANOSECOND_PER_SECOND,
                        SCLK_TO_CS_CLOCKS   =   10*SCLK_TO_CS_TIME*CORE_FREQUENCY/NANOSECOND_PER_SECOND;

  reg  [ 2:0] state_reg;
  reg         sub_state_reg;
  reg         start_send;
  reg  [15:0] send_data;
  wire [15:0] received_data;
  wire        transfer_done_tick;
  wire        transfer_ready;

  wire [ 2:0] received_channel;
  wire [11:0] received_channel_data;
  wire        recv_data_valid;

  assign recv_data_valid       = ~received_data[15];
  assign received_channel      = received_data[14:12];
  assign received_channel_data = received_data[11:0];


  localparam  STATE_IDLE              =   0,
                STATE_INIT_SEND_DUMMY_0 =   1,
                STATE_INIT_SEND_DUMMY_1 =   3,
                STATE_INIT_SEND_CONFIG  =   2,
                STATE_READ_DATA         =   6;

  localparam SUB_STATE_WAIT_TO_SEND = 0, SUB_STATE_SEND = 1;


  always @(posedge clk) begin
    if (~reset_n) begin
      state_reg    <= STATE_IDLE;
      start_send   <= 1'b0;
      send_data    <= 0;
      m_adc_data   <= 0;
      m_adc_chanel <= 0;
      m_axis_valid <= 1'b0;
    end else begin
      m_axis_valid <= 1'b0;

      case (state_reg)
        STATE_IDLE: begin
          if (adc_init & core_en) begin
            state_reg     <= STATE_INIT_SEND_DUMMY_0;
            sub_state_reg <= SUB_STATE_WAIT_TO_SEND;
          end
        end
        STATE_INIT_SEND_DUMMY_0: begin
          case (sub_state_reg)
            SUB_STATE_WAIT_TO_SEND: begin
              if (transfer_ready) begin
                sub_state_reg <= SUB_STATE_SEND;
                send_data     <= 16'hffff;
                start_send    <= 1'b1;
              end
            end
            SUB_STATE_SEND: begin
              start_send <= 1'b0;
              if (transfer_done_tick) begin
                state_reg <= STATE_INIT_SEND_DUMMY_1;
                sub_state_reg <= SUB_STATE_WAIT_TO_SEND;
              end
            end
          endcase  // sub_state_reg
        end
        STATE_INIT_SEND_DUMMY_1: begin
          case (sub_state_reg)
            SUB_STATE_WAIT_TO_SEND: begin
              if (transfer_ready) begin
                sub_state_reg <= SUB_STATE_SEND;
                send_data     <= 16'hffff;
                start_send    <= 1'b1;
              end
            end
            SUB_STATE_SEND: begin
              start_send <= 1'b0;
              if (transfer_done_tick) begin
                state_reg <= STATE_INIT_SEND_CONFIG;
                sub_state_reg <= SUB_STATE_WAIT_TO_SEND;
              end
            end
          endcase  // sub_state_reg
        end
        STATE_INIT_SEND_CONFIG: begin
          case (sub_state_reg)
            SUB_STATE_WAIT_TO_SEND: begin
              if (transfer_ready) begin
                sub_state_reg <= SUB_STATE_SEND;
                send_data     <= 16'hDF90;
                start_send    <= 1'b1;
              end
            end
            SUB_STATE_SEND: begin
              start_send <= 1'b0;
              if (transfer_done_tick) begin
                state_reg <= STATE_READ_DATA;
                sub_state_reg <= SUB_STATE_WAIT_TO_SEND;
              end
            end
          endcase  // sub_state_reg
        end
        STATE_READ_DATA: begin
          case (sub_state_reg)
            SUB_STATE_WAIT_TO_SEND: begin
              if (transfer_ready & core_en) begin
                sub_state_reg <= SUB_STATE_SEND;
                send_data     <= 16'h0000;
                start_send    <= 1'b1;
              end
            end
            SUB_STATE_SEND: begin
              start_send <= 1'b0;
              if (transfer_done_tick) begin
                state_reg     <= STATE_READ_DATA;
                sub_state_reg <= SUB_STATE_WAIT_TO_SEND;

                m_adc_data    <= received_channel_data;
                m_adc_chanel  <= received_channel;
                m_axis_valid  <= recv_data_valid;

              end
            end
          endcase  // sub_state_reg
        end
      endcase  // state_reg
    end
  end


  ad7928_spi #(
      .MAX_NUM_DATA_BITS(16)
  ) spi_inst (
      .clk               (clk),
      .reset_n           (reset_n),
      .divisor           (spi_divisor),
      .cs_idle_clocks    (CS_IDLE_CLOCKS),
      .cs_to_sclk_clocks (CS_TO_SCLK_CLOCKS),
      .sclk_to_cs_clocks (SCLK_TO_CS_CLOCKS),
      .start_send        (start_send),
      .num_data_bits     (16),
      .send_data         (send_data),
      .received_data     (received_data),
      .transfer_done_tick(transfer_done_tick),
      .transfer_ready    (transfer_ready),
      .spi_cs            (spi_cs),
      .spi_sclk          (spi_sclk),
      .spi_mosi          (spi_mosi),
      .spi_miso          (spi_miso)
  );

endmodule
