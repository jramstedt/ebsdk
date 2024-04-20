/*----------------------------------------------------------------------*/
/* asmhead - print the first words of an image file disassembled */

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <bfd.h>
#include "dis-asm.h"


static void disasm ( unsigned *address, unsigned iword )
{
    static int my_read_memory ( bfd_vma memaddr, bfd_byte *storehere,
				int length, struct disassemble_info *my_di )
    {
	assert(length==4);
	*(int*)storehere=iword;
	return 0;
    }

    static int my_fprintf (FILE *f, const char *format, ...)
    {
	va_list ap;
	int result;

	va_start (ap, format);
	result = vprintf(format, ap);
	va_end (ap);

	return result;
    }

    static void my_print_address (bfd_vma addr, struct disassemble_info *info)
    {
	printf("0x%04X", (unsigned long)addr);
    }


    struct disassemble_info di;
    di.flavour = bfd_target_elf_flavour;	/* from bfd_flavour */
    di.endian = BFD_ENDIAN_LITTLE;
    di.arch = bfd_arch_alpha;

    di.read_memory_func = my_read_memory;
    di.fprintf_func     = my_fprintf;
    di.print_address_func = my_print_address;

    my_fprintf(NULL, "0x%08lx\t0x%08lx\t\n",address,(unsigned)iword);
    print_insn_alpha ((bfd_vma)address, &di);
}


static void usage( char *name )
{
    fprintf(stderr, "usage: %s <image>\n", name);
    exit(-1);
}


#define NINSTS	10
typedef unsigned int instr;
static instr insts[NINSTS];

int main(int argc, char *argv[] )
{
    int fd;
    int i;

    if ( argc != 2 ) 					usage(argv[0]);
    if ( ( fd = open( argv[1], O_RDONLY, 0 ) ) == -1 ) 	usage(argv[0]);
    if ( read( fd, (void *)insts, NINSTS * sizeof(instr) ) == -1 )
							usage(argv[0]);

    for ( i = 0; i < NINSTS; i++ ) 
	disasm( (unsigned *)(insts + i), insts[i] ); 

    close( fd );
}
