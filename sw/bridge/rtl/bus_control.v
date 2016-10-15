module bus_control
(
    input clk,

    input _ads,

    output reg _ready,

    output oe,
    output bce
);

`define ST_IDLE     1
`define ST_T1       2
`define ST_T2       3

reg [7:0] state;

assign oe = state == `ST_T2;
assign bce = state != `ST_IDLE;

initial
begin
    _ready <= 0;
    state <= `ST_IDLE;
end

task init_bus_cycle;
    begin
        _ready <= 1;
        state <= `ST_T1;
    end
endtask

task refresh_fsm;
    begin
        case (state)
            `ST_T1:
            begin
                _ready <= 0;
                state <= `ST_T2;
            end
        endcase
    end
endtask

always @(posedge clk)
begin
    if (!_ads)
    begin
        init_bus_cycle();
    end
    else
        refresh_fsm();
end

endmodule
