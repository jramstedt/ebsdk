# Makefile for UnZip 5.3 using QDOS/xtc68 and GNU make
#
#      This makefile is a makefile for the xtc68 cross-compiler on Linux;
#      conversion to a native c68 makefile should be trivial. This builds
#      native QDOS binaries.
#
# Last modified:  4 Feb 97

include /etc/ql.mak
CFLAGS = -O -I. -I./qdos/

OBJS = unzip.o process.o crypt.o envargs.o explode.o \
         unshrink.o extract.o zipinfo.o inflate.o match.o unreduce.o \
         ttyio.o fileio.o list.o globals.o crctab.o qdos.o config.o crc32.o

FOBJS = funzip.o ttyio.o crypt.fo inflate.fo globals.fo config.o crc32.o qdos.fo

XOBJS = unzip.xo process.xo crypt.o extract.xo fileio.o \
        ttyio.o inflate.o match.o globals.o crctab.o qdos.xo config.o crc32.o

all : unzip unzipsfx funzip makesfx

# xtc68/cc is brain dead !!

%.xo : %.c
	qcpp -DSFX -I. -I./qdos/ $*.c $*.i
	c68 -O $*.i $*.s
	as68 $*.s $*.xo
	rm -f $*.s $*.i

%.fo : %.c
	qcpp -DFUNZIP -I. -I./qdos/ $*.c $*.i
	c68 -O $*.i $*.s
	as68 $*.s $*.fo
	rm -f $*.s $*.i

u : unzip

f : funzip

x : unzipsfx

m : makesfx

unzip :     $(OBJS)
	$(LD) $(LDFLAGS) -o$@ $(OBJS) $(LDLIBS)

unzipsfx :  $(XOBJS)
	$(LD) $(LDFLAGS) -o$@ $(XOBJS) $(LDLIBS)

funzip :    $(FOBJS)
	$(LD) $(LDFLAGS) -o$@ $(FOBJS) $(LDLIBS)

makesfx : qdos/makesfx.c
	cp qdos/makesfx.c .
	$(CC) $(CFLAGS) -o makesfx makesfx.c
	rm -f makesfx.c

clean :
	rm *.o *.xo *.fo

cleaner : clean
	rm unzip funzip unzipsfx makesfx


$(OBJS) $(XOBJS) $(FOBJS) : unzip.h unzpriv.h globals.h \
                             qdos/izqdos.h

crypt.o crypt.fo ttyio.o ttyio.fo : zip.h

inflate.o inflate.fo : inflate.h

fileio.o : ebcdic.h
funzip.o : tables.h

crypt.o crypt.fo ttyio.o ttyio.fo unzip.o unzip.xo funzip.o : crypt.h
fileio.o extract.o extract.xo inflate.o inflate.fo : crypt.h

crypt.o crypt.fo ttyio.o ttyio.fo funzip.o fileio.o : ttyio.h

unzip.o unzip.xo : version.h consts.h


# Special case object files:

qdos.o: qdos/qdos.c
	cp qdos/qdos.c .
	$(CC) -c -oqdos.o $(CFLAGS) $*.c
	rm -f qdos.c

config.o: qdos/config.S
	cp qdos/config.S .
	$(CC) -c $*.S
	rm -f config.S

qdos.xo: qdos/qdos.c
	cp qdos/qdos.c .
	qcpp -DSFX -I. -I./qdos/ qdos.c qdos.i
	c68 qdos.i qdos.s
	as68 qdos.s qdos.xo
	rm -f qdos.s qdos.i
	rm -f qdos.c

qdos.fo: qdos/qdos.c
	cp qdos/qdos.c .
	qcpp -DFUNZIP -I. -I./qdos/ qdos.c qdos.i
	c68 qdos.i qdos.s
	as68 qdos.s qdos.fo
	rm -f qdos.s qdos.i
	rm -f qdos.c
