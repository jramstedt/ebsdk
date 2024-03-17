#ifdef _WIN32

#include <io.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#define lseek _lseek
#define open _open

#else

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#endif

#include "coff.h"


main(int argc, char ** argv)
{
    int ifd;
    int ofd;
    struct filehdr fh;
    struct aouthdr ah;
    struct scnhdr sh;
    int i, s;
    char buf[1024];
    int bufsize;
    int iosize;
    unsigned int textbase;
    int	verbose=0;
    char *outfile=(char *)0;
    char *infile=(char *)0;
    int strip_in_place=0;

    for(i = 1; i < argc; i++) {
	if(strcmp(argv[i], "-v") == 0) {
	    verbose=1;
	}

	else if(strcmp(argv[i], "-o") == 0) {
	    /* next arg is output file */
	    if((i+1) == argc) {
		printf("%s: -o flag requires argument\n", argv[0]);
		exit(-1);
	    }
	    outfile=argv[++i];
	}
	else if(argv[i][0] == '-') {
	    printf("%s: Unrecognized flag '%s'\n", argv[0], argv[i]);
	    exit(-1);
	}
	else if(infile) {
	    printf("%s: Only one input file may be specified\n", argv[0]);
	    exit(-1);
	}
	else {
	    infile = argv[i];
	}
    }

    if(outfile == (char *)0) {
	strip_in_place=1;
	outfile=mktemp("/temp/stripXXXXXX");
    }
	

#ifdef _WIN32
    ifd = open(infile, strip_in_place ? O_BINARY|O_RDWR : O_BINARY|O_RDONLY);
#else
    ifd = open(infile, strip_in_place ? O_RDWR : O_RDONLY);
#endif
    if(ifd < 0) {
	perror(infile);
	exit(-1);
    }

#ifdef _WIN32
    ofd = open(outfile, 
	(strip_in_place ? O_RDWR|O_CREAT|O_BINARY|O_TEMPORARY
			: O_RDWR|O_CREAT|O_BINARY), _S_IREAD|_S_IWRITE);
#else
    ofd = open(outfile, O_RDWR|O_CREAT, 0644);
#endif
    if(ofd < 0) {
	perror(outfile);
	exit(-1);
    }

    read(ifd, &fh, sizeof(fh));
    if(verbose) {
        printf("%s: File header: magic 0x%x (0%o), num secns: %d\n",
		argv[1], fh.f_magic, fh.f_magic, fh.f_nscns);
        printf("    Time/date: %s", ctime(&(fh.f_timdat)));
        printf("    Symtab @ 0x%x, nentries 0x%x(%d)\n",
		fh.f_symptr, fh.f_nsyms, fh.f_nsyms);
        printf("    opthdr size: 0x%x, flags 0x%x\n",
		fh.f_opthdr, fh.f_flags);
    }

    if((fh.f_opthdr != 0) && (fh.f_opthdr != sizeof(ah))) {
	printf("Header size mismatch; possible wrong version\n");
	exit(-1);
    }

	if(fh.f_opthdr != 0) {
		read(ifd, &ah, sizeof(ah));
		if(verbose) {
			printf("a.out header: magic 0x%x(%o), vstamp 0x%x\n", 
    			ah.magic, ah.magic, ah.vstamp);
			printf("    tsize: 0x%x (%d)\n", ah.tsize, ah.tsize);
			printf("    dsize: 0x%x (%d)\n", ah.dsize, ah.dsize);
			printf("    bsize: 0x%x (%d)\n", ah.bsize, ah.bsize);
			printf("    entry: 0x%x\n", ah.entry);
			printf("    text_start: 0x%x\n", ah.text_start);
			printf("    data_start: 0x%x\n", ah.data_start);
			printf("    bss_start: 0x%x\n", ah.bss_start);
			printf("    gprmask: 0x%x, fprmask: 0x%x, gp_value 0x%x\n",
				ah.gprmask, ah.fprmask, ah.gp_value);
	    }
	}

    for(i = 0; i < fh.f_nscns; i++) {
	lseek(ifd, sizeof(fh)+fh.f_opthdr+(i*sizeof(sh)), SEEK_SET);
	read(ifd, &sh, sizeof(sh));
	if(verbose) {
	    printf("Section: %s\n", sh.s_name);
	    printf("    paddr: 0x%x, vaddr: 0x%x, size: 0x%x(%d)\n",
		sh.s_paddr, sh.s_vaddr, sh.s_size, sh.s_size);
	    printf("    section @0x%x, reloc @0x%x, linenos @0x%x\n",
		sh.s_scnptr, sh.s_relptr, sh.s_lnnoptr);
	    printf("    num reloc entries: %d, num lnno entries: %d\n",
		sh.s_nreloc, sh.s_nlnno);
	    printf("    section flags: 0x%x\n", sh.s_flags);
	}


	if(i == 0) {
	    if(strcmp(sh.s_name, ".text") != 0) {
		printf("ERROR! First section is not .text!\n");
		exit(-1);
	    }
	    textbase = sh.s_scnptr;
	}

	if(sh.s_scnptr < textbase) {
	    printf("ERROR! Secton %s precedes text!\n", sh.s_name);
	    exit(-1);
	}

	lseek(ifd, sh.s_scnptr, SEEK_SET);
	lseek(ofd, sh.s_scnptr - textbase, SEEK_SET);

	iosize = sh.s_size;
	bufsize = sizeof(buf);

	while(iosize > 0) {
	    s = (iosize < bufsize) ? iosize : bufsize;
	    if(read(ifd, buf, s) != s) {
		perror("input error");
		exit(-1);
	    }
	    if(write(ofd, buf, s) != s) {
		perror("output error");
		exit(-1);
	    }
	    iosize -= s;
	}
    }

    if(strip_in_place) {
	/* Copy bits from output file to input file */
	close(ifd);
	ifd = open(infile, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, _S_IREAD|_S_IWRITE);
	lseek(ofd, 0L, SEEK_SET);
	while((s = read(ofd, buf, bufsize)) > 0) {
	    write(ifd, buf, s);
	}
    }


    close(ifd);
    close(ofd);
}

