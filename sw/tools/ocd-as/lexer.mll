{
    open Parser
    let print_lexer_error c =
        Printf.fprintf stderr "Unknown token : %c\n" c;
        exit 2

  let nl lexbuf =
    let p = lexbuf.Lexing.lex_curr_p in
      lexbuf.Lexing.lex_curr_p <- {
        p with
          Lexing.pos_lnum = p.Lexing.pos_lnum + 1;
          Lexing.pos_bol  = p.Lexing.pos_cnum;
      }


    let ht_sym = Hashtbl.create 13
    let sym =
        [
            (':',       COLON);
            (',',       COMMA);
        ]
    let _ = List.iter (fun (kwd, tok) -> Hashtbl.add ht_sym kwd tok) sym
}

let white = [' ' '\t']

let ident = ['A'-'Z' 'a'-'z'] ['A'-'Z' 'a'-'z' '0'-'9' '_']*

let integer = ['0'-'9']+ | ("0b" ['0' '1']+) | ("0x" ['0'-'9' 'A'-'F' 'a'-'f']+)

let dir = "." ident

let sym = ',' | ':'

rule token = parse
| white         { token lexbuf }
| '\n'          { nl lexbuf; EOL }
| ";;"          { comment lexbuf }
| integer as i  { INT(int_of_string i) }
| sym as s      { Hashtbl.find ht_sym s }
| ident as i    { ID(i)}
| dir as i      { DIR(i)}
| eof           { EOF }
| _ as c        { print_lexer_error c }

and comment = parse
| '\n'         { nl lexbuf; EOL }
| eof          { EOF }
| _            { comment lexbuf }
