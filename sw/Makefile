.PHONY: all program run clean
all:
	$(MAKE) -C bios
	$(MAKE) -C test
	$(MAKE) -C bridge
	$(MAKE) -C lib
	$(MAKE) -C monitor

program:
	$(MAKE) -C bridge program
	$(MAKE) -C bios flash

run:
	$(MAKE) -C monitor stop
	$(MAKE) -C bios flash
	$(MAKE) -C monitor sysemu

clean:
	$(MAKE) -C bridge clean
	$(MAKE) -C lib clean
	$(MAKE) -C monitor clean
	$(MAKE) -C test clean
	$(MAKE) -C bios clean
