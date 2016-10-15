GSCH2PCB = gsch2pcb
DJBOXSYM = djboxsym
REF = refdes_renum
GNETLIST = gnetlist

GENERATE_BOM = scripts/generate_bom.py

SYMBOLS = EP4CE22E22C7N.sym \
	  LM1086.sym \
	  MCP1700.sym \
	  AS6C4008.sym \
	  A29040C.sym \
	  SN74CB3T16210.sym \
	  O10M000000L644.sym \
	  80386SXLP.sym \
	  AS6C8016.sym \
	  FT232R.sym \
	  FT2232D.sym \
	  FT230X.sym \
	  MCP1826.sym \
	  MCP1827.sym \
	  LD1117A.sym \
	  OSC.sym \
	  93LC46B.sym \
	  SST39SF040.sym \
	  M25P16.sym

SCH_MOBO = mobo/cpu.sch \
	   mobo/io.sch \
	   mobo/conf.sch \
	   mobo/power.sch \
	   mobo/memory.sch \
	   mobo/usb.sch

PCB = mobo.pcb

PROJ = $(PCB:.pcb=.prj)

PARTSLIST = partslist.csv
PARTSDB = partsdb.csv
BOM = bom.csv

SYMBOLS := $(addprefix symbols/, $(SYMBOLS))
SCH := $(addprefix sch/, $(SCH_MOBO))
PCB := $(addprefix pcb/, $(PCB))

all:: $(SYMBOLS) $(PCB)

ref:: $(SCH)
	$(REF) $^

%.sym: %.symdef
	$(DJBOXSYM) $< > $@

pcb/%.pcb: %.prj ref
	$(GSCH2PCB) $<

$(PARTSLIST): $(SCH) | $(SYMBOLS)
	$(GNETLIST) -g bom2 -o $@ -Oattribs=device,value,footprint $(SCH)

$(BOM): $(GENERATE_BOM) $(PARTSLIST) $(PARTSDB)
	$(GENERATE_BOM) $(PARTSLIST) $(PARTSDB) > $@
