.PHONY: all clean program

all: supervisor_bsp supervisor/ocd_prgm.h
	$(MAKE) -C supervisor

supervisor/ocd_prgm.h: supervisor/ocd_prgm.s
	$(MAKE) -C ../../../../tools/ocd-as
	../../../../tools/ocd-as/lsepc-ocd-as $< $@

supervisor_bsp: settings.def.bsp
	mkdir -p supervisor_bsp
	cp ./settings.def.bsp ./supervisor_bsp/settings.bsp
	nios2-bsp-generate-files --settings=./supervisor_bsp/settings.bsp  \
		--bsp-dir=./supervisor_bsp/

program: all
	nios2-download -g ./supervisor/supervisor.elf

clean:
	-$(MAKE) -C supervisor clean
	$(RM) -r supervisor_bsp
