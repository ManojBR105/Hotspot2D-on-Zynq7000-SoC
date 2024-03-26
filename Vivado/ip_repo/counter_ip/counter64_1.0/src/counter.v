`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03/01/2023 06:43:55 PM
// Design Name: 
// Module Name: counter
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


module counter(
    input en,
    input res,
    input clk,
    output reg ovf,
    output reg [31:0] count
    );
    
    always@(posedge clk)
    begin
        if (res)
        begin
            count <= 32'b0;
            ovf <= 1'b0;
        end
        else 
        begin
            if (en)
            begin
                if (count === 32'hFFFFFFFF)
                begin
                    count <= 32'b0;
                    ovf <= 1'b1;
                end
                else
                begin
                    count <= count+1'b1;
                    ovf <= 1'b0;
                end
            end
            else
            begin
                count <= count; 
                ovf <= 1'b0;
            end
        end         
    end
endmodule
