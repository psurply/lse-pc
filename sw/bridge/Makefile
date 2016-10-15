PROJECT = lse-pc_bridge
TOP_LEVEL_ENTITY = top_level
RTL_DIR = rtl/
RTL = top_level.v \
	pll.qip \
	pll.v \
	memory_control.v \
	internal_ram.v \
	bus_control.v \
	supervisor.v \
	supervisor/supervisor_qsys.qsys \
	ocd.v \

RTL := $(addprefix $(RTL_DIR), $(RTL)) \
	stp/cpu_signals.stp

FAMILY = "Cyclone IV E"
PART = EP4CE22E22C7

.PHONY: all clean quartus program map fir asm sta smart

all: quartus
	$(MAKE) -C rtl/supervisor/software

clean:
	$(MAKE) -C rtl/supervisor/software clean
	$(RM) -r *.rpt *.chg smart.log *.htm *.eqn *.pin \
		 *.sof *.pof db incremental_db \
		 *.fit.smsg *.summary *.qdf \
		 *.summary *.jdi *.done *qws *.smsg *.sld


quartus: smart.log $(PROJECT).asm.rpt $(PROJECT).sta.rpt

program: all $(PROJECT).sof
	quartus_pgm --no_banner --mode=jtag -o "P;$(PROJECT).sof"
	$(MAKE) -C rtl/supervisor/software program

map: smart.log $(PROJECT).map.rpt
fit: smart.log $(PROJECT).fit.rpt
asm: smart.log $(PROJECT).asm.rpt
sta: smart.log $(PROJECT).sta.rpt
smart: smart.log

MAP_ARGS = --read_settings_files=on $(addprefix --source=,$(RTL))
FIT_ARGS = --part=$(PART) --read_settings_files=on
ASM_ARGS =
STA_ARGS =

STAMP = echo done >

$(PROJECT).map.rpt: map.chg $(RTL)
	quartus_map $(MAP_ARGS) $(PROJECT)
	$(STAMP) fit.chg

$(PROJECT).fit.rpt: fit.chg $(PROJECT).map.rpt
	quartus_fit $(FIT_ARGS) $(PROJECT)
	$(STAMP) asm.chg
	$(STAMP) sta.chg

$(PROJECT).asm.rpt: asm.chg $(PROJECT).fit.rpt
	quartus_asm $(ASM_ARGS) $(PROJECT)

$(PROJECT).sta.rpt: sta.chg $(PROJECT).fit.rpt
	quartus_sta $(STA_ARGS) $(PROJECT)

smart.log: $(PROJECT_SETTINGS)
	quartus_sh --determine_smart_action $(PROJECT) > smart.log

map.chg:
	$(STAMP) map.chg
fit.chg:
	$(STAMP) fit.chg
sta.chg:
	$(STAMP) sta.chg
asm.chg:
	$(STAMP) asm.chg
