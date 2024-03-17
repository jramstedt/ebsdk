#ifndef __J_H__
#define __J_H__

/*
// Create #defines for the length of the fields in a fetch block.
//
// Match these field orderings with the code in j.mdl (for j_dat_gdp->data_in_n1b_h)
//
// field           Numer of bits
// -----           -------------
// I3              64
// I2              64
// trouble         4
// ea_src          12 ((2 src per array element + 1 stall per array element) * 4 array elements)
// s_c             8 
// dest_valid      4
// I1              64
// I0              64
//
// I0              <0:31>
// I1              <32:63>
// dstv            <64:67>
// s_c             <68:75>
// ea_src          <76:87>
// trouble         <88:91>
// predec          <92:103>
// I2              <104:135>
// I3              <136:167>
// parity          <168:169>
*/

#define J_I0_LSB           0
#define J_I0_MSB           31
#define J_I1_LSB           32
#define J_I1_MSB           63
#define J_I2_LSB           J_I1_MSB + 1 + J_DEC_BITS_PER_FB
#define J_I2_MSB           J_I2_LSB + 31
#define J_I3_LSB           J_I2_MSB +  1
#define J_I3_MSB           J_I3_LSB + 31

#define J_DEST_VALID_BITS  4
#define J_DEST_VALID_LSB   J_I1_MSB + 1
#define J_DEST_VALID_MSB   J_DEST_VALID_LSB + J_DEST_VALID_BITS - 1

#define J_COUT_BITS        4
#define J_COUT_LSB         J_DEST_VALID_MSB + 1
#define J_COUT_MSB         J_COUT_LSB + J_COUT_BITS - 1

/* 8 src bits plus 4 stall bits.  Stall bit affect src code. */
#define J_EA_SRC_BITS      12
#define J_EA_SRC_LSB       J_COUT_MSB + 1
#define J_EA_SRC_MSB       J_EA_SRC_LSB + J_EA_SRC_BITS - 1

/* now there are 8 due to priority encoder */
#define J_TROUBLE_BITS     8
#define J_TROUBLE_LSB      J_EA_SRC_MSB + 1
#define J_TROUBLE_MSB      J_TROUBLE_LSB + J_TROUBLE_BITS - 1

/* 3 per instr * 4 instrs per line. */
#define J_PREDEC_BITS      12
#define J_PREDEC_LSB       J_TROUBLE_MSB + 1
#define J_PREDEC_MSB       J_PREDEC_LSB + J_PREDEC_BITS - 1

/* 2 parity bits (left and right) */
#define J_PARITY_BITS       2
#define J_PARITY_LSB        J_I3_MSB + 1
#define J_PARITY_MSB        J_PARITY_LSB + J_PARITY_BITS - 1

#define J_DEC_BITS_PER_FB  J_DEST_VALID_BITS + J_COUT_BITS + J_EA_SRC_BITS + J_TROUBLE_BITS + J_PREDEC_BITS

#define J_INSTRS_PER_FB     4

#define J_BITS_PER_DATA_FB  J_DEC_BITS_PER_FB + J_INSTRS_PER_FB*32 + J_PARITY_BITS

/*
//  Line predict bits.
//  Until I can get a write mask in the REG_FILE_WRITE() primitives ssp is a separate array.
*/
#define J_LP_IDX_BITS       11
#define J_LP_IDX_LSB        0
#define J_LP_IDX_MSB        J_LP_IDX_LSB + J_LP_IDX_BITS - 1

#define J_LP_SP_BITS        1
#define J_LP_SP_LSB         J_LP_IDX_MSB + 1
#define J_LP_SP             J_LP_SP_LSB
#define J_LP_SP_MSB         J_LP_SP_LSB  + J_LP_SP_BITS - 1

#define J_LP_SSP_BITS       5

#define J_LP_SRC_BITS       3
#define J_LP_SRC_LSB        J_LP_SP_MSB + 1
#define J_LP_SRC_MSB        J_LP_SRC_LSB + J_LP_SRC_BITS- 1

#define J_LP_TRAIN_BITS     1
#define J_LP_TRAIN_LSB      J_LP_SRC_MSB + 1
#define J_LP_TRAIN_MSB      J_LP_TRAIN_LSB + J_LP_TRAIN_BITS - 1

#define J_LP_BITS_PER_FB    J_LP_IDX_BITS + J_LP_SP_BITS + J_LP_SRC_BITS + J_LP_TRAIN_BITS

#define J_LP_LSB           0
#define J_LP_MSB           J_LP_LSB + J_LP_BITS_PER_FB - 1

/*
// TAGS
//
// <47:15> -> <32:0>
*/
#define J_TAG_LSB           0
#define J_TAG_MSB           32
#define J_KSEU_LSB          33
#define J_KSEU_MSB          36
#define J_PAL               37
#define J_ASN_LSB           38
#define J_ASN_MSB           45
#define J_TAG_PAR           46
#define J_ASM               47
#define J_VASMC             48
#define J_TAG_BITS          49

#endif /* NOT defined __J_H__ */
