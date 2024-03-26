`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03/10/2023 08:07:02 PM
// Design Name: 
// Module Name: soda
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


module soda #(parameter DATA_WIDTH=32,
            parameter INT_WIDTH=10,
            parameter FLOAT_WIDTH=22,
            parameter SIZE=512) (
            aclk,
            axi_resetn,
            //temp data interface
            s_axis_temp_data, 
            s_axis_temp_ready,
            s_axis_temp_valid,
            //power data interface
            s_axis_power_data, 
            s_axis_power_ready,
            s_axis_power_valid,
            //output temp interface
            m_axis_temp_data, 
            m_axis_temp_ready,
            m_axis_temp_valid,
            //output power interface
            m_axis_power_data, 
            m_axis_power_ready,
            m_axis_power_valid
    );

    input                           aclk;
    input                           axi_resetn;
    //temp data interface
    input   [DATA_WIDTH-1:0]        s_axis_temp_data; 
    output                          s_axis_temp_ready;
    input                           s_axis_temp_valid;
    //power data interface
    input   [DATA_WIDTH-1:0]        s_axis_power_data; 
    output                          s_axis_power_ready;
    input                           s_axis_power_valid;
    //output temp interface
    output  [DATA_WIDTH*5-1:0]      m_axis_temp_data; 
    input                           m_axis_temp_ready;
    output                          m_axis_temp_valid;
    //output power interface
    output  [DATA_WIDTH-1:0]        m_axis_power_data; 
    input                           m_axis_power_ready;
    output                          m_axis_power_valid;
    
    integer valid_data_counter;
    wire actual_valid;
    wire valid_from_FF_inst;
    wire [DATA_WIDTH*5-1:0] data_from_FF_inst;
    
    wire valid_from_powerBuffer_inst;
    
    always @(posedge aclk) begin
        if(~axi_resetn) begin
            valid_data_counter <= 0;
        end
        else begin
            if(s_axis_temp_ready & s_axis_temp_valid) begin
                if(valid_data_counter==SIZE*SIZE+SIZE) begin
                    valid_data_counter <= 1;
                end
                else begin
                    valid_data_counter <= valid_data_counter+1;
                end
            end
        end
    end
    
    assign m_axis_temp_valid = (valid_data_counter<SIZE+1)?1'b0:valid_from_FF_inst;
    assign m_axis_power_valid = (valid_data_counter<SIZE+1)?1'b0:valid_from_powerBuffer_inst;
    
    sodaCore #(.DATA_WIDTH(DATA_WIDTH),
           .INT_WIDTH(INT_WIDTH),
           .FLOAT_WIDTH(FLOAT_WIDTH),
           .SIZE(SIZE)) sodaCore_inst (
            .aclk(aclk),
            .axi_resetn(axi_resetn),
            //temp data interface
            .s_axis_temp_data(s_axis_temp_data), 
            .s_axis_temp_ready(s_axis_temp_ready),
            .s_axis_temp_valid(s_axis_temp_valid),
            //output temp interface
            .m_axis_temp_data(data_from_FF_inst), 
            .m_axis_temp_ready(m_axis_temp_ready),
            .m_axis_temp_valid(valid_from_FF_inst)
    );
    
    powerBuffer #(.DATA_WIDTH(DATA_WIDTH),
           .INT_WIDTH(INT_WIDTH),
           .FLOAT_WIDTH(FLOAT_WIDTH),
           .SIZE(SIZE)) powerBuffer_inst (
            .aclk(aclk),
            .axi_resetn(axi_resetn),
            //power data interface
            .s_axis_power_data(s_axis_power_data), 
            .s_axis_power_ready(s_axis_power_ready),
            .s_axis_power_valid(s_axis_power_valid),
            //output power interface
            .m_axis_power_data(m_axis_power_data), 
            .m_axis_power_ready(m_axis_power_ready),
            .m_axis_power_valid(valid_from_powerBuffer_inst)
    );
    
    edgeCases #(.DATA_WIDTH(DATA_WIDTH),
                .SIZE(SIZE)) edgeCases_inst (
    .valid_data_counter(valid_data_counter),
    .in_data(data_from_FF_inst),
    .out_data(m_axis_temp_data)
    );
    
endmodule
