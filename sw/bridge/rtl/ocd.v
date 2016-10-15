module ocd
(
    input rst,
    input clk,
    output reg en,
    output reg stop,

    // 80386 signals
    input _ads,
    input _lock,

    input [23:0] a,
    output reg [15:0] dout,
    input [15:0] din,

    output reg _ready,
    output reg hold,
    input hlda,

    output reg intr,
    output reg nmi,

    output reg reset,

    input wr,
    input dc,
    input mio,

    // Avalon Interface
    output reg [8:0] mem_address,
    output mem_chipselect,
    output mem_clken,
    output reg mem_write,
    input [15:0] mem_readdata,
    output reg [15:0] mem_writedata,
    output [1:0] mem_byteenable,
    output mem_reset,
    output mem_reset_req,
    output mem_clk
);

// Avalon Interface

assign mem_byteenable = 2'b11;
assign mem_clk = clk;
assign mem_clken = 1;
assign mem_chipselect = 1;

// FSM

`define ST_FETCH        0
`define ST_LOAD         2
`define ST_EXEC         4
`define ST_EXEC_STORE   5
`define ST_STORE        6
`define ST_STORE_FETCH  7

reg [2:0] state;

// Decoding

`define OCD_CTL     (0 << 5)
`define LD_ST       (1 << 5)
`define CPU_CTL     (2 << 5)
`define FLOW_CTL    (3 << 5)
`define MISC        (7 << 5)

`define ATTACH      (`OCD_CTL | 5'b00010)
`define DETACH      (`OCD_CTL | 5'b00011)
`define EXIT        (`OCD_CTL | 5'b00000)

`define LDI         (`LD_ST | 5'b00000)
`define MOV         (`LD_ST | 5'b00001)
`define LDD         (`LD_ST | 5'b00100)
`define LDAL        (`LD_ST | 5'b00010)
`define LDAH        (`LD_ST | 5'b00011)
`define LDWR        (`LD_ST | 5'b00101)
`define LDDC        (`LD_ST | 5'b00110)
`define LDMIO       (`LD_ST | 5'b00111)
`define LDCMP       (`LD_ST | 5'b01000)
`define CLR         (`LD_ST | 5'b01001)
`define STD         (`LD_ST | 5'b10100)

`define START       (`CPU_CTL | 5'b00000)
`define RESET       (`CPU_CTL | 5'b00001)
`define WAITADS     (`CPU_CTL | 5'b00010)
`define WAITLOCK    (`CPU_CTL | 5'b01000)
`define WAITIO      (`CPU_CTL | 5'b01001)
`define READY       (`CPU_CTL | 5'b00011)
`define INT         (`CPU_CTL | 5'b00100)
`define NMI         (`CPU_CTL | 5'b00101)
`define HOLD        (`CPU_CTL | 5'b00110)

`define CMP         (`FLOW_CTL | 5'b00000)
`define BA          (`FLOW_CTL | 5'b10000)
`define BEQ         (`FLOW_CTL | 5'b10001)
`define BNE         (`FLOW_CTL | 5'b10011)

`define NOP         (`MISC | 5'b11111)

reg [15:0] instr;

wire [7:0] op = instr[15:8];
wire [7:0] addr = instr[7:0];
wire [3:0] s = instr[7:4];
wire [3:0] d = instr[3:0];

wire instr_attach   = (op == `ATTACH);
wire instr_detach   = (op == `DETACH);
wire instr_exit     = (op == `EXIT);
wire instr_ldi      = (op == `LDI);
wire instr_ldcmp    = (op == `LDCMP);
wire instr_clr      = (op == `CLR);
wire instr_mov      = (op == `MOV);
wire instr_ldd      = (op == `LDD);
wire instr_ldal     = (op == `LDAL);
wire instr_ldah     = (op == `LDAH);
wire instr_ldwr     = (op == `LDWR);
wire instr_lddc     = (op == `LDDC);
wire instr_ldmio    = (op == `LDMIO);
wire instr_std      = (op == `STD);
wire instr_start    = (op == `START);
wire instr_reset    = (op == `RESET);
wire instr_waitads  = (op == `WAITADS);
wire instr_waitlock = (op == `WAITLOCK);
wire instr_waitio   = (op == `WAITIO);
wire instr_ready    = (op == `READY);
wire instr_int      = (op == `INT);
wire instr_nmi      = (op == `NMI);
wire instr_hold     = (op == `HOLD);
wire instr_cmp      = (op == `CMP);
wire instr_beq      = (op == `BEQ);
wire instr_bne      = (op == `BNE);
wire instr_ba       = (op == `BA);
wire instr_nop      = (op == `NOP);

// Mem access

`define REG_OFFSET  9'h100


wire mem_load = instr_ldi |
                instr_mov |
                instr_ldcmp |
                instr_cmp |
                instr_std;
wire mem_store = instr_ldi |
                instr_mov |
                instr_ldd |
                instr_ldal |
                instr_ldah |
                instr_ldwr |
                instr_lddc |
                instr_ldmio |
                instr_clr;

// Flow control
//
reg [7:0] pc;
reg [7:0] next_pc;
wire exec_wait = (instr_waitads && _ads) ||
                 (instr_waitlock && (_ads || _lock)) ||
                 (instr_waitio && (_ads || mio)) ||
                 (instr_hold && !hlda) ||
                 (instr_exit);

// Compare

reg [15:0] cmpr;

wire [15:0] source = ({16{mem_load}} & mem_readdata) |
                    ({16{instr_ldd}} & din) |
                    ({16{instr_ldal}} & a) |
                    ({16{instr_ldal}} & (a >> 16)) |
                    (instr_ldwr & wr) |
                    (instr_lddc & dc) |
                    (instr_ldmio & mio);

task reset_ocd;
begin
    stop <= 0;
    state <= `ST_STORE;
    pc <= 8'hFF;
    next_pc <= 0;
    mem_address <= 0;
    mem_write <= 0;
    instr <= 0;
end
endtask

task do_fetch;
begin
    next_pc <= pc + 1;
    instr <= mem_readdata;
    mem_address <= (mem_readdata[15:8] == `LDI) ? pc + 1
                    : (`REG_OFFSET | mem_readdata[7:4] );
    state <= `ST_LOAD;
end
endtask

task do_load;
begin
    state <= `ST_EXEC;
end
endtask

task do_exec;
begin
    if (instr_attach)
        en <= 1;
    if (instr_detach)
        en <= 0;
    if (instr_exit)
        stop <= 1;
    if (instr_start)
    begin
        reset <= 0;
        nmi <= 0;
        intr <= 0;
        hold <= 0;
    end
    if (instr_reset)
        reset <= 1;
    if (instr_ready || instr_waitads || instr_waitlock
        || instr_std)
    begin
        _ready <= 0;
        hold <= 0;
    end
    if (instr_int)
        intr <= 1;
    if (instr_nmi)
        nmi <= 1;
    if (instr_hold)
        hold <= 1;
    if (instr_std)
        dout <= source;
    if (instr_ldcmp)
        cmpr <= source;
    if (instr_cmp)
        cmpr <= source == cmpr;
    if ((instr_beq && cmpr) || (instr_bne && !cmpr) || instr_ba)
        next_pc <= addr;

    if (exec_wait)
        state <= `ST_EXEC;
    else
    begin
        state <= `ST_STORE;
        mem_address <= `REG_OFFSET | d;
        mem_writedata <= source;
        if (mem_store)
            mem_write <= 1;
    end
end
endtask

task do_store;
begin
    if (instr_waitads || instr_waitlock || instr_waitio)
    begin
        _ready <= 1;
        nmi <= 0;
        en <= 1;
    end

    mem_write <= 0;
    state <= `ST_STORE_FETCH;
    pc <= next_pc + instr_ldi;
    mem_address[7:0] <= next_pc + instr_ldi;
    mem_address[8] <= 0; // Select program memory
end
endtask

task do_store_fetch;
begin
    state <= `ST_FETCH;
end
endtask

initial
    reset_ocd();

always @(posedge clk or posedge rst)
begin
    if (rst)
        reset_ocd();
    else
    begin
        case (state)
            `ST_FETCH:          do_fetch();
            `ST_LOAD:           do_load();
            `ST_EXEC:           do_exec();
            `ST_STORE:          do_store();
            `ST_STORE_FETCH:    do_store_fetch();
        endcase
    end
end

endmodule
