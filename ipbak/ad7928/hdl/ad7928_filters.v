module ad7928_filters (
    input  wire         clk,
    input  wire         reset_n,
    input  wire         core_en,

    input wire [3:0] filter_level,

    // s
    input wire  [11:0]  s_adc_data,
    input wire  [2:0]   s_adc_chanel,
    input wire [15:0]  s_axis_valid,

    // m
    output wire  [11:0]  m_data_0,
    output wire  [11:0]  m_data_1,
    output wire  [11:0]  m_data_2,
    output wire  [11:0]  m_data_3,
    output wire  [11:0]  m_data_4,
    output wire  [11:0]  m_data_5,
    output wire  [11:0]  m_data_6,
    output wire  [11:0]  m_data_7,
);

localparam CNT_WIDTH = 16;
localparam DWIDTH = CNT_WIDTH + 12;

reg [DWIDTH-1:0] adc_data[0:7];
reg [11:0] filter_data[0:7];
reg [CNT_WIDTH-1:0] data_cnt[0:7];

reg [1:0] step;
reg [DWIDTH-1:0] old_adc_data;
reg [CNT_WIDTH-1:0] old_cnt;
reg [2:0] chanel;

wire [DWIDTH-1:0] adc_data_next;
wire [CNT_WIDTH-1:0] cnt_next;

reg [11:0]  adc_tmp_data;
assign cnt_next = old_cnt + 1'b1;
assign adc_data_next = old_adc_data + {DWIDTH-12{adc_tmp_data[11]}, adc_tmp_data};

assign m_data_0 = filter_data[0];
assign m_data_1 = filter_data[1];
assign m_data_2 = filter_data[2];
assign m_data_3 = filter_data[3];
assign m_data_4 = filter_data[4];
assign m_data_5 = filter_data[5];
assign m_data_6 = filter_data[6];
assign m_data_7 = filter_data[7];

always @(posedge clk) begin
    if(~reset_n) begin
        adc_data[0] <= 0;
        adc_data[1] <= 0;
        adc_data[2] <= 0;
        adc_data[3] <= 0;
        adc_data[4] <= 0;
        adc_data[5] <= 0;
        adc_data[6] <= 0;
        adc_data[7] <= 0;

        data_cnt[0] <= 0;
        data_cnt[1] <= 0;
        data_cnt[2] <= 0;
        data_cnt[3] <= 0;
        data_cnt[4] <= 0;
        data_cnt[5] <= 0;
        data_cnt[6] <= 0;
        data_cnt[7] <= 0;

        filter_data[0] <= 0;
        filter_data[1] <= 0;
        filter_data[2] <= 0;
        filter_data[3] <= 0;
        filter_data[4] <= 0;
        filter_data[5] <= 0;
        filter_data[6] <= 0;
        filter_data[7] <= 0;

        adc_tmp_data <= 0;
    end else begin
        case (step)
            0: begin
                if (s_axis_valid) begin
                    old_adc_data <= adc_data[s_adc_chanel];
                    old_cnt <= data_cnt[s_adc_chanel];
                    adc_tmp_data <= s_adc_data;
                    chanel <= s_adc_chanel;
                    step <= 1;
                end
            end 
            1: begin
                adc_data[s_adc_chanel] <= adc_data_next;
                old_adc_data <= adc_data_next;
                data_cnt[s_adc_chanel] <= cnt_next;
                old_cnt <= cnt_next;
                step <= 2;
            end
            2: begin
                if (old_cnt == (1 << filter_level)) begin
                    filter_data[chanel] <= (old_adc_data >> filter_level);
                    data_cnt[chanel] <= 0;
                    adc_data[chanel] <= 0;
                end
                step <= 0;
            end
            default: step <= 0;
        endcase
    end
end
    
endmodule