`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 08/10/2019 02:28:28 PM
// Design Name: 
// Module Name: edge_detect
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


module edge_detect
#(
    parameter N = 8,
    parameter RST_DELAY = 100
)
(
    input         clk,
    input         nreset,
    input [N-1:0] din,
    input         bvalid,                       // bijan valid, valid name is taken by vivado internally :D
    output        edge_detected
);

    reg [N-1:0] tmp             = 0;
    reg [31:0]   rst_cntr        = 0;
    reg         edge_detected_i = 0;
    reg         fouput_i        = 0;
    
    
    assign edge_detected = edge_detected_i;
    
    always @ (posedge clk) begin
        if (nreset == 1'b0) begin
            tmp = 0;
            rst_cntr = 0;
            edge_detected_i <= 1'b0;
        end
        else if (rst_cntr < RST_DELAY) begin
            rst_cntr <= rst_cntr + 1'b1;
            if (rst_cntr == RST_DELAY)
                edge_detected_i <= 1'b1;
            else
                edge_detected_i <= 1'b0;
        end
        else if (bvalid == 1'b1) begin
            tmp <= din;
            if (tmp != din)
                edge_detected_i <= 1'b1;
            else
                edge_detected_i <= 1'b0;
        end
        else begin
            edge_detected_i <= 1'b0;
        end
    end
        
    
endmodule



