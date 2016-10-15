open Context
open Printf

let output_func cout name ctxt =
    fprintf cout "\nstatic const uint8_t %s[] = {\n" name;
    for i = 0 to !ctxt.cursor - 1 do
        let opcode = !ctxt.prog_mem.(i) in
        fprintf cout "\t0x%02x, 0x%02x,\t/* %x */\n"
             (opcode land 0xFF) (opcode lsr 8) i;
    done;
    fprintf cout "};\n"

let output_cheader cout title ctxts =
    fprintf cout "#ifndef %s_H\n" title;
    fprintf cout "#define %s_H\n\n" title;
    fprintf cout "#include <stdint.h>\n";
    Hashtbl.iter (output_func cout) ctxts.funcs;
    fprintf cout "\n#endif /* %s_H */\n" title
