module top_level
(
    input clk,
    output led_tst,

    input [23:1] a,
    input _bhe,
    input _ble,
    input _ads,

    inout [15:0] d,

    output _ready,
    output hold,
    input hlda,

    output intr,
    output nmi,

    output reset,
    input _lock,

    input wr,
    input dc,
    input mio,

    input uart_tx,
    output uart_rx,

    output ramwe,
    output ramcs
);

wire [15:0] dout;

// BC signals
wire [15:0] bc_dout;
wire [15:0] mem_dout;
wire [15:0] io_dout;
wire bc_ready;
wire bc_wr;
wire mc_ramcs;
wire mc_ramwe;

// SV signals
wire ocd_en;
wire sv_clk;
wire [15:0] sv_dout;
wire sv_ready;
wire sv_hold;
wire sv_intr;
wire sv_nmi;
wire sv_reset;

assign d[15:8] = (!ocd_en && !oe) || wr || _bhe || !ramcs ? 8'hzz : dout[15:8]; 
assign d[7:0] = (!ocd_en && !oe) || wr || _ble || !ramcs ? 8'hzz : dout[7:0]; 

// Signals Mux
assign dout = ocd_en ? sv_dout : mem_dout;
assign _ready = ocd_en ? sv_ready : bc_ready;
assign hold = sv_hold;
assign intr = sv_intr;
assign nmi = sv_nmi;
assign reset = sv_reset;

assign bc_wr = ocd_en ? 0 : wr;

assign ramcs = ocd_en ? 1 : mc_ramcs;
assign ramwe = ocd_en ? 1 : mc_ramwe;

bus_control bc
(
    .clk                (clk),
    ._ads               (_ads),
    ._ready             (bc_ready),
    .oe                 (oe),
    .bce                (bce)
);

memory_control mem
(
    .clk                (clk),
    .cs                 (bce && mio),
    .addr               (a),
    .data_in            (d),
    .data_out           (mem_dout),
    .wr                 (wr),
    .low                (!_ble),
    .high               (!_bhe),
    .ramcs              (mc_ramcs),
    .ramwe              (mc_ramwe)
);

supervisor sv
(
    .cpu_clk            (clk),
    .sv_clk             (sv_clk),

    .uart_tx            (uart_rx),
    .uart_rx            (uart_tx),

    .ocd_en             (ocd_en),

    ._ads               (_ads),
    ._lock              (_lock),
    .a                  ({a, _ble}),
    ._bhe               (_bhe),
    ._ble               (_ble),
    .dout               (sv_dout),
    .din                (d),
    ._ready             (sv_ready),
    .hold               (sv_hold),
    .hlda               (hlda),
    .intr               (sv_intr),
    .nmi                (sv_nmi),
    .reset              (sv_reset),
    .wr                 (wr),
    .dc                 (dc),
    .mio                (mio),


    .led_tst            (led_tst)
);

pll pll
(
    .inclk0             (clk),
    .c1                 (sv_clk)
);

endmodule
