open Ast
open Context
open Error

let getop ctxt loc = function
    | Int i -> i
    | Id i ->
        begin
            try
                Hashtbl.find ctxt.const i
            with Not_found -> err loc (sof "constant not defined: %s" i)
        end
    | NoOp -> failwith "getop"

let analyse_instr ctxts instr =
    let ctxt = !(ctxts.ctxt) in
    let d = instr.d in
    let s = instr.s in
    let loc = instr.loc in
    let op =
        begin
            match instr.mnem with
            | "ATTACH" -> Attach
            | "DETACH" -> Detach
            | "EXIT" -> Exit
            | "LDI" -> Ldi (getop ctxts loc d, getop ctxts loc s)
            | "MOV" -> Mov (getop ctxts loc d, getop ctxts loc s)
            | "LDCMP" -> Ldcmp (getop ctxts loc d)
            | "CLR" -> Clr (getop ctxts loc d)
            | "LDD" -> Ldd (getop ctxts loc d)
            | "LDAL" -> Ldal (getop ctxts loc d)
            | "LDAH" -> Ldah (getop ctxts loc d)
            | "LDWR" -> Ldwr (getop ctxts loc d)
            | "LDDC" -> Lddc (getop ctxts loc d)
            | "LDMIO" -> Ldmio (getop ctxts loc d)
            | "STD" -> Std (getop ctxts loc d)
            | "START" -> Start
            | "RESET" -> Reset
            | "WAITADS" -> Waitads
            | "WAITLOCK" -> Waitlock
            | "WAITIO" -> Waitio
            | "READY" -> Ready
            | "HOLD" -> Hold
            | "INT" -> Int
            | "NMI" -> Nmi
            | "CMP" -> Cmp (getop ctxts loc d)
            | "BEQ" -> Beq
            | "BNE" -> Bne
            | "BA" -> Ba
            | "NOP" -> Nop
            | "" -> Ud
            | x -> err instr.loc (sof "Undefined instruction: %s" x)
        end
    in
    instr.op <- op;
    instr.offset <- ctxt.cursor;
    begin
        match instr.op with
        | Ldi _ -> ctxt.cursor <- ctxt.cursor + 2
        | Ud -> ()
        | _ -> ctxt.cursor <- ctxt.cursor + 1
    end;
    begin
        match instr.label with
        | Some l -> Hashtbl.add ctxt.labels l instr.offset
        | _ -> ()
    end

let dir_define ctxt loc =
    let usage = "Expected: .define [NAME] [VALUE]" in
    function
        | h1::h2::[] ->
            begin
                match h1 with
                | Id i -> Hashtbl.add ctxt.const i (getop ctxt loc h2)
                | _ -> err loc usage
            end
        | _ -> err loc usage

let dir_org ctxt loc =
    let usage = "Expected: .org [INTEGER]" in
    function
        | h::[] ->
            begin
                match h with
                | Int i -> ctxt.cursor <- i
                | _ -> err loc usage
            end
        | _ -> err loc usage

let dir_func ctxts loc =
    let usage = "Expected: .func [NAME]" in
    function
        | h::[] ->
            begin
                match h with
                | Id i ->
                    begin
                        ctxts.ctxt <- ref (create_context ());
                        Hashtbl.add ctxts.funcs i ctxts.ctxt
                    end
                | _ -> err loc usage
            end
        | _ -> err loc usage

let analyse_dir ctxts dir =
    match dir.dir with
    | ".define" -> dir_define ctxts dir.loc dir.args
    | ".org" -> dir_org !(ctxts.ctxt) dir.loc dir.args
    | ".func" -> dir_func ctxts dir.loc dir.args
    | x -> err dir.loc (sof "Unimplemented directive: %s" x)

let analyse_statement ctxts = function
    | Instr i -> analyse_instr ctxts i
    | Directive d -> analyse_dir ctxts d

let analyse ctxts l =
    List.iter (analyse_statement ctxts) l
