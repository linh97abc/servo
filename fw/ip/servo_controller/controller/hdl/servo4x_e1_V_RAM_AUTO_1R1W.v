// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Version: 2022.2
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================

`timescale 1ns/1ps

module servo4x_e1_V_RAM_AUTO_1R1W
#(parameter
    DataWidth    = 16,
    AddressWidth = 2,
    AddressRange = 4
)(
    input  wire [AddressWidth-1:0] address0,
    input  wire                    ce0,
    input  wire                    we0,
    input  wire [DataWidth-1:0]    d0,
    output wire [DataWidth-1:0]    q0,
    input  wire                    clk,
    input  wire                    reset
);
//------------------------Local signal-------------------
reg  [AddressRange-1:0] written = {AddressRange{1'b0}} ;
wire [DataWidth-1:0]    q0_ram;
wire [DataWidth-1:0]    q0_rom;
wire                    q0_sel;
reg  [0:0]              sel0_sr;

//------------------------Instantiation------------------
servo4x_e1_V_RAM_AUTO_1R1W_ram #(
    .DataWidth(DataWidth),
    .AddressWidth(AddressWidth),
    .AddressRange(AddressRange))
servo4x_e1_V_RAM_AUTO_1R1W_ram_u(
    .address0       ( address0 ),
    .ce0            ( ce0 ),
    .we0            ( we0 ),
    .d0             ( d0 ),
    .q0             ( q0_ram ),
    .clk                ( clk ),
    .reset              ( reset )
);
//------------------------Body---------------------------
assign q0     = q0_sel? q0_ram : q0_rom;
assign q0_sel = sel0_sr[0];
assign q0_rom = 'b0; 

always @(posedge clk ) begin
    if (reset)
        written <= 1'b0;
    else begin
        if (ce0 & we0) begin
            written[address0] <= 1'b1;
        end
    end
end

always @(posedge clk) begin
    if (ce0) begin
        sel0_sr[0] <= written[address0];
    end
end

endmodule
