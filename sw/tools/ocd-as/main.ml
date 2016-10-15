open Context

let parse_file f =
    let cin = open_in f in
    let lexbuf = Lexing.from_channel cin in
    let ast = Parser.main Lexer.token lexbuf in
    let ctxts = create_contexts () in
    Analysis.analyse ctxts ast;
    Synth.synth ctxts ast;
    close_in_noerr cin;
    ctxts

let output_to_file f ctxts =
    let cout = open_out f in
    Cheader.output_cheader cout "OCD_PRGM" ctxts;
    close_out_noerr cout

let main () =
    if (Array.length Sys.argv) <> 3 then
    begin
        Printf.fprintf stderr "USAGE: %s INPUT OUTPUT\n" Sys.argv.(0);
        exit 2
    end;
    let ctxts = parse_file Sys.argv.(1) in
    output_to_file Sys.argv.(2) ctxts;
    exit 0

let _ = main ()
