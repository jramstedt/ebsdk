/*
 *
 *    FRU50_DEF.H
 *
 */
#include "cp$src:common.h"

typedef struct _fru_name_list {
  unsigned __int64	fru_id;
           char         name[15];
           int          iic_support;
} FRU_NAME_LIST;

