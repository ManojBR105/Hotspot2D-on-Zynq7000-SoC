`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03/10/2023 08:34:41 PM
// Design Name: 
// Module Name: edgeCases
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


module edgeCases #(parameter DATA_WIDTH=32,
                    parameter SIZE=512)(
    valid_data_counter,
    in_data,
    out_data
    );
    input   [31:0] valid_data_counter;
    input   [DATA_WIDTH*5-1:0] in_data;
    output  [DATA_WIDTH*5-1:0]  out_data;
    
    wire [DATA_WIDTH-1:0] in_center;
    wire [DATA_WIDTH-1:0] in_north;
    wire [DATA_WIDTH-1:0] in_south;
    wire [DATA_WIDTH-1:0] in_east;
    wire [DATA_WIDTH-1:0] in_west;
    
    reg [DATA_WIDTH-1:0] out_center;
    reg [DATA_WIDTH-1:0] out_north;
    reg [DATA_WIDTH-1:0] out_south;
    reg [DATA_WIDTH-1:0] out_east;
    reg [DATA_WIDTH-1:0] out_west;
    
    assign in_center = in_data[5*DATA_WIDTH-1:4*DATA_WIDTH];
    assign in_north = in_data[4*DATA_WIDTH-1:3*DATA_WIDTH];
    assign in_south = in_data[3*DATA_WIDTH-1:2*DATA_WIDTH];
    assign in_east = in_data[2*DATA_WIDTH-1:1*DATA_WIDTH];
    assign in_west = in_data[1*DATA_WIDTH-1:0*DATA_WIDTH];
    
    assign out_data = {out_center, out_north, out_south, out_east, out_west};
    
    always @(*) begin
        if(valid_data_counter%SIZE==1) begin
            out_west = in_center;
        end
        else begin
            out_west = in_west;
        end
        if (valid_data_counter%SIZE==0) begin
            out_east = in_center;
        end
        else begin
            out_east = in_east;
        end
        if(valid_data_counter < (2*SIZE + 1)) begin
            out_north = in_center;
        end
        else begin
            out_north = in_north;
        end
        if (valid_data_counter>SIZE*SIZE) begin
            out_south = in_center;
        end
        else begin
            out_south = in_south;
        end
        out_center = in_center;
    end
    
endmodule
