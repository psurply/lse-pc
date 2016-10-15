type operation =
    (* OCD Control *)
    | Attach
    | Detach
    | Exit

    (* Load / Store *)
    | Ldi   of int * int
    | Mov   of int * int
    | Ldcmp of int
    | Ldd   of int
    | Ldal  of int
    | Ldah  of int
    | Ldwr  of int
    | Lddc  of int
    | Ldmio of int
    | Clr   of int
    | Std   of int

    (* CPU Control *)
    | Start
    | Reset
    | Waitads
    | Waitlock
    | Waitio
    | Ready
    | Hold
    | Int
    | Nmi

    (* Flow Control *)
    | Cmp of int
    | Beq
    | Bne
    | Ba

    (* Misc *)
    | Nop
    | Ud

type op =
    | Int of int
    | Id of string
    | NoOp

type 'loc instr = {
    label               : string option;
    mnem                : string;
    d                   : op;
    s                   : op;

    mutable offset      : int;
    mutable op          : operation;
    loc                 : 'loc;
}

type 'loc directive = {
    dir                 : string;
    args                : op list;
    loc                 : 'loc;
}

type 'loc statement =
    | Instr of 'loc instr
    | Directive of 'loc directive

type 'loc t = ('loc statement) list
