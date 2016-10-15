let sof = Printf.sprintf

let err sp s = 
    let line = sp.Lexing.pos_lnum in
    Printf.fprintf stderr "line %d: %s\n" line s;
    exit 2
