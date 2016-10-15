OCAMLBUILD = ocamlbuild
BUILDTYPE = byte

EXEC = lsepc-ocd-as
MAIN = main.$(BUILDTYPE)

all:: $(EXEC)

$(EXEC): $(MAIN)
	@mv _build/$^ $@
	@rm $(MAIN)

$(MAIN): $(SRC)
	@$(OCAMLBUILD) $(BUILDFLAGS) $@

clean::
	$(RM) $(EXEC)
	$(OCAMLBUILD) -clean
