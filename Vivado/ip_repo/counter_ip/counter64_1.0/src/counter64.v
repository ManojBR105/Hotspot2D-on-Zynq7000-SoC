`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03/02/2023 04:13:53 PM
// Design Name: 
// Module Name: counter64
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module counter64(
    input [31:0] CntrlReg,
    input clk,
    output [31:0] CountReg0,
    output [31:0] CountReg1,
    output [31:0] OvrflReg
    );
    
    assign reset0 = CntrlReg[0];
    assign enable0 = CntrlReg[1];
    
    assign reset1 = CntrlReg[2];
    assign enable1 = CntrlReg[3];
    
    assign reset2 = CntrlReg[4];
    assign enable2 = CntrlReg[5];
    
    assign cascade = CntrlReg[6];
    
    
    wire res0, res1, en0, en1, ovf0, ovf1;
    
    bufif0( res0, reset0,  cascade);
    bufif0( en0, enable0, cascade);
    bufif0( OvrflReg[0], ovf0, cascade);
    bufif0(res1, reset1, cascade);
    bufif0(en1, enable1, cascade);
    bufif0( OvrflReg[1], ovf1, cascade);
    
    bufif1(res0, reset2, cascade);
    bufif1(en0, enable2, cascade);
    bufif1(res1, reset2, cascade);
    bufif1(en1, ovf0, cascade);
    bufif1(OvrflReg[2],  ovf1, cascade);
    
    assign OvrflReg[31:3] = 29'b0;
    
    counter counter0(en0, res0, clk, ovf0, CountReg0);
    counter counter1(en1, res1, clk, ovf1, CountReg1);
    
     
endmodule
