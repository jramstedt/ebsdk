/*
{ Copyright (c) 1991 by
{ Digital Equipment Corporation, Maynard, Massachusetts.
{ All rights reserved.
{
{ This software is furnished under a license and may be used and copied
{ only  in  accordance  of  the  terms  of  such  license  and with the
{ inclusion of the above copyright notice.  This software or any  other
{ copies thereof may not be provided or otherwise made available to any
{ other person.  No title to and  ownership of the  software is  hereby
{ transferred.
{
{ The information in this software is  subject to change without notice
{ and  should  not  be  construed  as a commitment by Digital Equipment
{ Corporation.
{
{ Digital assumes no responsibility for the use  or  reliability of its
{ software on equipment which is not supplied by Digital.
{
*/

#define prom$toupper (*vector_ptr->_prom$toupper)
#define prom$strcmp (*vector_ptr->_prom$strcmp)
#define prom$strcpy (*vector_ptr->_prom$strcpy)
#define prom$strlen (*vector_ptr->_prom$strlen)
#define prom$strncmp (*vector_ptr->_prom$strncmp)
#define prom$strncat (*vector_ptr->_prom$strncat)
#define prom$strchr (*vector_ptr->_prom$strchr)
#define prom$strrchr (*vector_ptr->_prom$strrchr)
#define prom$strcspn (*vector_ptr->_prom$strcspn)
#define prom$memcpy (*vector_ptr->_prom$memcpy)
#define prom$memcmp (*vector_ptr->_prom$memcmp)
#define prom$read_nvr (*vector_ptr->_prom$read_nvr)
#define prom$write_nvr (*vector_ptr->_prom$write_nvr)
#define prom$calc_fcs (*vector_ptr->_prom$calc_fcs)
#define prom$read_addr_rom (*vector_ptr->_prom$read_addr_rom)
#define prom$calc_addr_rom_cksum (*vector_ptr->_prom$calc_addr_rom_cksum)
#define prom$fprintf (*vector_ptr->_prom$fprintf)
#define prom$sprintf (*vector_ptr->_prom$sprintf)
#define prom$strcat (*vector_ptr->_prom$strcat)
#define prom$strncpy (*vector_ptr->_prom$strncpy)
#define prom$1ms_timer (*vector_ptr->_prom$1ms_timer)
#define prom$calc_bitmap_cksum (*vector_ptr->_prom$calc_bitmap_cksum)
#define prom$calc_signature (*vector_ptr->_prom$calc_signature)
#define prom$find_dma_regs (*vector_ptr->_prom$find_dma_regs)
#define prom$map_dma_buffer (*vector_ptr->_prom$map_dma_buffer)
#define prom$unmap_dma_buffer (*vector_ptr->_prom$unmap_dma_buffer)
#define prom$convert_phys_to_dma_addr (*vector_ptr->_prom$convert_phys_to_dma_addr)
#define prom$convert_dma_to_phys_addr (*vector_ptr->_prom$convert_dma_to_phys_addr)
#define prom$allocate_memory (*vector_ptr->_prom$allocate_memory)
#define prom$free_memory (*vector_ptr->_prom$free_memory)
#define prom$malloc (*vector_ptr->_prom$malloc)
#define prom$free (*vector_ptr->_prom$free)
#define prom$calloc (*vector_ptr->_prom$calloc)
#define prom$check_error (*vector_ptr->_prom$check_error)
#define prom$1us_timer (*vector_ptr->_prom$1us_timer)
#define prom$read_register (*vector_ptr->_prom$read_register)
#define prom$write_register (*vector_ptr->_prom$write_register)

