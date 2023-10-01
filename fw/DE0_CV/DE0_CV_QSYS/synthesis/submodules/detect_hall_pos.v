module detect_hall_pos
(
    clk,
    reset_n,
    hall, // a, b, c
    mea_trigger,
    delta_pos,
    position
);

localparam DWIDTH = 16;

input clk;
input reset_n;
input [2:0] hall;

input mea_trigger;

output reg [DWIDTH-1:0] delta_pos;

output reg [DWIDTH-1:0] position;
reg [2:0] hall_old;

wire [DWIDTH-1:0] pos_next;
wire [DWIDTH-1:0] pos_prev;

assign pos_next = position + 1'b1;
assign pos_prev = position - 1'b1;

always @(posedge clk) begin
    if (~reset_n) begin
        delta_pos <= 0;
        position <= 0;
        hall_old <= 0;
    end else begin
        hall_old <= hall;

        if (mea_trigger) begin
            delta_pos <= position;

            case ({hall, hall_old})
                {3'b001, 3'b101}: position <= 1;
                {3'b001, 3'b011}: position <= {DWIDTH{1'b1}};

                {3'b011, 3'b001}: position <= 1;
                {3'b011, 3'b010}: position <= {DWIDTH{1'b1}};

                {3'b010, 3'b011}: position <= pos_next;
                {3'b010, 3'b110}: position <= {DWIDTH{1'b1}};

                {3'b110, 3'b010}: position <= 1;
                {3'b110, 3'b100}: position <= {DWIDTH{1'b1}};

                {3'b100, 3'b110}: position <= 1;
                {3'b100, 3'b101}: position <= {DWIDTH{1'b1}};

                {3'b101, 3'b100}: position <= 1;
                {3'b101, 3'b001}: position <= {DWIDTH{1'b1}};

                default: position <= 0;
            endcase
        end else begin

            case ({hall, hall_old})
                {3'b001, 3'b101}: position <= pos_next;
                {3'b001, 3'b011}: position <= pos_prev;

                {3'b011, 3'b001}: position <= pos_next;
                {3'b011, 3'b010}: position <= pos_prev;

                {3'b010, 3'b011}: position <= pos_next;
                {3'b010, 3'b110}: position <= pos_prev;

                {3'b110, 3'b010}: position <= pos_next;
                {3'b110, 3'b100}: position <= pos_prev;

                {3'b100, 3'b110}: position <= pos_next;
                {3'b100, 3'b101}: position <= pos_prev;

                {3'b101, 3'b100}: position <= pos_next;
                {3'b101, 3'b001}: position <= pos_prev;

                default: position <= position;
            endcase
        end
    end
end

endmodule