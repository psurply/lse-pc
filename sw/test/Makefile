MIF?=

.PHONY: all clean

all:
	make -C pm/
	make -C real/

clean:
	make -C pm/ clean
	make -C real/ clean

flash: $(MIF)
	../tools/memory flash $<
