PCB = mobo.pcb

PNG = $(PCB:.pcb=.png)
GERBS = $(PCB:.pcb=.zip)

all:: $(GERBS) $(PNG)

%.zip: %.GTL %.GBL %.GTS %.GBS %.GTO %.GBO %.TXT %.GKO %.GL1 %.GL2
	zip $@ $^

%.GTL: %.top.gbr
	cp $< $@

%.GBL: %.bottom.gbr
	cp $< $@

%.GTS: %.topmask.gbr
	cp $< $@

%.GBS: %.bottommask.gbr
	cp $< $@

%.GTO: %.topsilk.gbr
	cp $< $@

%.GBO: %.bottomsilk.gbr
	cp $< $@

%.TXT: %.plated-drill.cnc
	cp $< $@

%.GKO: %.outline.gbr
	cp $< $@

%.GL1: %.group2.gbr
	cp $< $@

%.GL2: %.group3.gbr
	cp $< $@

%.top.gbr \
%.bottom.gbr \
%.topmask.gbr \
%.bottommask.gbr\
%.topsilk.gbr \
%.bottomsilk.gbr \
%.outline.gbr \
%.group2.gbr \
%.group3.gbr \
%.plated-drill.cnc: %.pcb
	pcb -x gerber --gerberfile $(shell basename -s .pcb $<) \
	    --metric --all-layers $<

%.png: %.pcb
	pcb -x png --outfile $@ --dpi 300 --photo-mode --only-visible \
	    --use-alpha $<

clean::
	$(RM) *.gbr *.cnc $(GERB_FILES) $(GERBS)

%-gerbv: %.GTL %.GBL %.GTS %.GBS %.GTO %.GBO %.TXT %.GKO %.GL1 %.GL2
	gerbv $^

%-png: %.png
	feh $^
