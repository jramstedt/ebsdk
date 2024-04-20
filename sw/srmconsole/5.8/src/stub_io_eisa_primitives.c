#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:pb_def.h"
#include "cp$inc:platform_io.h"
#include "cp$inc:kernel_entry.h"

#ifndef EISA_HOSE
#define EISA_HOSE 0
#endif

unsigned char eisa_inportb( unsigned __int64 offset )
    {
    return( inportb( 0, offset ) );
    }

unsigned short eisa_inportw( unsigned __int64 offset )
    {
    return( inportw( 0, offset ) );
    }

unsigned long eisa_inportl( unsigned __int64 offset )
    {
    return( inportl( 0, offset ) );
    }

void eisa_outportb( unsigned __int64 offset, unsigned char data )
    {
    outportb( 0, offset, data );
    }

void eisa_outportw( unsigned __int64 offset, unsigned short data )
    {
    outportw( 0, offset, data );
    }

void eisa_outportl( unsigned __int64 offset, unsigned long data )
    {
    outportl( 0, offset, data );
    }

unsigned char eisa_inmemb( unsigned __int64 offset )
    {
    return( inmemb( 0, offset ) );
    }

unsigned short eisa_inmemw( unsigned __int64 offset )
    {
    return( inmemw( 0, offset ) );
    }

unsigned long eisa_inmeml( unsigned __int64 offset )
    {
    return( inmeml( 0, offset ) );
    }

void eisa_outmemb( unsigned __int64 offset, unsigned char data )
    {
    outmemb( 0, offset, data );
    }

void eisa_outmemw( unsigned __int64 offset, unsigned short data )
    {
    outmemw( 0, offset, data );
    }

void eisa_outmeml( unsigned __int64 offset, unsigned long data )
    {
    outmeml( 0, offset, data );
    }

unsigned long eisa_indmeml( unsigned __int64 offset )
    {
    return( indmeml( 0, offset ) );
    }

unsigned __int64 eisa_indmemq( unsigned __int64 offset )
    {
    return( indmemq( 0, offset ) );
    }

void eisa_outdmeml( unsigned __int64 offset, unsigned long data )
    {
    outdmeml( 0, offset, data );
    }

void eisa_outdmemq( unsigned __int64 offset, unsigned __int64 data )
    {
    outdmemq( 0, offset, data );
    }

unsigned char eisa_incfgb( int bus, int slot, int function, unsigned __int64 offset )
    {
    struct pb pb;

    pb.hose = EISA_HOSE;
    pb.bus = bus;
    pb.slot = slot;
    pb.function = function;
    return( incfgb( &pb, offset ) );
    }

unsigned short eisa_incfgw( int bus, int slot, int function, unsigned __int64 offset )
    {
    struct pb pb;

    pb.hose = EISA_HOSE;
    pb.bus = bus;
    pb.slot = slot;
    pb.function = function;
    return( incfgw( &pb, offset ) );
    }

unsigned long eisa_incfgl( int bus, int slot, int function, unsigned __int64 offset )
    {
    struct pb pb;

    pb.hose = EISA_HOSE;
    pb.bus = bus;
    pb.slot = slot;
    pb.function = function;
    return( incfgl( &pb, offset ) );
    }

eisa_outcfgb( int bus, int slot, int function, unsigned __int64 offset, unsigned char data )
    {
    struct pb pb;

    pb.hose = EISA_HOSE;
    pb.bus = bus;
    pb.slot = slot;
    pb.function = function;
    outcfgb( &pb, offset, data );
    }

eisa_outcfgw( int bus, int slot, int function, unsigned __int64 offset, unsigned short data )
    {
    struct pb pb;

    pb.hose = EISA_HOSE;
    pb.bus = bus;
    pb.slot = slot;
    pb.function = function;
    outcfgw( &pb, offset, data );
    }

eisa_outcfgl( int bus, int slot, int function, unsigned __int64 offset, unsigned long data )
    {
    struct pb pb;

    pb.hose = EISA_HOSE;
    pb.bus = bus;
    pb.slot = slot;
    pb.function = function;
    outcfgl( &pb, offset, data );
    }
