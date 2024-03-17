#include "a.out.h"
#include "reloc.h"
#include <sys/file.h>

char *malloc();

main(argc,argv)
     char **argv;
{
  int f;

  f = open(argv[1], O_RDONLY, 0);
  printit(f);
  close(f);
}

printit(f)
     int f;
{
  struct {
    struct filehdr f;
    struct aouthdr a;
  } header;
  struct scnhdr scn;
  int i, off;

  if (read(f, &header, sizeof(header)) != sizeof(header))
    exit(1);
  off = sizeof(header);
  printf("%d sections in file, gp_value = %lx\n",
	 header.f.f_nscns, header.a.gp_value);

  for (i = 0; i < header.f.f_nscns; i++) {
    read(f, &scn, sizeof(scn));
    printrelocs(f, &scn);
    off += sizeof(scn);
    lseek(f, (long)off, 0);
  }
}

printrelocs(f, scnp)
     struct scnhdr *scnp;
{
  int i;
  struct reloc *r;
  char *dat;

  printf("Section %8.8s:\n", scnp->s_name);
  printf("\tpaddr %lx vaddr %lx size %lx ptr %lx\n",
	 scnp->s_paddr, scnp->s_vaddr, scnp->s_size, scnp->s_scnptr);
  printf("\t%d relocs @ %lx  [%x]\n",
	 scnp->s_nreloc, scnp->s_relptr, scnp->s_flags);

  dat = malloc(scnp->s_size);
  lseek(f, scnp->s_scnptr, 0);
  if (read(f, dat, scnp->s_size) != scnp->s_size) exit(2);

  i = scnp->s_nreloc * sizeof(*r);
  r = (struct reloc *) malloc( i );
  lseek(f, scnp->s_relptr, 0);
  if (read(f, r, i) != i) exit(2);

  for (i = 0; i < scnp->s_nreloc; i++) {
    print_a_reloc(r[i]);
    printf("\t[%x]\n", *(int *)(dat + r[i].r_vaddr - scnp->s_vaddr));
  }

  free(r); free(dat);
}

print_a_reloc(r)
     struct reloc r;
{
  static char *r_sn_name[] = {
    "NULL",
    "TEXT",
    "RDATA",
    "DATA",
    "SDATA",
    "SBSS",
    "BSS",
    "INIT",
    "LIT8",
    "LIT4",
    "XDATA",
    "PDATA",
    "FINI",
    "LITA",
    "ABS",
  };

  static char *r_type_name[] = {
    "ABS",
    "REFLONG",
    "REFQUAD",
    "GPREL32",
    "LITERAL",
    "LITUSE",
    "GPDISP",
    "BRADDR",
    "HINT",
    "SREL16",
    "SREL32",
    "SREL64",
    "OP_PUSH",
    "OP_STORE",
    "OP_PSUB",
    "OP_PRSHIFT",
  };

  if (r.r_extern || (r.r_symndx > MAX_R_SN))
    printf("\t    %lx: %x %s %d %x",
	   r.r_vaddr, r.r_symndx, r_type_name[r.r_type],
	   r.r_offset, r.r_size);
  else
    printf("\t    %lx: %s %s %d %x",
	   r.r_vaddr, r_sn_name[r.r_symndx], r_type_name[r.r_type],
	   r.r_offset, r.r_size);
}

