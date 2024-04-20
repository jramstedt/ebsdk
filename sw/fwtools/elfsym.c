/*----------------------------------------------------------------------*/
/* A tool for printing the symbols of an ELF image file (like nm)
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
static char *strings, *syms;

static const char *progname = "elfsym";


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

char *getsym( Elf64_Word s )
{
	return syms + s;
}


/* Return the name of a section, given it's Elf64_Section number, an index 
   into the Shdr, with a few special values */
char *getscn( Elf64_Section s )
{
	char *result;

        switch ( s )
	{
	    case 0:		result = "(undefined)"; 	break;
	    case 0xFFF1:	result = "(Absolute value)"; 	break;
	    case 0xFFF2:	result = "(Common value)"; 	break;
	    default:		result = getsname(Shtab[s].sh_name);
	}

	return result;
}


void psymtab( Elf64_Shdr *s )
{
	unsigned nsyms = s->sh_size / s->sh_entsize;
	Elf64_Sym *sym = (Elf64_Sym *)(fileptr + s->sh_offset);
	unsigned i;

	for (i = 0; i < nsyms; i++)
	{
		printf("%-20s: section %s, value %p\n",
			getsym(sym[i].st_name),
			getscn(sym[i].st_shndx),
			sym[i].st_value );
	}
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


	/*----------------------------------------*/
	/* Traverse sections to the useful stuff */

	Shtab = (Elf64_Shdr *)(fileptr + Ehdr->e_shoff);
	strings = (char *)(fileptr + Shtab[Ehdr->e_shstrndx].sh_offset);

	/* find the main string table */
	for (i = 0; i < nshdrs; i++)
		if ( Shtab[i].sh_type == SHT_STRTAB && i != Ehdr->e_shstrndx)
			syms = (char *)(fileptr + Shtab[i].sh_offset);

	for (i = 0; i < nshdrs; i++)
		if ( Shtab[i].sh_type == SHT_SYMTAB )	psymtab( Shtab + i );
}	
