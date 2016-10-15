module supervisor
(
    input cpu_clk,
    input sv_clk,

    output uart_tx,
    input uart_rx,

    output ocd_en,

    output led_tst,

    // 80386 signals
    input _ads,
    input _lock,

    input [23:0] a,
    input _ble,
    input _bhe,
    output [15:0] dout,
    input [15:0] din,

    output _ready,
    output hold,
    input hlda,

    output intr,
    output nmi,

    output reset,

    input wr,
    input dc,
    input mio
);

`define ST_STOP     0
`define ST_RUNNING  1
`define ST_BRK      2

wire [1:0] status;

wire [8:0] ocd_address;
wire ocd_chipselect;
wire ocd_clken;
wire ocd_write;
wire [15:0] ocd_readdata;
wire [15:0] ocd_writedata;
wire [1:0] ocd_byteenable;
wire ocd_reset;
wire ocd_reset_req;
wire ocd_clk;
wire ocd_rst;
wire ocd_stop;

supervisor_qsys sv_qsys
(
    .clk_clk                    (sv_clk),
    .uart_0_txd                 (uart_tx),
    .uart_0_rxd                 (uart_rx),
    .status_export              (status),
    .led_export                 (led_tst),
    .ocd_ctl_export             (ocd_rst),
    .ocd_status_export          (ocd_stop),

    .ocd_mem_s2_address         (ocd_address),
    .ocd_mem_s2_chipselect      (ocd_chipselect),
    .ocd_mem_s2_clken           (ocd_clken),
    .ocd_mem_s2_write           (ocd_write),
    .ocd_mem_s2_readdata        (ocd_readdata),
    .ocd_mem_s2_writedata       (ocd_writedata),
    .ocd_mem_s2_byteenable      (ocd_byteenable),
    .ocd_mem_reset2_reset       (ocd_reset),
    .ocd_mem_reset2_reset_req   (ocd_reset_req),
    .ocd_mem_clk2_clk           (ocd_clk)
);

ocd ocd
(
    .clk                (sv_clk),
    .stop               (ocd_stop),
    .rst                (ocd_rst),
    .en                 (ocd_en),

    .mem_address        (ocd_address),
    .mem_chipselect     (ocd_chipselect),
    .mem_clken          (ocd_clken),
    .mem_write          (ocd_write),
    .mem_readdata       (ocd_readdata),
    .mem_writedata      (ocd_writedata),
    .mem_byteenable     (ocd_byteenable),
    .mem_reset          (ocd_reset),
    .mem_reset_req      (ocd_reset_req),
    .mem_clk            (ocd_clk),

    ._ads               (_ads),
    ._lock              (_lock),
    .a                  (a),
    .dout               (dout),
    .din                (din),
    ._ready             (_ready),
    .hold               (hold),
    .hlda               (hlda),
    .intr               (intr),
    .nmi                (nmi),
    .reset              (reset),
    .wr                 (wr),
    .dc                 (dc),
    .mio                (mio)
);

endmodule
