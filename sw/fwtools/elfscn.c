/*----------------------------------------------------------------------*/
/* A tool for listing the sections of an ELF file in Alpha-Linux format
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

static const char *progname = "elfscn";


static void myexit(const char *msg)
{
	if (msg)	fprintf(stderr,msg);
	fprintf(stderr, "usage: %s <input file>\n", progname);
	exit(-1);
}



/* Get the name of a section */
char *getsname( Elf64_Word s )
{
	return strings + s;
}	

void pshdr( Elf64_Shdr *s )
{
	printf(	"%s:\n" 
		"\ttype: %d\n"
		"\tvaddr: %p\n"
		"\tfaddr: %p\n"
		"\tsize: 0x%X\n",
		getsname(s->sh_name), s->sh_type, s->sh_addr, s->sh_offset,
		s->sh_size );
}

void pphdr( Elf64_Phdr *p )
{
	printf( "type %d:\n"
		"\tfaddr %p\n"
		"\tvaddr %p\n"
		"\tpaddr %p\n"
		"\tfilesize %d\n"
		"\tmemsize %d\n",
		p->p_type, p->p_offset, p->p_vaddr, p->p_paddr, p->p_filesz, p->p_memsz );
}
		


int main(int argc, char *argv[])
{
	struct stat statdata;

	char *ifile;		/* input filename, input file descriptor */
	int ifd;

	Elf64_Shdr *Shdr;
	Elf64_Phdr *Phdr;

	ssize_t sval;
	off_t	oval;

	int i;


	/*----------------------------------------*/
	/* validate arguments, open files */

	if (argc != 2)	myexit("Error: specify input file\n");

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

	printf( "Ehdr:\n"
		"\t%d phdrs (off %p)\n"
		"\t%d shdrs (off %p)\n"
		"\tshstrndx = %d\n",
		nphdrs, Ehdr->e_phoff,
		nshdrs, Ehdr->e_shoff,
		Ehdr->e_shstrndx);


	/*----------------------------------------*/
	/* Traverse sections to the useful stuff */

	Shtab = (Elf64_Shdr *)(fileptr + Ehdr->e_shoff);
	strings = (char *)(fileptr + Shtab[Ehdr->e_shstrndx].sh_offset);

	for (i = 0; i < nshdrs; i++)
		pshdr(Shtab + i);


	Phtab = (Elf64_Phdr *)(fileptr + Ehdr->e_phoff);

	for (i = 0; i < nphdrs; i++)
		pphdr(Phtab + i);

}	
