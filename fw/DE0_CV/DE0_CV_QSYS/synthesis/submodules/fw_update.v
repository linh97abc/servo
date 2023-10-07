module fw_update (
    clk,
    reset_n,

    address,
    writedata,
    write_n,
    read_n,
    readdata

);

  parameter [7:0] VERSION_MAJOR = 1;
  parameter [7:0] VERSION_MINOR = 0;
  parameter [7:0] PRODUCT_ID = 0;
  parameter [31:0] LOCK_PWD = 12345;

  input wire clk;
  input wire reset_n;

  //avalon mm interface
  input wire [2:0] address;
  input wire [31:0] writedata;
  input wire write_n;
  input wire read_n;
  output reg [31:0] readdata;

  localparam VERSION_REG = 0;
  localparam LOCK_REG = 1;
  localparam USER0_REG = 2;
  localparam USER1_REG = 3;
  localparam USER2_REG = 4;
  localparam USER3_REG = 5;
  localparam USER4_REG = 6;
  localparam USER5_REG = 7;


  reg [31:0] lock_reg;
  reg [31:0] user0_reg;
  reg [31:0] user1_reg;
  reg [31:0] user2_reg;
  reg [31:0] user3_reg;
  reg [31:0] user4_reg;
  reg [31:0] user5_reg;


  //avalon write data 
  always @(posedge clk)             //write down to reg
    begin

    if (~write_n) begin
      if (address == LOCK_REG) begin
        lock_reg <= writedata;
      end else if (lock_reg == LOCK_PWD) begin
        case (address)
          USER0_REG: user0_reg <= writedata;
          USER1_REG: user1_reg <= writedata;
          USER2_REG: user2_reg <= writedata;
          USER3_REG: user3_reg <= writedata;
          USER4_REG: user4_reg <= writedata;
          USER5_REG: user5_reg <= writedata;
        endcase
      end


    end

  end

  //avalon read data
  always @(*)            //read reg value      
    begin
    if (~read_n) begin
      case (address)
        VERSION_REG: readdata <= {PRODUCT_ID, VERSION_MAJOR, VERSION_MINOR};
        USER0_REG: readdata <= user0_reg;
        USER1_REG: readdata <= user1_reg;
        USER2_REG: readdata <= user2_reg;
        USER3_REG: readdata <= user3_reg;
        USER4_REG: readdata <= user4_reg;
        USER5_REG: readdata <= user5_reg;
        default: readdata <= 0;
      endcase
    end

  end
endmodule
