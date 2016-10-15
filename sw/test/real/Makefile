MIFS = led.mif \
       pusha.mif \
       extram.mif \
       serial.mif \
       loadall.mif
BIN = $(MIFS:%.mif=%.bin)

BIN2MIF = ../../tools/bin2mif
ASM = yasm

.PHONY: all clean

all: $(MIFS)

%.bin: %.asm
	$(ASM) -f bin -o $@ $<

%.mif: %.bin ../reset.bin
	$(BIN2MIF) -o $@ -b 0xF8000 0 \
	    -i $< 0xF8000 0 \
	    -i ../reset.bin 0xFFFF0 0 \

clean:
	$(RM) $(BIN) $(MIFS)
