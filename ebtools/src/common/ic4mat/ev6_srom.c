#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ic4mat.h"
#include "ev6_ic.h"

#define NUM_WRITE_ONCE_BITS 367

FILE* ofil;
int binary_output;

void print_ldic_error(struct ldic_config* cnfg_p)
{
    if (cnfg_p->error == ldicNoError)
        return;

    switch (cnfg_p->error) {
    case ldicErrCrossBlk:
        fprintf(stderr, "%E-ICache, write_ic_line cannot cross an ic block (4 LWs).\n");
        fprintf(stderr, "%%E-ICache, num_instr = %d, pc = 0x%lx", cnfg_p->num_instr, cnfg_p->pc);
        fprintf(stderr, "%%E-ICache, pc<3:2> = %d only leaves %d instructions in icache block.\n",
               ((cnfg_p->pc >> 2) & 0x3), (4 - ((cnfg_p->pc >> 2) & 0x3)));
        break;
    case ldicWarnBlkValid:
        fprintf(stderr, "%%W-ICache, block already valid ");
               
        if (cnfg_p->set & ldicOtherSetIfValid)
            fprintf(stderr, "in both sets with different tag(s): set 0 address = 0x%lx, set 1 address = 0x%lx\n",
                   cnfg_p->vaddr[0], cnfg_p->vaddr[1]);
        else
            fprintf(stderr, "in set %d with different tag: address = 0x%lx\n",
                   cnfg_p->set_data, cnfg_p->vaddr[cnfg_p->set_data]);
        fprintf(stderr, "%%W-ICache, will overwrite set %d with address = 0x%lx\n",
               cnfg_p->set_data, cnfg_p->pc);
        break;
    default:
        fprintf(stderr, "%%E-ICache, unknown error reported (%d).", cnfg_p->error);
        break;
    }

    // Clear error so later print does not print it again.
    //
    cnfg_p->error = ldicNoError;
}

static int nbits = 0;
static MTV_uint64 leftover_data;

void dump_binary_bits(FILE* outputfile, int numbits, MTV_uint64* arr_p)
{
    int arrcnt = 0;
    int curbit = 0;

/*     int cnt; */
/*     fprintf(stderr, "incoming: "); */
/*     for (cnt = 0; cnt < numbits; cnt += 64) */
/*         fprintf(stderr, "%016lx ", arr_p[cnt / 64]); */
/*     fprintf(stderr, "\n"); */
/*     fflush(stderr); */
    
    while (curbit != numbits) {
        MTV_uint64 mask;
        int bits_to_do = 64 - nbits;			/* number of bits to fill leftover_data */
        int array_bits_to_do = 64 - (curbit % 64);	/* number of bits remaining in current array ele */

        /* check that the current array element isn't a partial last
           element.  if it is then adjust array_bits_to_do to not be
           more the the ending bit.
           */
        if ((array_bits_to_do + curbit) > numbits)
            array_bits_to_do = numbits - curbit;

        /* do smaller of the two bits_to_do. */
        if (array_bits_to_do < bits_to_do)
            bits_to_do = array_bits_to_do;

        /* calculate mask */
        if (bits_to_do == 64)
            mask = -1;
        else {
            mask = 1;
            mask <<= bits_to_do;
            mask -= 1;
        }

/*         fprintf(stderr, "#bits: %d, mask: %016lx\n", bits_to_do, mask); */
    
        leftover_data |= ((arr_p[arrcnt] >> (curbit % 64)) & mask) << nbits;
        nbits += bits_to_do;

        if (nbits == 64) {
            int nele;
            
            nele = fwrite(&leftover_data, 1, sizeof(leftover_data), outputfile);

            nbits = 0;
            leftover_data = 0;
        }
        
        curbit += bits_to_do;
        if ((curbit % 64) == 0) {
            arrcnt += 1;
        }
    }

    return;
}

