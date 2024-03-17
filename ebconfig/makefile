all:	nt_conf.exe setup.exe

nt_conf.exe:	conf.c targets.h
	cl conf.c /Fent_conf.exe

setup.exe:	setup.c
	cl -c setup.c
	link setup.obj advapi32.lib -out:setup.exe

clean:
	del *.obj
	del *.exe

