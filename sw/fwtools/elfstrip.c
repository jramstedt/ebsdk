/*----------------------------------------------------------------------*/
/* A tool for converting diagnostic code in ELF format to raw format 
 *
 * (c) 1999 Alpha Processor Inc, begun 25 January 1999 by Stig Telfer */
/*----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>



/*----------------------------------------------------------------------*/
/* Data definitions */

static char *fileptr;
static Elf64_Ehdr *Ehdr;
static Elf64_Shdr *Shtab;
static Elf64_Phdr *Phtab;
static unsigned nshdrs, nphdrs;
static char *strings;


static const char *progname = "elfstrip";


static void myexit(const char *msg)
{
	if (msg)	fprintf(stderr,msg);
	fprintf(stderr, "usage: %s <input> <section name> <output>\n", 
			progname);
	exit(-1);
}



/* Get the name of a section */
char *getsname( Elf64_Word s )
{
	return strings + s;
}


int main(int argc, char *argv[])
{
	struct stat statdata;

	char *ifile, *ofile;		/* input filename, output filename */
	char *scnname;
	int ifd, ofd;
	char *wptr;
	size_t wsize;

	Elf64_Shdr *Shdr;
	Elf64_Phdr *Phdr;

	ssize_t sval;
	off_t	oval;

	int i;

	/*----------------------------------------*/
	/* validate arguments, open files */

	if (argc != 4)	
		myexit("Error: need input file, section and output file\n");

	ifile=argv[1];
	if (stat(ifile, &statdata) == -1) {
		perror("stat of input file");
		myexit("Input file could not be verified\n");
	}

	ifd = open(ifile, O_RDONLY, S_IRWXU);
	if (ifd == -1) {
		perror("open of input file");
		myexit("Input file could not be opened\n");
	}

	scnname = argv[2];

	ofile=argv[3];
	ofd = open(ofile, O_CREAT | O_WRONLY);
	if (ofd == -1) {
		perror("open/creation of output file");
		myexit("Output file could not be opened/created\n");
	}


        /*----------------------------------------*/
	/* map input file and analyse Ehdr */

	fileptr = mmap(0, statdata.st_size, PROT_READ, MAP_SHARED, ifd, 0);
	/* if (fileptr == -1)
		myexit("Could not memory-map input file\n"); */
	Ehdr = (Elf64_Ehdr *)fileptr;

	if( strncmp(ELFMAG, Ehdr->e_ident, SELFMAG) != 0)
		myexit("Input file not an ELF file\n");
	if(Ehdr->e_ident[EI_CLASS] != ELFCLASS64)
		myexit("ELF input file is not 64-bit\n");
	if(Ehdr->e_ident[EI_DATA] != ELFDATA2LSB)
		myexit("ELF input file is not little-endian\n");
	if(Ehdr->e_type != ET_EXEC)
		myexit("ELF input file is not in executable format\n");
	if(Ehdr->e_machine != EM_ALPHA)
		myexit("ELF input file is not Alpha platform\n");

	nshdrs = Ehdr->e_shnum;
	nphdrs = Ehdr->e_phnum;


	/*----------------------------------------*/
	/* Traverse sections to the one we want */

	Shtab = (Elf64_Shdr *)(fileptr + Ehdr->e_shoff);
	strings = fileptr + Shtab[Ehdr->e_shstrndx].sh_offset;

	for (i = 0; i < nshdrs; i++)
	{
		if ( strcmp( getsname( Shtab[i].sh_name ), scnname ) != 0 )
			continue;		/* not this scn */
	
		wsize = Shtab[i].sh_size;
		wptr = fileptr + Shtab[i].sh_offset;
		printf("Found section %s (%d bytes)\n", scnname, wsize );

		if ( write(ofd, wptr, wsize) != wsize )
		{
			perror("writing of output section");
			myexit("Couldn't write output section\n");
		}

		break;		/* done! */
	}

	close(ifd);
	close(ofd);
}	
