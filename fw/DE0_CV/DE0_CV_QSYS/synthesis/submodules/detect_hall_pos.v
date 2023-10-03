module detect_hall_pos
(
    clk,
    reset_n,
    hall, // a, b, c
    mea_trigger,
    delta_pos,
    position,

    pos_init,
    pos_init_valid
);

localparam DWIDTH = 16;

input clk;
input reset_n;
input [2:0] hall;

input mea_trigger;

output reg [DWIDTH-1:0] delta_pos;

output reg [31:0] position;
input [31:0] pos_init;
input pos_init_valid;

reg [2:0] hall_old;

wire [31:0] pos_next;
wire [31:0] pos_prev;

reg [31:0] old_position;

assign pos_next = position + 1'b1;
assign pos_prev = position - 1'b1;

always @(posedge clk) begin
    if (~reset_n) begin
        delta_pos <= 0;
        position <= 0;
        hall_old <= 0;
        old_position <= 0;
    end else begin
        hall_old <= hall;


        if (pos_init_valid) begin
            position <= pos_init;
            old_position <= pos_init;
        end else begin
            if (mea_trigger) begin
                delta_pos <= position - old_position;
                old_position <= position;
            end 
            
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