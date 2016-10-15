%{
    open Parsing
    open Lexing
    open Ast

    let print_error (sp, ep) =
        let line = sp.Lexing.pos_lnum in
        let b = sp.Lexing.pos_cnum - sp.Lexing.pos_bol in
        let e = ep.Lexing.pos_cnum - ep.Lexing.pos_bol in
            Printf.fprintf stderr "Syntax error (line: %d, char: %d-%d)\n"
                line b e;
        exit 2
%}

%token <string> ID
%token <string> DIR
%token <int> INT
%token COMMA
%token COLON
%token EOL
%token EOF

%start main
%type <(Lexing.position) Ast.t> main
%%

main:
    | l=statement* EOF {l}
    | error { print_error ($startpos, $endpos) }
;

%inline
op:
    | i=INT     { Int i }
    | i=ID      { Id i }
;

%inline
label:
    | id=ID COLON       { Some id }
    |                   { None }
;

%inline
args:
    | d=op COMMA s=op   { (d, s) } 
    | d=op              { (d, NoOp) } 
    |                   { (NoOp, NoOp) }
;

%inline
instr:
    | m=ID a=args       { (m, a) }
    |                   { ("", (NoOp, NoOp))}
;

statement:
    | l=label i=instr EOL
    {
        begin
            match i with (m, (d, s)) ->
                Instr
                {
                    label = l;
                    mnem = m;
                    d = d;
                    s = s;
                    op = Ud;
                    offset = 0;
                    loc = $startpos
                }
        end
    }
    | d=DIR a=op* EOL
    {
        Directive
        {
            dir = d;
            args = a;
            loc = $startpos
        }
    }
;
