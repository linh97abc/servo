module avl_ltc2992
(
clk,
reset_n,

address,
writedata,
write_n,
read_n,
readdata,

sda_t,
sda_i,
scl_t,
scl_i,

irq
);
    
parameter FREQ_CLK = 100_000_000;
parameter BUS_CLK = 400_000;

input wire clk;
input wire reset_n;

    //avalon mm interface
input wire [3:0] address;
input wire [31:0] writedata;
input wire write_n;
input wire read_n;    
output reg [31:0] readdata; 

output sda_t;
output scl_t;
input sda_i;
input scl_i;

output irq;

reg i2c_ena;
reg [6:0] reg_i2c_addr;
reg [7:0] i2c_command;
reg rw;
reg [7:0] i2c_dout;
wire [7:0] i2c_din;
wire i2c_busy;
wire ack_error;
reg i2c_done;

reg core_reset;
reg start_transmit;
reg [2:0] transmit_len;
reg transmit_rw;
reg core_en;
reg interrupt_en;
reg [7:0] send_data[0:4];
reg [7:0] recv_data[0:4];
wire [2:0] status;

reg [2:0] data_index;

assign status = {i2c_busy, ack_error, i2c_done};
assign irq = i2c_done & interrupt_en;

i2c_master#
(
.input_clk(FREQ_CLK),
.bus_clk(BUS_CLK)
)
i2c_master_inst
(
.clk(clk),
.reset_n(reset_n & ~core_reset),
.ena(i2c_ena),
.addr(reg_i2c_addr),
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

    localparam CR_REG = 0;
    localparam IE_REG = 1;
    localparam FLAG_REG = 2;
    localparam TX_REG = 3;
    localparam RX_REG = 4;

reg [3:0] state;

localparam WAIT_I2C_READY_STATE = 0;
localparam SEND_ADDR_STATE = 1;
localparam WAIT_SEND_ADDR_STATE = 2;
localparam SEND_DATA_STATE = 3;
localparam WAIT_SEND_DATA_STATE = 4;
localparam READ_FIRST_DATA = 5;
localparam READ_DATA_STATE = 6;
localparam WAIT_READ_DATA_STATE = 7;
localparam ERR_STATE = 8;
localparam DONE_STATE = 9;

 //avalon write data 
    always @(posedge clk or negedge reset_n)             //write down to reg
    begin
        if(~reset_n) begin
            core_reset <= 1'b0;
            core_en <= 1'b0;
            interrupt_en <= 1'b0;
            transmit_rw <= 0;
            transmit_len <= 0;
            reg_i2c_addr <= 0;
            i2c_command <= 0;
            i2c_done <= 0;
        end
        else begin 
            core_reset <= (address == CR_REG)? (~write_n & writedata[31]): 1'b0;
            start_transmit <= (state != SEND_ADDR_STATE) & 
                (start_transmit | ((address == CR_REG)? (~write_n & writedata[30]): 1'b0));

            i2c_done <= (i2c_done & ((address == FLAG_REG)? (write_n | ~writedata[0]): 1'b1)) |
                 (((state == DONE_STATE) || (state == ERR_STATE))? 1'b1: 1'b0);

            if(~write_n) begin
                case (address)
                CR_REG: begin
                    $display("Set CR_REG writedata=%x", writedata);
                    $display("Set transmit_rw=%d, transmit_len=%d",
                        writedata[24],
                         writedata[18:16]);
                    core_en <= writedata[29];
                    transmit_rw <= writedata[24];
                    transmit_len <= writedata[18:16];
                    
                    reg_i2c_addr <= writedata[14:8];
                    i2c_command <= writedata[7:0];
                end

                IE_REG: begin
                    interrupt_en <= writedata[0];
                end
                TX_REG: begin
                    if (core_en) begin
                        send_data[0] <= writedata[7:0];
                        send_data[1] <= writedata[15:8];
                        send_data[2] <= writedata[23:0];
                        send_data[3] <= writedata[31:0];
                        
                    end
                end
                endcase 
            end
            
        end
    end
    
    //avalon read data
    always @(*)            //read reg value      
    begin
        if(~read_n) begin
            case (address)
                CR_REG: readdata <= {core_en, 4'b0, transmit_rw, 5'b0, transmit_len, 1'b0, reg_i2c_addr, i2c_command};
                FLAG_REG: readdata <= status;
                RX_REG: readdata <= {recv_data[3], recv_data[2], recv_data[1], recv_data[0]};
                default: readdata <= 0;
            endcase
        end

    end  

    always @(posedge clk or negedge reset_n)
    begin
        /// write
        // wait ~i2c_busy

        // i2c_ena = 1
        // rw = 0
        // write i2c_addr
        // write reg_addr

        // wait ~i2c_busy, write data

        /// read
        // wait ~i2c_busy

        // i2c_ena = 1
        // rw = 1
        // write i2c_addr
        // write reg_addr

        // wait ~i2c_busy

        // rw = 1
        // write i2c_addr

        // wait ~i2c_busy, read data

        if (~reset_n) begin
            state <= WAIT_I2C_READY_STATE;
            i2c_ena <= 1'b0;
            rw <= 0;
            i2c_dout <= 0;
            data_index <= 0;
        end else begin
            case (state)
                WAIT_I2C_READY_STATE: begin
                    if (~i2c_busy & start_transmit) begin
                        state <= SEND_ADDR_STATE;
                    end
                end

                SEND_ADDR_STATE: begin
                    i2c_ena <= 1'b1;
                    rw <= 1'b0;
                    i2c_dout <= i2c_command;
                    if (i2c_busy) begin

                        $display("WAIT_SEND_ADDR_STATE");
                        state <= WAIT_SEND_ADDR_STATE;
                    end
                end

                WAIT_SEND_ADDR_STATE: begin
                    i2c_ena <= ~transmit_rw;
                    if (~i2c_busy) begin
                        if (ack_error) begin
                            $display("WAIT_SEND_ADDR_STATE ack err");
                            state <= ERR_STATE;
                        end else begin
                            data_index <= 0;
                            if (transmit_rw) begin
                                rw <= 1'b1;
                                $display("WAIT_SEND_ADDR_STATE -> READ_FIRST_DATA");
                                state <= READ_FIRST_DATA;
                            end else begin
                                $display("WAIT_SEND_ADDR_STATE -> SEND_DATA_STATE");
                                state <= SEND_DATA_STATE;
                            end
                        end

                    end
                end

                SEND_DATA_STATE: begin

                    if (data_index + 1'b1 < transmit_len) begin
                        i2c_ena <= 1'b1;
                        
                    end else begin
                        i2c_ena <= 1'b0;
                    end

                    rw <= 1'b0;
                    i2c_dout <= send_data[data_index];
                    if (i2c_busy) begin

                        $display("WAIT_SEND_DATA_STATE");
                        state <= WAIT_SEND_DATA_STATE;
                    end
                end

                WAIT_SEND_DATA_STATE: begin
                    
                    if (ack_error) begin
                    // if (0) begin
                        $display("WAIT_SEND_DATA_STATE ack err");
                        state <= ERR_STATE;
                    end else if (~i2c_busy) begin
                        data_index = data_index + 1'b1;
                        if (data_index  < transmit_len) begin
                            // i2c_ena <= 1'b1;
                            $display("WAIT_SEND_DATA_STATE -> SEND_DATA_STATE");
                            state <= SEND_DATA_STATE;
                        end else begin
                            // i2c_ena <= 1'b0;
                            $display("data_index: %d, transmit_len: %d", data_index, transmit_len);
                            $display("WAIT_SEND_DATA_STATE -> DONE_STATE");
                            state <= DONE_STATE;
                        end

                    end
                end

                READ_FIRST_DATA: begin
                    state <= READ_DATA_STATE;
                end

                READ_DATA_STATE: begin
                    if (data_index + 1'b1 < transmit_len) begin
                        i2c_ena <= 1'b1;
                        
                    end else begin
                        i2c_ena <= 1'b0;
                    end

                    rw <= 1'b1;
                    recv_data[data_index] = 
                    i2c_dout <= send_data[data_index];
                    if (i2c_busy) begin

                        $display("WAIT_READ_DATA_STATE");
                        state <= WAIT_READ_DATA_STATE;
                    end
                end

                WAIT_READ_DATA_STATE: begin
                    

                    if(0) begin
                    // if (ack_error) begin
                        $display("WAIT_READ_DATA_STATE ack err");
                        state <= ERR_STATE;
                    end else if (~i2c_busy) begin
                        data_index = data_index + 1'b1;
                        if (data_index < transmit_len) begin
                            $display("READ_DATA_STATE");
                            state <= READ_DATA_STATE;
                        end else begin
                            $display("DONE_STATE");
                            state <= DONE_STATE;
                        end

                    end
                end

                ERR_STATE: begin
                    i2c_ena <= 1'b0;
                    state <= WAIT_I2C_READY_STATE;
                end

                DONE_STATE: begin
                    state <= WAIT_I2C_READY_STATE;
                end
                default: begin
                    state <= WAIT_I2C_READY_STATE;
                end
            endcase
        end

    end
endmodule