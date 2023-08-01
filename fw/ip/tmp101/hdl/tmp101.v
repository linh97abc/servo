module tmp101
 (
    clk,
    reset_n,
    ena,

    temperature,
    status,

    sda_t,
    sda_i,
    scl_t,
    scl_i
);
parameter FREQ_CLK = 100_000_000;
parameter BUS_CLK = 400_000;
parameter UPDATE_FREQ = 2;
parameter [2:0] I2C_ADDR = 0;

input clk;
input reset_n;
input ena;

output [11:0] temperature;
output [2:0] status; //i2c_busy, i2c_ack_err, data_valid

output sda_t;
output scl_t;
input sda_i;
input scl_i;

localparam [31:0] UPDATE_PRES = FREQ_CLK/UPDATE_FREQ - 1;

wire [6:0] tmp101_addr;
assign tmp101_addr = {4'b1001, I2C_ADDR};

reg rw;
wire [7:0] i2c_dout;
wire [7:0] i2c_din;
wire i2c_busy;
wire ack_error;
reg i2c_ena;

reg [7:0] temp0;
reg [3:0] temp1;
reg [31:0] update_cnt;
reg data_valid;

assign status = {i2c_busy, ack_error, data_valid};

i2c_master#
(
.input_clk(FREQ_CLK),
.bus_clk(BUS_CLK)
)
i2c_master_inst
(
.clk(clk),
.reset_n(reset_n),
.ena(i2c_ena),
.addr(tmp101_addr),
.rw(rw),
.data_wr(i2c_dout),
.busy(i2c_busy),
.data_rd(i2c_din),
.ack_error(ack_error),
    .sda_t(sda_t),
    .sda_i(sda_i),
    .scl_t(scl_t),
    .scl_i(scl_i)
);

assign i2c_dout = 0;

reg [3:0] state;

localparam WAIT_I2C_READY_STATE = 0;
localparam SEND_POINTER_STATE = 1;
localparam WAIT_SEND_POINTER_STATE = 2;
localparam READ_TEMP_STATE = 3;
localparam WAIT_READ0_STATE = 4;
localparam WAIT_START_READ1_STATE = 5;
localparam WAIT_READ1_STATE = 6;
localparam END_STATE = 7;



assign temperature = {temp0, temp1};

always @(posedge clk) begin
    if (~reset_n) begin
        state <= WAIT_I2C_READY_STATE;
        i2c_ena <= 1'b0;
        update_cnt <= 0;
        temp0 <= 0;
        temp1 <= 0;
        data_valid <= 0;
    end else begin
        if (ena) begin
            case (state)
            WAIT_I2C_READY_STATE: begin
                if (~i2c_busy) begin
                    state <= SEND_POINTER_STATE;
                end
            end
            SEND_POINTER_STATE: begin
                i2c_ena <= 1'b1;
                rw <= 1'b0;
                if (i2c_busy) begin

                    $display("WAIT_SEND_POINTER_STATE");
                    state <= WAIT_SEND_POINTER_STATE;
                end
            end


            WAIT_SEND_POINTER_STATE: begin
                i2c_ena <= 1'b0;
                if (ack_error) begin
                    $display("WAIT_SEND_POINTER_STATE ack err");
                    state <= WAIT_I2C_READY_STATE;
                end else if (~i2c_busy) begin
                    $display("READ_TEMP_STATE");
                    state <= READ_TEMP_STATE;
                end
            end

            READ_TEMP_STATE: begin
                i2c_ena <= 1'b1;
                rw <= 1'b1;
                if (i2c_busy) begin
                    $display("WAIT_READ0_STATE");
                    state <= WAIT_READ0_STATE;
                end
            end

            WAIT_READ0_STATE: begin
                if (~i2c_busy) begin
                    state <= WAIT_START_READ1_STATE;
                    temp0 <= i2c_din;
                end
            end

            WAIT_START_READ1_STATE: begin
                i2c_ena <= 1'b0;
                if (i2c_busy) begin
                    state <= WAIT_READ1_STATE;
                    temp0 <= i2c_din;
                end
            end


            WAIT_READ1_STATE: begin
                if (~i2c_busy) begin
                    update_cnt <= 0;
                    state <= END_STATE;
                    
                    temp1 <= i2c_din[7:4];
                end
            end

            END_STATE: begin

                if (ack_error) begin
                    data_valid <= 1'b0;
                    state <= WAIT_I2C_READY_STATE;
                end else begin
                    data_valid <= 1'b1;
                    if (update_cnt == UPDATE_PRES) begin
                        state <= WAIT_I2C_READY_STATE;
                    end else begin
                        
                        update_cnt <= update_cnt + 1'b1;
                    end
                    
                end

            end
            default: begin
            end
            endcase

        end
    end
end

endmodule