void reverse_bits(int numbits, MTV_uint64 *arr_p)
{
    int numele = (numbits / 64) + ((numbits % 64) != 0);
    MTV_uint64* temp_p = (MTV_uint64*)malloc(numele * sizeof(MTV_uint64));
    int cnt;

/*     fprintf(stderr, "incoming(%d:%d): ", numbits, numele); */
    
    for (cnt = 0; cnt < numele; ++cnt) {
/*         fprintf(stderr, "%016lx ", arr_p[cnt]); */
        temp_p[cnt] = arr_p[cnt];
        arr_p[cnt] = 0;
    }

/*     fprintf(stderr, "\n"); */
/*     fflush(stderr); */
    
    for (cnt = (numbits - 1); cnt >= 0; --cnt) {
        MTV_uint64 this_bit = (temp_p[cnt / 64] >> (cnt % 64)) & 1;
        int newpos = numbits - cnt - 1;

        arr_p[newpos / 64] |= this_bit << (newpos % 64);
    }    

/*     fprintf(stderr, "outgoing: "); */
/*     for (cnt = 0; cnt < numbits; cnt += 64) */
/*         fprintf(stderr, "%016lx ", arr_p[cnt / 64]); */
/*     fprintf(stderr, "\n"); */
/*     fflush(stderr); */

    free(temp_p);
}

void flush_binary_bits()
{
    fwrite(&leftover_data, sizeof(leftover_data), 1, ofil);
    nbits = 0;
    leftover_data = 0;
    fflush(ofil);
}

void print_srom_data(struct ldic_config* cnfg_p,
                     MTV_uint64 addr, int set,
                     MTV_uint32 addr_14_3,
                     MTV_uint64 srom_192, MTV_uint64 srom_191_128, MTV_uint64 srom_127_64, MTV_uint64 srom_63_0)
{
    if (!binary_output) 
        fprintf(ofil,
                "ICACHE_SHIFT_DATA[0x%03x](192,0) = 0x%d%016lx%016lx%016lx\n",
                addr_14_3, srom_192, srom_191_128, srom_127_64, srom_63_0);
    else {
        MTV_uint64 arr[4];

        arr[0] = srom_63_0;
        arr[1] = srom_127_64;
        arr[2] = srom_191_128;
        arr[3] = srom_192;

        reverse_bits(193, arr);
        dump_binary_bits(ofil, 193, arr);
    }
}

