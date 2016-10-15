module memory_control
(
    input clk,
    input cs,
    input [23:1] addr,
    output [15:0] data_out,
    input [15:0] data_in,
    input low,
    input high,
    input wr,

    output ramcs,
    output ramwe
);

wire [15:0] internal_ram_q;
wire [15:0] internal_ram_out;
wire internal_ram_wr;
wire internal_ram_cs;

assign data_out = cs ? internal_ram_out : 0;

assign internal_ram_cs = cs && ((addr[23:15] == 9'h1FF)
                             || (addr[23:15] == 9'h01F));
assign internal_ram_wr = internal_ram_cs ? wr : 0;
assign internal_ram_out = internal_ram_cs ? internal_ram_q : 0;

assign ramwe = !ramcs ? !wr : 1;
assign ramcs = !(cs && ((addr[23:16] < 8'h0A)
                     || (addr[23:20] == 4'h1)));

internal_ram ram
(
    .address (addr[14:1]),
    .clock (clk),
    .data (data_in),
    .wren (internal_ram_wr),
    .byteena ({high, low}),
    .q (internal_ram_q)
);

endmodule
