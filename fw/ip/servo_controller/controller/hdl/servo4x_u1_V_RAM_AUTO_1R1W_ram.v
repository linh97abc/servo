// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Version: 2022.2
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
`timescale 1 ns / 1 ps
module servo4x_u1_V_RAM_AUTO_1R1W_ram (
     
    address0, ce0,
    
    q0, 
      
    address1, ce1,
    d1, we1, 
    q1, 
     
    reset, clk);

parameter DataWidth = 48;
parameter AddressWidth = 2;
parameter AddressRange = 4;
 
input[AddressWidth-1:0] address0;
input ce0;

output reg[DataWidth-1:0] q0; 
 
input[AddressWidth-1:0] address1;
input ce1;
input[DataWidth-1:0] d1;
input we1; 
output reg[DataWidth-1:0] q1; 

input reset;
input clk;

(* ram_style = "auto"  *)reg [DataWidth-1:0] ram[0:AddressRange-1];


 



always @(posedge clk) 
begin 
    if (ce0) begin
        q0 <= ram[address0];
    end
end 

 
  





//read first
always @(posedge clk)  
begin 
    if (ce1) begin
        if (we1) 
            ram[address1] <= d1; 
        q1 <= ram[address1];

    end
end 
 
 

endmodule

