`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03/09/2023 05:07:58 PM
// Design Name: 
// Module Name: powerBuffer
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


module powerBuffer #(parameter DATA_WIDTH=32,
            parameter INT_WIDTH=10,
            parameter FLOAT_WIDTH=22,
            parameter SIZE=512) (
            aclk,
            axi_resetn,
            //power data interface
            s_axis_power_data, 
            s_axis_power_ready,
            s_axis_power_valid,
            //output power interface
            m_axis_power_data, 
            m_axis_power_ready,
            m_axis_power_valid
    );

    input                           aclk;
    input                           axi_resetn;
    //power data interface
    input   [DATA_WIDTH-1:0]        s_axis_power_data; 
    output                          s_axis_power_ready;
    input                           s_axis_power_valid;
    //output power interface
    output  [DATA_WIDTH*5-1:0]      m_axis_power_data; 
    input                           m_axis_power_ready;
    output                          m_axis_power_valid;

  
    reg [DATA_WIDTH-1:0] power_buffer [0:SIZE];
    wire [DATA_WIDTH-1:0] center;
    
    
    reg     delayed_current_power_valid;
    integer i;
    //temp
    always @(posedge aclk) begin
        if(~axi_resetn) begin
            //temp pipeline registers
            for(i=0; i<SIZE+1; i=i+1) begin
                power_buffer[i] <= {DATA_WIDTH{1'b0}}; 
            end
        end
        else begin
            //pipeline stage 1 update
            if (s_axis_power_valid & s_axis_power_ready) begin
                power_buffer[0] <= s_axis_power_data;
                for(i=0; i<SIZE; i=i+1) begin
                    power_buffer[i+1] <= power_buffer[i];
                end
            end
  
        end
    end
    
    always @(posedge aclk) begin
        delayed_current_power_valid <= s_axis_power_valid;
    end
    
    assign center = power_buffer[SIZE];
    
    assign m_axis_power_data = center;
    assign m_axis_power_valid = delayed_current_power_valid;
    assign s_axis_power_ready = m_axis_power_ready;

endmodule
