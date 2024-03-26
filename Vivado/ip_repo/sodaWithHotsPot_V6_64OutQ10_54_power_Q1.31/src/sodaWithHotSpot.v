`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03/11/2023 04:27:32 PM
// Design Name: 
// Module Name: sodaWithHotSpot
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


module sodaWithHotSpot#(parameter DATA_WIDTH=32,
                        parameter INT_WIDTH=10,
                        parameter FLOAT_WIDTH=22,
                        parameter SIZE=512)(
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
                        m_axis_result_data, //{power,temp}
                        m_axis_result_ready,
                        m_axis_result_valid,
                        //config data
                        cns,
                        cwe,
                        cc,
                        Cap_1,
                        c_amb  

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
    output  [DATA_WIDTH*4-1:0]      m_axis_result_data;
    input                           m_axis_result_ready;
    output                          m_axis_result_valid;
    //config data
    input   [DATA_WIDTH-1:0]        cns;
    input   [DATA_WIDTH-1:0]        cwe;
    input   [DATA_WIDTH-1:0]        cc;
    input   [DATA_WIDTH-1:0]        Cap_1;
    input   [DATA_WIDTH-1:0]        c_amb;
       
    wire    [DATA_WIDTH*5-1:0]        temp_data_soda_to_kernel;
    wire                              temp_valid_soda_to_kernel;
    wire                              temp_ready_kernel_to_soda;
    
    wire    [DATA_WIDTH-1:0]          power_data_soda_to_kernel;
    wire                              power_valid_soda_to_kernel;
    wire                              power_ready_kernel_to_soda;
    
    soda #(.DATA_WIDTH(DATA_WIDTH),
           .INT_WIDTH(INT_WIDTH),
           .FLOAT_WIDTH(FLOAT_WIDTH),
           .SIZE(SIZE)) soda_inst (
            .aclk(aclk),
            .axi_resetn(axi_resetn),
            //temp data interface
            .s_axis_temp_data(s_axis_temp_data), 
            .s_axis_temp_ready(s_axis_temp_ready),
            .s_axis_temp_valid(s_axis_temp_valid),
            //power data interface
            .s_axis_power_data(s_axis_power_data), 
            .s_axis_power_ready(s_axis_power_ready),
            .s_axis_power_valid(s_axis_power_valid),
            //output temp interface
            .m_axis_temp_data(temp_data_soda_to_kernel), 
            .m_axis_temp_ready(temp_ready_kernel_to_soda),
            .m_axis_temp_valid(temp_valid_soda_to_kernel),
            //output power interface
            .m_axis_power_data(power_data_soda_to_kernel), 
            .m_axis_power_ready(power_ready_kernel_to_soda),
            .m_axis_power_valid(power_valid_soda_to_kernel)
    );
    
    hotspot_buffer_wrapper #(.DATA_WIDTH(32), .INT_WIDTH(10), .FLOAT_WIDTH(22)) hotspot_buffer_wrapper_inst (
            .aclk(aclk),
            .axi_resetn(axi_resetn),
            //temp data interface
            .s_axis_temp_data(temp_data_soda_to_kernel), //[c,n,s,e,w]
            .s_axis_temp_ready(temp_ready_kernel_to_soda),
            .s_axis_temp_valid(temp_valid_soda_to_kernel),
            //power data interface
            .s_axis_power_data(power_data_soda_to_kernel),
            .s_axis_power_ready(power_ready_kernel_to_soda),
            .s_axis_power_valid(power_valid_soda_to_kernel),
            //result interface
            .m_axis_result_data(m_axis_result_data),
            .m_axis_result_ready(m_axis_result_ready),
            .m_axis_result_valid(m_axis_result_valid),
            //config data
            .cns(cns),
            .cwe(cwe),
            .cc(cc),
            .Cap_1(Cap_1),
            .c_amb(c_amb)  
    );
    
    
endmodule