int process_ev6_srom(datum_t* write_once_p)
{
    char* input_fname;
    struct ldic_config ldic_cnfg;
    MTV_uint64 base_address = 0;
    MTV_uint64 max_address = 65535;
    MTV_int64 cur_address;
    int ctr, argn;

    ofil = stdout;		/* by default send output to stdout. */
    binary_output = 0;		/* by default provide ascii output. */

    ldic_cnfg.filler_lw = 0x0baddeed;

    ldic_cnfg.asm_bit = 1;
    ldic_cnfg.asn_value = 0;
    ldic_cnfg.kseu_value = 0xf;
    ldic_cnfg.pal_bit = 1;
    ldic_cnfg.call_pal_r23 = 0 /*???*/;

    ldic_cnfg.set = ldicOtherSetIfValid;
    ldic_cnfg.debug = 1;
    ldic_cnfg.print_error_p = print_ldic_error;
    ldic_cnfg.print_srom_data_p = print_srom_data;
    ldic_cnfg.enable_range = 0;

    for (ctr = 0; ctr < 512; ++ctr)
        ldic_cnfg.trouble_array[ctr] = 0;

    ldic_cnfg.error = ldicNoError;

#define bit2lw(width)	((width >> 5) + ((width & 0x1f) != 0))
    
    ldic_cnfg.data_p = calloc(bit2lw(J_BITS_PER_DATA_FB) * J_NUM_INDEX * J_NUM_BANKS, sizeof(MTV_uint32));
    ldic_cnfg.lp_data_p = calloc(bit2lw(J_LP_BITS_PER_FB) * J_NUM_INDEX * J_NUM_BANKS, sizeof(MTV_uint32));
    ldic_cnfg.lp_ssp_p = calloc(bit2lw(5) * J_NUM_INDEX * J_NUM_BANKS, sizeof(MTV_uint32));
    ldic_cnfg.s0_tag_lay_p = calloc(bit2lw(34) * J_NUM_INDEX, sizeof(MTV_uint32));
    ldic_cnfg.s1_tag_lay_p = calloc(bit2lw(34) * J_NUM_INDEX, sizeof(MTV_uint32));
    ldic_cnfg.s0_tag_ray_p = calloc(bit2lw(34) * J_NUM_INDEX, sizeof(MTV_uint32));
    ldic_cnfg.s1_tag_ray_p = calloc(bit2lw(34) * J_NUM_INDEX, sizeof(MTV_uint32));

#ifdef _WIN32
    ofil = fopen(outfile.fname, "wb");
#else
    ofil = fopen(outfile.fname, "w");
#endif

    if (ofil == NULL) {
      fprintf(stderr, "%%E-srom, unable to open output file: %s.\n", outfile.fname);
      exit(EXIT_FAILURE);
    }

    binary_output = ! _SEEN('d');

    input_fname = dxefile.fname;
    
    if (getopt_option_b != NULL) {
        char* addr_str_p = getopt_option_b;
        int radix = 10;

        if ((strncmp(addr_str_p, "0x", 2) == 0) ||
	    (strncmp(addr_str_p, "0X", 2) == 0)) {
            radix = 16;
            addr_str_p += 2;
        }

        base_address = strtol(addr_str_p, (char**)NULL, radix);
    }

    if (getopt_option_m != NULL) {
	int max = strtol(getopt_option_m, (char**)NULL, 0);
	if( max <= 0 ) /* check for a relative size based on the size of the source file. */
	{
	    struct stat st;
	    stat( dxefile.fname, &st );
	    max_address = st.st_size + max;
	}
	else
	    max_address = max;
    }

    ldic_load_exe(&ldic_cnfg, base_address, input_fname);

    if (!binary_output) {
        
        fprintf(ofil, "SROM Dump File for %s\n\n", input_fname);
        
        fprintf(ofil, "*** Icache Shift Address shifts right through LSB ***\nICACHE_SHIFT_ADDR(11,0) = 0x%03x\n\n", max_address >> 4);
        
        fprintf(ofil, "*** Icache Shift Data shifts left through MSB ***\n");
    
    } else {
        /*MTV_uint64* write_once_p = (MTV_uint64*)calloc((NUM_WRITE_ONCE_BITS >> 6) + ((NUM_WRITE_ONCE_BITS & 63) != 0), 8);*/

        /* placeholder for write_once chain */

        reverse_bits(NUM_WRITE_ONCE_BITS, (MTV_uint64*) write_once_p);
        dump_binary_bits(ofil, NUM_WRITE_ONCE_BITS, (MTV_uint64*) write_once_p);

        /* icache shift address */
        write_once_p[0] = max_address >> 4;

        /* no reverse needed */
        dump_binary_bits(ofil, 12, (MTV_uint64*) write_once_p);
    }
    
    for (cur_address = max_address; cur_address >= 0; cur_address -= 16)
        ldic_output_srom_data(&ldic_cnfg, (cur_address & 0x8000) != 0, cur_address);

    if (binary_output)
        flush_binary_bits();
    
    fclose(ofil);
    
    free(ldic_cnfg.data_p);
    free(ldic_cnfg.lp_data_p);
    free(ldic_cnfg.lp_ssp_p);
    free(ldic_cnfg.s0_tag_lay_p);
    free(ldic_cnfg.s1_tag_lay_p);
    free(ldic_cnfg.s0_tag_ray_p);
    free(ldic_cnfg.s1_tag_ray_p);

    return(0);
}
