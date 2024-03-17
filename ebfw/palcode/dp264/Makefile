CONFIG_DIR=../../..
include $(CONFIG_DIR)/sdkmake.conf

PAL_ASM_FILES=ev6_defs.mar ev6_redefine_defs.mar ev6_egore.mar \
	ev6_pal_temps.mar ev6_alpha_defs.mar ev6_vms_pal_defs.mar \
	ev6_system_pal_impure.mar ev6_system_pal_defs.mar \
	ev6_pal_macros.mar ev6_vms_pal.mar ev6_vms_callpal.mar \
	ev6_vms_system_pal.mar

PAL_EB_ASM_FILES=ev6_defs.mar ev6_redefine_defs.mar ev6_pal_temps.mar \
	ev6_alpha_defs.mar ev6_vms_pal_defs.mar ev6_system_pal_impure.mar \
	ev6_system_pal_defs.mar ev6_pal_macros.mar ev6_eb.mar \
	ev6_vms_pal.mar ev6_vms_callpal.mar ev6_vms_system_pal.mar


pal:
	$(EB_TOOLBOX)/hal -o pal.exe -l pal.lis -h EV6 -e ".SHOW MEB" $(PAL_ASM_FILES)

pal_eb:
	$(EB_TOOLBOX)/hal -o pal_eb.exe -l pal_eb.lis -h EV6 -e ".SHOW MEB" $(PAL_EB_ASM_FILES)

dnr:
	defnr -i ev6_pal_temps.dnr -m -o ev6_pal_temps.mar
	defnr -i ev6_system_pal_defs.dnr -m -o ev6_system_pal_defs.mar
	defnr -i ev6_system_pal_impure_p1.dnr -m -o ev6_system_pal_impure_p1.mar
	defnr -i ev6_system_pal_impure.dnr -m -o ev6_system_pal_impure.mar

depend:

clean:
	rm -f core
	rm -f pal.exe palp1.exe pal_eb.exe palp1_eb.exe
	rm -f pal.lis palp1.lis pal_eb.lis palp1_eb.lis

distclean:	clean
