`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/24/2023 11:51:41 AM
// Design Name: 
// Module Name: hotspot_kernel
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

module hotspot_buffer_wrapper #(parameter DATA_WIDTH=32,
                        parameter INT_WIDTH=10,
                        parameter FLOAT_WIDTH=22)(
                        aclk,
                        axi_resetn,
                        //temp data interface
                        s_axis_temp_data, //[c,n,s,e,w]
                        s_axis_temp_ready,
                        s_axis_temp_valid,
                        //power data interface
                        s_axis_power_data,
                        s_axis_power_ready,
                        s_axis_power_valid,
                        //result interface
                        m_axis_result_data,
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
    input   [DATA_WIDTH*5-1:0]      s_axis_temp_data; //[c,n,s,e,w]
    output                          s_axis_temp_ready;
    input                           s_axis_temp_valid;
    //power data interface
    input   [DATA_WIDTH-1:0]        s_axis_power_data; 
    output                          s_axis_power_ready;
    input                           s_axis_power_valid;
    //result interface
    output  [DATA_WIDTH*4-1:0]      m_axis_result_data;
    input                           m_axis_result_ready;
    output                          m_axis_result_valid;
    //config data
    input   [DATA_WIDTH-1:0]        cns;
    input   [DATA_WIDTH-1:0]        cwe;
    input   [DATA_WIDTH-1:0]        cc;
    input   [DATA_WIDTH-1:0]        Cap_1;
    input   [DATA_WIDTH-1:0]        c_amb;
    
    wire   [DATA_WIDTH*5-1:0]        temp_data_to_kernel;
    wire   [DATA_WIDTH-1:0]          power_data_to_kernel;
    wire                            input_ready_from_kernel;
    wire                            input_valid_to_kernal;
    
    wire                          full_from_temp_fifo;
    wire                          full_from_power_fifo;
    wire                          empty_from_temp_fifo;
    wire                          empty_from_power_fifo;
//    reg                          delayed_empty_from_temp_fifo;
//    reg                          delayed_empty_from_power_fifo;
    wire                          rd_en_to_temp_fifo;
    wire                          rd_en_to_power_fifo;
    
    wire   [DATA_WIDTH*2-1:0]        m_axis_power_data;
    wire   [DATA_WIDTH*2-1:0]        m_axis_temp_data;
    
    assign s_axis_temp_ready = ~full_from_temp_fifo; //if almost full not ready. Why almost: safer buffer
    assign s_axis_power_ready = ~full_from_power_fifo; //if almost full not readty. Why almost: safer buffer
    
    assign input_valid_to_kernal = (~empty_from_temp_fifo) & (~empty_from_power_fifo); //if both buffers are not empty, valid data is available. or else, no valid data
    
    assign rd_en_to_temp_fifo = (~empty_from_power_fifo) & input_ready_from_kernel; //if power is ready to give data and kernel is ready to read, give rd_en to temp fifo
    assign rd_en_to_power_fifo = (~empty_from_temp_fifo) & input_ready_from_kernel; //if temp is ready to give data and kernel is ready to read, give rd_en to power fifo
    
    assign m_axis_result_data = {m_axis_power_data, m_axis_temp_data};//combine temp, power outputs
    
//    always @(posedge aclk) begin //though empty become 0 at the same cycle data is written, actual data available at output is 1 clock cycle delayed.
//        delayed_empty_from_temp_fifo <= empty_from_temp_fifo;
//        delayed_empty_from_power_fifo <= empty_from_power_fifo;
//    end
    
    //temp control buffer
    fifo_generator_0 temp_fifo (
      .clk(aclk),              // input wire clk
      .srst(~axi_resetn),            // input wire srst
      .din(s_axis_temp_data),              // input wire [159 : 0] din
      .wr_en(s_axis_temp_valid),          // input wire wr_en
      .rd_en(rd_en_to_temp_fifo),          // input wire rd_en
      .dout(temp_data_to_kernel),            // output wire [159 : 0] dout
      .full(full_from_temp_fifo),            // output wire full
      .empty(empty_from_temp_fifo),          // output wire empty
      .prog_full()  // output wire prog_full
    );
    
    //power control buffer
    fifo_generator_1 power_fifo (
      .clk(aclk),              // input wire clk
      .srst(~axi_resetn),            // input wire srst
      .din(s_axis_power_data),              // input wire [31 : 0] din
      .wr_en(s_axis_power_valid),          // input wire wr_en
      .rd_en(rd_en_to_power_fifo),          // input wire rd_en
      .dout(power_data_to_kernel),            // output wire [31 : 0] dout
      .full(full_from_power_fifo),            // output wire full
      .empty(empty_from_power_fifo),          // output wire empty
      .prog_full()  // output wire prog_full
    );
    
    
    hotspot_kernel #(.DATA_WIDTH(32), .INT_WIDTH(10), .FLOAT_WIDTH(22)) hotspot_kernel_inst (
                        .aclk(aclk),
                        .axi_resetn(axi_resetn),
                        //temp data interface
                        .s_axis_temp_data(temp_data_to_kernel), //[c,n,s,e,w]
                        //power data interface
                        .s_axis_power_data(power_data_to_kernel), 
                        //control signals
                        .s_axis_ready(input_ready_from_kernel),
                        .s_axis_valid(input_valid_to_kernal),
                        //power out interface
                        .m_axis_power_data(m_axis_power_data),
                        //result interface
                        .m_axis_temp_data(m_axis_temp_data),
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

module hotspot_kernel #(parameter DATA_WIDTH = 32,
                        parameter INT_WIDTH=10,
                        parameter FLOAT_WIDTH=22)(
                        aclk,
                        axi_resetn,
                        //temp data interface
                        s_axis_temp_data, //[c,n,s,e,w]
                        //power data interface
                        s_axis_power_data,
                        //control signals
                        s_axis_ready,
                        s_axis_valid,
                        //power out interface
                        m_axis_power_data,
                        //result interface
                        m_axis_temp_data,
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
    input   [DATA_WIDTH*5-1:0]      s_axis_temp_data; //[c,n,s,e,w]
    //power data interface
    input   [DATA_WIDTH-1:0]        s_axis_power_data; 
    output                          s_axis_ready;
    input                           s_axis_valid;
    //power out interface
    output  [DATA_WIDTH*2-1:0]        m_axis_power_data;
    //result interface
    output  [DATA_WIDTH*2-1:0]      m_axis_temp_data;
    input                           m_axis_result_ready;
    output  reg                     m_axis_result_valid;
    //config data
    input   [DATA_WIDTH-1:0]        cns;
    input   [DATA_WIDTH-1:0]        cwe;
    input   [DATA_WIDTH-1:0]        cc;
    input   [DATA_WIDTH-1:0]        Cap_1;
    input   [DATA_WIDTH-1:0]        c_amb;
    
    
    
    wire [DATA_WIDTH-1:0] temp_c;
    wire [DATA_WIDTH-1:0] temp_n;
    wire [DATA_WIDTH-1:0] temp_s;
    wire [DATA_WIDTH-1:0] temp_e;
    wire [DATA_WIDTH-1:0] temp_w;
    
    //pipeline 1 registers
    reg [DATA_WIDTH-1:0]    add_ns_p1;
    reg [DATA_WIDTH-1:0]    add_ew_p1;
    reg [DATA_WIDTH-1:0]    c_amb_p1;
    reg [DATA_WIDTH*2-1:0]  center_temp_p1;
    reg [DATA_WIDTH*2-1:0]  center_power_p1;
    reg                     valid_sig_p1;
    wire                     ready_sig_p1;
    reg [DATA_WIDTH-1:0]    power_p1;
    
    //pipeline 2 registers
    reg [DATA_WIDTH*2-1:0]  mul_ns_p2;
    reg [DATA_WIDTH*2-1:0]  mul_ew_p2;
    reg [DATA_WIDTH*2-1:0]  add_center_power_temp_p2;
    reg [DATA_WIDTH*2-1:0]  c_amb_p2;
    reg                     valid_sig_p2;
    wire                     ready_sig_p2;
    reg [DATA_WIDTH-1:0]    power_p2;
    
    //pipeline 3 registers
    reg [DATA_WIDTH*2-1:0]  add_nsew_p3;
    reg [DATA_WIDTH*2-1:0]  add_center_pt_amb_p3;
    reg                     valid_sig_p3;
    wire                     ready_sig_p3;
    reg [DATA_WIDTH-1:0]    power_p3;
    
    //pipeline 4 registers
    reg [DATA_WIDTH*2-1:0]  temp_result_p4;
    reg [DATA_WIDTH-1:0]    power_p4;
    
    wire [DATA_WIDTH-1:0] power_c;
    
    assign m_axis_temp_data = temp_result_p4;
    assign m_axis_power_data = {{DATA_WIDTH{1'b0}},power_p4};
    
    assign temp_c = s_axis_temp_data[5*DATA_WIDTH-1:4*DATA_WIDTH];
    assign temp_n = s_axis_temp_data[4*DATA_WIDTH-1:3*DATA_WIDTH];
    assign temp_s = s_axis_temp_data[3*DATA_WIDTH-1:2*DATA_WIDTH];
    assign temp_e = s_axis_temp_data[2*DATA_WIDTH-1:1*DATA_WIDTH];
    assign temp_w = s_axis_temp_data[1*DATA_WIDTH-1:0*DATA_WIDTH];
    
    assign power_c = s_axis_power_data;
    
    //equation
    //temp_result_p4 <= temp_c*(cc) + cns*(temp_n+temp_s) + cwe*(temp_e+temp_w) + (c_amb) + (Cap_1 * power_c); //{cc=1.0-((cns<<1) + (cwe<<1) + ctb)}, {c_amb=ctb * amb_temp} is defined in CPU and passed
    always @(posedge aclk) begin
        if(~axi_resetn) begin
            
            //pipeline 1 registers
            add_ns_p1 <= {DATA_WIDTH{1'b0}};
            add_ew_p1 <= {DATA_WIDTH{1'b0}};
            c_amb_p1 <= {DATA_WIDTH{1'b0}};
            center_temp_p1 <= {DATA_WIDTH*2{1'b0}};
            center_power_p1 <= {DATA_WIDTH*2{1'b0}};
            power_p1 <= {DATA_WIDTH{1'b0}};
            
            //pipeline 2 registers
            mul_ns_p2 <= {DATA_WIDTH*2{1'b0}};
            mul_ew_p2 <= {DATA_WIDTH*2{1'b0}};
            add_center_power_temp_p2 <= {DATA_WIDTH*2{1'b0}};
            c_amb_p2 <= {DATA_WIDTH*2{1'b0}};
            power_p2 <= {DATA_WIDTH{1'b0}};
            
            //pipeline 3 registers
            add_nsew_p3 <= {DATA_WIDTH*2{1'b0}};
            add_center_pt_amb_p3 <= {DATA_WIDTH*2{1'b0}};
            power_p2 <= {DATA_WIDTH{1'b0}};
            
            //pipeline 4 registers
            temp_result_p4 <= {DATA_WIDTH*2{1'b0}};
            power_p4 <= {DATA_WIDTH{1'b0}};
        end
        else begin
            //pipeline stage 1 update
            if (s_axis_valid & s_axis_ready) begin
                add_ns_p1 <= temp_n+temp_s; //temp_n=Q10.22, temp_s=Q10.22, add_ns_p1=Q10.22
                add_ew_p1 <= temp_e+temp_w; //temp_e=Q10.22, temp_w=Q10.22, add_ew_p1=Q10.22
                c_amb_p1 <= c_amb;  //c_amb=Q0.32, c_amb_p1=Q0.32
                center_temp_p1 <= temp_c * cc; //temp_c=Q10.22, cc=Q0.32, center_temp_p1=Q10.54
                center_power_p1 <= Cap_1 * power_c; //power_c=Q1.31, Cap_1=Q0.32, center_power_p1=Q1.63
                power_p1 <= power_c; //all Q1.31
            end
  
            //pipeline stage 2 update
            if (valid_sig_p1 & ready_sig_p1) begin
                mul_ns_p2 <= add_ns_p1 * cns; //add_ns_p1=Q10.22, cns=Q0.32, mul_ns_p2=Q10.54
                mul_ew_p2 <= add_ew_p1 * cwe; //add_ew_p1=Q10.22, cwe=Q0.32, mul_ew_p2=Q10.54
                add_center_power_temp_p2 <= (center_power_p1>>(INT_WIDTH-1)) + center_temp_p1; //center_power_p1=Q1.63, center_temp_p1=Q10.54, add_center_power_temp_p2=Q10.54
                c_amb_p2 <= c_amb_p1<<FLOAT_WIDTH; //c_amb_p1=Q0.32 , c_amb_p2=Q10.54
                power_p2 <= power_p1;//all Q1.31
            end     

            //pipeline stage 3 update
            if (valid_sig_p2 & ready_sig_p2) begin
                add_nsew_p3 <= mul_ns_p2 + mul_ew_p2; //all Q10.54
                add_center_pt_amb_p3 <= add_center_power_temp_p2 + c_amb_p2; //All Q10.54
                power_p3 <= power_p2;//all Q1.31
            end
            
            //pipeline stage 4 update
            if (valid_sig_p3 & ready_sig_p3) begin
                temp_result_p4 <= add_nsew_p3 + add_center_pt_amb_p3; //all Q10.54
                power_p4 <= power_p3;//all Q1.31
            end
        end
    end
    
    always @(posedge aclk) begin
        if(~axi_resetn) begin
            valid_sig_p1 <= 1'b0;
            valid_sig_p2 <= 1'b0;
            valid_sig_p3 <= 1'b0;
            m_axis_result_valid <= 1'b0;
        end
        else begin
            if(s_axis_ready) begin
                valid_sig_p1 <= s_axis_valid;
            end
            if(ready_sig_p1) begin
                valid_sig_p2 <= valid_sig_p1;
            end
            if(ready_sig_p2) begin
                valid_sig_p3 <= valid_sig_p2;
            end
            if(ready_sig_p3) begin
                m_axis_result_valid <= valid_sig_p3;
            end
        end
    end
    
    assign ready_sig_p3 = m_axis_result_ready;
    assign ready_sig_p2 = ready_sig_p3;
    assign ready_sig_p1 = ready_sig_p2;
    assign s_axis_ready = ready_sig_p1;
    
    /*
    always @(posedge aclk) begin
        if(~axi_resetn) begin
            ready_sig_p1 <= 1'b0;
            ready_sig_p2 <= 1'b0;
            ready_sig_p3 <= 1'b0;
            s_axis_temp_ready <= 1'b0;
            s_axis_power_ready <= 1'b0;
        end
        else begin
            ready_sig_p3 <= m_axis_result_ready;
            ready_sig_p2 <= ready_sig_p3;
            ready_sig_p1 <= ready_sig_p2;
            s_axis_temp_ready <= ready_sig_p1;
            s_axis_power_ready <= ready_sig_p1;
        end
    end
    */
    
endmodule
