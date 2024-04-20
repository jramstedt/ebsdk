MAKEROM = makerom

TARGETS = nautilus swordfish tinosa

TARGETS_DBM = $(TARGETS:%=%_dbm.pal)
TARGETS_OSF = $(TARGETS:%=%_osf.pal)

BIN = $(shell echo `pwd`/bin)

ifndef HAL
	HAL = hal
endif

.PHONY: nautilus swordfish dbm osf all

all:
	$(MAKE) -C src HAL=$(HAL) BIN=$(BIN) $(TARGETS_DBM) $(TARGETS_OSF)


# build diagnostic firmware PALcode for all targets
dbm:
	$(MAKE) -C src HAL=$(HAL) BIN=$(BIN) $(TARGETS_DBM)


# build Linux/OSF PALcode for all targets
osf:
	$(MAKE) -C src HAL=$(HAL) BIN=$(BIN) $(TARGETS_OSF)


nautilus: bin/nautilus_dbm.pal bin/nautilus_osf.pal bin/nautilus.rom

swordfish: bin/swordfish_dbm.pal bin/swordfish_osf.pal bin/swordfish.rom

tinosa: bin/tinosa_dbm.pal bin/tinosa_osf.pal bin/tinosa.rom

bin/%.pal:
	$(MAKE) -C src HAL=$(HAL) BIN=$(BIN) $(@:bin/%=%)

bin/%.rom: bin/%_osf.pal
	$(MAKEROM) -i"OSFPAL" -s$(@:bin/%.rom=%) -l200000 $< -o $@

clean:
	$(RM) $(TARGETS_DBM:%=bin/%) $(TARGETS_OSF:%=bin/%)
	$(RM) $(TARGETS:%=bin/%.rom)
	$(MAKE) -C src clean

