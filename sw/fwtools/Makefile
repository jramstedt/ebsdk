#CC = gcc
CFLAGS = -I. -O
#CFLAGS = -I. -g

FWTOOLS = elfscn elfsym elfstrip sysgen makerom mkdata objstrip xload #asmhead 
FWTOOLS_O = $(FWTOOLS:%=%.o)
FWTOOLS_EXE = $(FWTOOLS:%=bin/%)

.PHONY: hal clean pvc mapcvt
fwtools:  $(FWTOOLS_EXE) hal pvc mapcvt

hal:
	$(MAKE) -C hal

pvc:
	$(MAKE) -C pvc

mapcvt:
	$(MAKE) -C mapcvt

bin/%: %.o
	gcc -o $@ $< -lbfd -liberty -lopcodes

bin/mkdata: mkdata.o
	gcc -o $@ $<

clean:
	rm -f $(FWTOOLS_EXE) $(FWTOOLS_O) $(COMMLIB_O)
	$(MAKE) -C hal clean
	$(MAKE) -C pvc clean
	$(MAKE) -C mapcvt clean
