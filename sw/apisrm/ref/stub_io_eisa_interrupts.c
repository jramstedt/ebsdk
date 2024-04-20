eisa_enable_interrupts( int vector )
    {
    io_enable_interrupts( 0, vector );
    }

eisa_disable_interrupts( int vector )
    {
    io_disable_interrupts( 0, vector );
    }

eisa_issue_eoi( int vector )
    {
    io_issue_eoi( 0, vector );
    }
