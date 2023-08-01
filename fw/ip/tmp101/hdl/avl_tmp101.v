module avl_tmp101
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
scl_i

);
    
parameter FREQ_CLK = 100_000_000;
parameter BUS_CLK = 400_000;
parameter UPDATE_FREQ = 2;
parameter [2:0] I2C_ADDR = 0;

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

reg core_reset;
reg core_en;
wire [11:0] temperature;
wire [2:0] status;

tmp101#
(
.FREQ_CLK(FREQ_CLK),
.BUS_CLK(BUS_CLK),
.UPDATE_FREQ(UPDATE_FREQ),
.I2C_ADDR(I2C_ADDR)

) tmp101_inst
 (
    .clk(clk),
    .reset_n(reset_n & ~core_reset),
    .ena(core_en),

    .temperature(temperature),
    .status(status),

    .sda_t(sda_t),
    .sda_i(sda_i),
    .scl_t(scl_t),
    .scl_i(scl_i)
);

    localparam CR_REG = 0;
    localparam FLAG_REG = 1;
    localparam TEMPER_REG = 2;

 //avalon write data 
    always @(posedge clk or negedge reset_n)             //write down to reg
    begin
        if(~reset_n) begin
            core_reset <= 1'b0;
            core_en <= 1'b0;
        end
        else begin 
            core_reset <= (address == CR_REG)? (~write_n & writedata[1]): 1'b0;

            if(~write_n) begin
                case (address)
                CR_REG: begin
                    core_en <= writedata[0];
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
                CR_REG: readdata <= core_en;
                FLAG_REG: readdata <= status;
                TEMPER_REG: readdata <= {temperature, 4'b0};
                default: readdata <= 0;
            endcase
        end

    end  
endmodule