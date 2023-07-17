module servo_protect(
    clk,
    reset_n,
    i0,
    i1,
    i2,
    i3,

    i0_max,
    i1_max,
    i2_max,
    i3_max,

    stop
);

parameter ADC_WIDTH = 12;
localparam SERVO_NUM = 4;

input clk;
input reset_n;
input [ADC_WIDTH-1:0]    i0;
input [ADC_WIDTH-1:0]    i1;
input [ADC_WIDTH-1:0]    i2;
input [ADC_WIDTH-1:0]    i3;

input [ADC_WIDTH-2:0]    i0_max;
input [ADC_WIDTH-2:0]    i1_max;
input [ADC_WIDTH-2:0]    i2_max;
input [ADC_WIDTH-2:0]    i3_max;

output reg [3:0] stop;

wire  [ADC_WIDTH-1:0] si0;
wire  [ADC_WIDTH-1:0] si1;
wire  [ADC_WIDTH-1:0] si2;
wire  [ADC_WIDTH-1:0] si3;
wire  [ADC_WIDTH-1:0] i0_max_neg;
wire  [ADC_WIDTH-1:0] i1_max_neg;
wire  [ADC_WIDTH-1:0] i2_max_neg;
wire  [ADC_WIDTH-1:0] i3_max_neg;

assign si0 = i0 - {1'b1, {ADC_WIDTH-1{1'b0}}};
assign si1 = i1 - {1'b1, {ADC_WIDTH-1{1'b0}}};
assign si2 = i2 - {1'b1, {ADC_WIDTH-1{1'b0}}};
assign si3 = i3 - {1'b1, {ADC_WIDTH-1{1'b0}}};
assign i0_max_neg = {ADC_WIDTH{1'b0}} - {1'b0, i0_max};
assign i1_max_neg = {ADC_WIDTH{1'b0}} - {1'b0, i1_max};
assign i2_max_neg = {ADC_WIDTH{1'b0}} - {1'b0, i2_max};
assign i3_max_neg = {ADC_WIDTH{1'b0}} - {1'b0, i3_max};

always @(posedge clk) begin
    if (~reset_n) begin
        stop <= {SERVO_NUM{1'b1}};
    end else begin
        if (si0[ADC_WIDTH-1]) begin
            stop[0] <= (si0[ADC_WIDTH-2] < i0_max_neg[ADC_WIDTH-2:0])? 1'b1: 1'b0;
        end else begin
            stop[0] <= (si0[ADC_WIDTH-2] > i0_max)? 1'b1: 1'b0;
        end

        if (si1[ADC_WIDTH-1]) begin
            stop[1] <= (si1[ADC_WIDTH-2] < i1_max_neg[ADC_WIDTH-2:0])? 1'b1: 1'b0;
        end else begin
            stop[1] <= (si1[ADC_WIDTH-2] > i1_max)? 1'b1: 1'b0;
        end

        if (si2[ADC_WIDTH-1]) begin
            stop[2] <= (si2[ADC_WIDTH-2] < i2_max_neg[ADC_WIDTH-2:0])? 1'b1: 1'b0;
        end else begin
            stop[2] <= (si2[ADC_WIDTH-2] > i2_max)? 1'b1: 1'b0;
        end

        if (si3[ADC_WIDTH-1]) begin
            stop[3] <= (si3[ADC_WIDTH-2] < i3_max_neg[ADC_WIDTH-2:0])? 1'b1: 1'b0;
        end else begin
            stop[3] <= (si3[ADC_WIDTH-2] > i3_max)? 1'b1: 1'b0;
        end
    end
end


endmodule