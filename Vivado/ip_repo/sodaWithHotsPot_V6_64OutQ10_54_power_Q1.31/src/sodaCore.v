`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03/09/2023 05:07:58 PM
// Design Name: 
// Module Name: sodaCore
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


module sodaCore #(parameter DATA_WIDTH=32,
            parameter INT_WIDTH=10,
            parameter FLOAT_WIDTH=22,
            parameter SIZE=512) (
            aclk,
            axi_resetn,
            //temp data interface
            s_axis_temp_data, 
            s_axis_temp_ready,
            s_axis_temp_valid,
            //output temp interface
            m_axis_temp_data, 
            m_axis_temp_ready,
            m_axis_temp_valid
    );

    input                           aclk;
    input                           axi_resetn;
    //temp data interface
    input   [DATA_WIDTH-1:0]        s_axis_temp_data; 
    output                          s_axis_temp_ready;
    input                           s_axis_temp_valid;
    //output temp interface
    output  [DATA_WIDTH*5-1:0]      m_axis_temp_data; 
    input                           m_axis_temp_ready;
    output                          m_axis_temp_valid;

  
    reg [DATA_WIDTH-1:0] temp_buffer [0:SIZE*2];
    wire [DATA_WIDTH-1:0] center;
    wire [DATA_WIDTH-1:0] north;
    wire [DATA_WIDTH-1:0] south;
    wire [DATA_WIDTH-1:0] east;
    wire [DATA_WIDTH-1:0] west;
    
    
    reg     delayed_current_temp_valid;
    integer i;
    //temp
    always @(posedge aclk) begin
        if(~axi_resetn) begin
            //temp pipeline registers
            for(i=0; i<SIZE*2+1; i=i+1) begin
                temp_buffer[i] <= {DATA_WIDTH{1'b0}}; 
            end
        end
        else begin
            //pipeline stage 1 update
            if (s_axis_temp_valid & s_axis_temp_ready) begin
                temp_buffer[0] <= s_axis_temp_data;
                for(i=0; i<SIZE*2; i=i+1) begin
                    temp_buffer[i+1] <= temp_buffer[i];
                end
            end
  
        end
    end
    
    always @(posedge aclk) begin
        delayed_current_temp_valid <= s_axis_temp_valid;
    end
    
    assign center = temp_buffer[SIZE];
    assign north = temp_buffer[SIZE*2];
    assign south = temp_buffer[0];
    assign east = temp_buffer[SIZE-1];
    assign west = temp_buffer[SIZE+1];
    
    assign m_axis_temp_data = {center, north, south, east, west};
    assign m_axis_temp_valid = delayed_current_temp_valid;
    assign s_axis_temp_ready = m_axis_temp_ready;

endmodule
