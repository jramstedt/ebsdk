INI	= ev6_defs.mar ev6_redefine_defs.mar ledcodes.mar
NAUT	= platform_nautilus.mar
SWORD	= platform_swordfish.mar
TINOSA	= platform_tinosa.mar
BIN = ../bin
HAL = `which hal`

OSF	= target_osf.mar \
	  ev6_pal_temps.mar \
	  ev6_alpha_defs.mar \
	  ev6_osf_pal_defs.mar \
          ev6_system_pal_defs.mar \
	  ev6_system_pal_impure.mar \
	  ev6_pal_macros.mar \
	  ev6_debug_macros.mar \
          ev6_osf_pal.mar \
	  sromcsrv.mar \
	  sromio.mar \
	  ev6_osf_system_pal.mar

DBM	= target_dbm.mar \
	  ev6_pal_temps.mar \
	  ev6_alpha_defs.mar \
	  ev6_osf_pal_defs.mar \
          ev6_dbm_pal_defs.mar \
	  ev6_system_pal_impure.mar \
	  ev6_pal_macros.mar \
	  ev6_debug_macros.mar \
          ev6_osf_pal.mar \
	  sromcsrv.mar \
	  sromio.mar \
	  ev6_osf_system_pal.mar

all: swordfish_osf.pal 

nautilus_dbm.pal:
	$(HAL) -o $(BIN)/$@ -l $@.lis -h EV6 -e ".SHOW MEB" $(INI) $(NAUT) $(DBM)
nautilus_osf.pal:
	$(HAL) -o $(BIN)/$@ -l $@.lis -h EV6 -e ".SHOW MEB" $(INI) $(NAUT) $(OSF)

swordfish_dbm.pal:
	$(HAL) -o $(BIN)/$@ -l $@.lis -h EV6 -e ".SHOW MEB" $(INI) $(SWORD) $(DBM)
swordfish_osf.pal:
	$(HAL) -o $(BIN)/$@ -l $@.lis -h EV6 -e ".SHOW MEB" $(INI) $(SWORD) $(OSF)

tinosa_dbm.pal:
	$(HAL) -o $(BIN)/$@ -l $@.lis -h EV6 -e ".SHOW MEB" $(INI) $(TINOSA) $(DBM)
tinosa_osf.pal:
	$(HAL) -o $(BIN)/$@ -l $@.lis -h EV6 -e ".SHOW MEB" $(INI) $(TINOSA) $(OSF)


clean:
	rm -f core
	rm -f *.pal *.lis

