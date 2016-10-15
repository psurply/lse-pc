type context = {
    labels              : (string, int) Hashtbl.t;
    mutable cursor      : int;
    prog_mem            : int array
}

type contexts = {
    const               : (string, int) Hashtbl.t;
    funcs               : (string, context ref) Hashtbl.t;
    mutable ctxt        : context ref
}

let create_context () =
    {
        labels = Hashtbl.create 13;
        cursor = 0;
        prog_mem = Array.make 256 0
    }

let create_contexts () =
    let ctxts = {
        funcs = Hashtbl.create 13;
        const = Hashtbl.create 13;
        ctxt = ref (create_context ())
    } in
    for i = 0 to 31 do
        Hashtbl.add ctxts.const (Printf.sprintf "R%d" i) i
    done;
    ctxts
