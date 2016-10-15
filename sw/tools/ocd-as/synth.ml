open Ast
open Context
open Error

let op_ocd_ctl  = (0 lsl 5)
let op_ld_st    = (1 lsl 5)
let op_cpu_ctl  = (2 lsl 5)
let op_flow_ctl = (3 lsl 5)
let op_misc     = (7 lsl 5)

let op_attach  = op_ocd_ctl lor 0b00010
let op_detach  = op_ocd_ctl lor 0b00011
let op_exit    = op_ocd_ctl lor 0b00000

let op_ldi     = op_ld_st lor 0b00000
let op_mov     = op_ld_st lor 0b00001
let op_ldd     = op_ld_st lor 0b00100
let op_ldal    = op_ld_st lor 0b00010
let op_ldah    = op_ld_st lor 0b00011
let op_ldwr    = op_ld_st lor 0b00101
let op_lddc    = op_ld_st lor 0b00110
let op_ldmio   = op_ld_st lor 0b00111
let op_ldcmp   = op_ld_st lor 0b01000
let op_clr     = op_ld_st lor 0b01001
let op_std     = op_ld_st lor 0b10100

let op_start   = op_cpu_ctl lor 0b00000
let op_reset   = op_cpu_ctl lor 0b00001
let op_waitads = op_cpu_ctl lor 0b00010
let op_waitlock= op_cpu_ctl lor 0b01000
let op_waitio  = op_cpu_ctl lor 0b01001
let op_ready   = op_cpu_ctl lor 0b00011
let op_int     = op_cpu_ctl lor 0b00100
let op_nmi     = op_cpu_ctl lor 0b00101
let op_hold    = op_cpu_ctl lor 0b00110

let op_cmp     = op_flow_ctl lor 0b00000
let op_ba      = op_flow_ctl lor 0b10000
let op_beq     = op_flow_ctl lor 0b10001
let op_bne     = op_flow_ctl lor 0b10011

let op_nop     = op_misc lor 0b11111

let opcode i d s = (i lsl 8) lor (((s land 0xF) lsl 4) lor (d land 0xF))
let opcode_addr i addr = (i lsl 8) lor (addr land 0xFF)

let get_addr ctxt loc = function
    | Int i -> i
    | Id i ->
        begin
            try
                Hashtbl.find ctxt.labels i
            with Not_found -> err loc (sof "label not defined: %s" i)
        end
    | NoOp -> err loc "get_addr: NoOp"

let synth_instr ctxt instr =
    let op =
        match instr.op with
        | Attach        -> opcode op_attach 0 0
        | Detach        -> opcode op_detach 0 0
        | Exit          -> opcode op_exit 0 0

        | Ldi (d, s)    ->
                begin
                    ctxt.prog_mem.(instr.offset + 1) <- s;
                    opcode op_ldi d 0
                end
        | Mov (d, s)    -> opcode op_mov d s
        | Ldcmp d       -> opcode op_ldcmp 0 d
        | Ldd d         -> opcode op_ldd d 0
        | Ldal d        -> opcode op_ldal d 0
        | Ldah d        -> opcode op_ldah d 0
        | Ldwr d        -> opcode op_ldwr d 0
        | Lddc d        -> opcode op_lddc d 0
        | Ldmio d       -> opcode op_ldmio d 0
        | Clr d         -> opcode op_clr d 0
        | Std d         -> opcode op_std 0 d

        | Start         -> opcode op_start 0 0
        | Reset         -> opcode op_reset 0 0
        | Waitads       -> opcode op_waitads 0 0
        | Waitlock      -> opcode op_waitlock 0 0
        | Waitio        -> opcode op_waitio 0 0
        | Ready         -> opcode op_ready 0 0
        | Hold          -> opcode op_hold 0 0
        | Int           -> opcode op_int 0 0
        | Nmi           -> opcode op_nmi 0 0

        | Cmp d         -> opcode op_cmp 0 d
        | Ba            -> opcode_addr op_ba (get_addr ctxt instr.loc instr.d)
        | Beq           -> opcode_addr op_beq (get_addr ctxt instr.loc instr.d)
        | Bne           -> opcode_addr op_beq (get_addr ctxt instr.loc instr.d)

        | Nop           -> opcode op_nop 0xFF 0xFF
        | Ud            -> err instr.loc "Undefined instruction"
    in
    ctxt.prog_mem.(instr.offset) <- op

let synth_statement ctxts = function
    | Instr i when i.op != Ud -> synth_instr !(ctxts.ctxt) i
    | Directive d when d.dir = ".func" ->
            begin
                match d.args with
                | (Id i)::[] ->
                begin
                    try
                        ctxts.ctxt <- Hashtbl.find ctxts.funcs i
                    with Not_found -> err d.loc "synth func"
                end
                | _ -> err d.loc "synth func"
            end
    | _ -> ()

let synth ctxts ast = List.iter (synth_statement ctxts) ast
