/* Title:       xload.c                          
 *
 * edit:        ajb   15-Feb-1988
 *              kp    15-May-1989       Synchronize XLOAD command line with the
 *                                      console transition to X I/O processing.
 *                                      Cleanup output for logging purposes.    
 *              jrh   28-Jan-1993       Got this to the point where it works 
 *                                      with FLAMINGO/SANDPIPER/PELICAN SROMs
 *
 *              dtr   1-Feb-1993        Added in the PC support so that this
 *                                      could be ported to a pc with a chance of
 *                                      working.  The changes consisted of
 *                                      changing the way the original file is
 *                                      loaded.  The file to be transferred is
 *                                      now loaded based on the compiler switch
 *                                      and how much space is to be allocated
 *                                      for the file.  On a PC the buffer size
 *                                      is 8k and on a vax/alpha platform it is
 *                                      32k.  The buffer is static at this
 *                                      implementation.
 *
 *              mjb     6-jun-1993      Added 19200 support for PC code by
 *                                      adding the optional "xload_fast" 
 *                                      parameter.
 *
 * How to generate the executable:
 *
 *              $ cc xload
 *              $ link xload, sys$Library:vaxcrtl/lib
 *              $ xload :== $ [directory_path_name]xload
 *
 */

#ifdef  VAXC				/* VAXC */
#include        stat.h			/* VAXC */
#include        stdio.h			/* VAXC */
#include        descrip.h		/* VAXC */
#include        ssdef.h			/* VAXC */
#include        iodef.h			/* VAXC */
#endif					/* VAXC */

#ifdef  MSDOS				/* MSDOS */
#include    <sys\types.h>		/* MSDOS */
#include    <sys\stat.h>		/* MSDOS */
#include    <stdio.h>			/* MSDOS */
#include    <stdlib.h>			/* MSDOS */
#include    <time.h>			/* MSDOS */
#include    "asynch_1.h"		/* MSDOS */
#define max_count 1024			/* MSDOS */
#define SS$_NORMAL 1			/* MSDOS */
void    sleep(int seconds);		/* MSDOS */
#endif					/* MSDOS */

#ifdef __linux__			/* __linux__ */
#include <stdio.h>			/* __linux__ */
#include <stdlib.h>			/* __linux__ */
#include <time.h>			/* __linux__ */
#include <string.h>			/* __linux__ */
#include <sys/types.h>			/* __linux__ */
#include <sys/stat.h>			/* __linux__ */
#include <fcntl.h>			/* __linux__ */
#include <termios.h>			/* __linux__ */
#include <unistd.h>			/* __linux__ */
#define SS$_NORMAL 1			/* __linux__ */
#endif					/* __linux__ */

#define CRLF		"\x0D\x0A"		/* "\015\012" */
#ifdef CONFIG_NAUTILUS
#define SROM_PROMPT 	"Don't Panic> "		/* "SROM> " */
#else
#define SROM_PROMPT 	"SROM> "		/* "SROM> " */
#endif



char    *helpmsg [] = {
" ",
" XLOAD exe_file load_address console_line [fast]",
" ",
" This program executes the host end of a VAX console binary load, XM command.",
" XLOAD takes as input an EXE image file and loads it in a target memory",
" at the specified address.  XLOAD assumes that the target ALPHA console is",
" connected to the specified serial port and uses that port to download the",
" image using the XM command protocol.", 
" ",
" pararameters:",
" ",
"       exe_file	This is the name of the image file to be loaded.",
"			No extentions are implied.",
" ",
"       load_address	This is the HEX physical address in the target",
"			memory, where the image will be loaded",
" ",
"       console_line	This is the name of the serial line to which",
"			the target Flamingo console is connected.",
" ",
"       fast    Add this parameter to run at 19200 baud (9600 is default)",
" ",
" Example:",
" ",
"       XLOAD VMB.EXE 200 TXA0:",
" ",
"       XLOAD BLAST.EXE 4000 COM1: FAST",
" ",
""
};

#ifdef VAXC						/* VAXC */
#define min(a,b) ((a) < (b) ? (a) : (b))		/* VAXC */
#define max(a,b) ((a) > (b) ? (a) : (b))		/* VAXC */
							/* VAXC */
/* if this is vax compile then allocate 32k */		/* VAXC */
							/* VAXC */
#define IMAGE_SIZE      (32 * 1024)			/* VAXC */
int     image_length;					/* VAXC */
int     data_left;					/* VAXC */
#endif							/* VAXC */

/* if this is pc compile then allocate 8k */
#ifdef MSDOS						/* MSDOS */
#define IMAGE_SIZE      (8 * 1024)			/* MSDOS */
_off_t  image_length;					/* MSDOS */
_off_t  data_left;					/* MSDOS */
#endif							/* MSDOS */

#ifdef __linux__					/* __linux__ */
#define min(a,b) ((a) < (b) ? (a) : (b))		/* __linux__ */
#define max(a,b) ((a) > (b) ? (a) : (b))		/* __linux__ */
							/* __linux__ */
#define IMAGE_SIZE      (32 * 1024)			/* __linux__ */
int     image_length;					/* __linux__ */
int     data_left;					/* __linux__ */
#endif							/* __linux__ */


#define IBUF_SIZE       1024

unsigned char   image [IMAGE_SIZE];

#define DEVICE_TIMEOUT  20              /* seconds                      */

/*                               
 * The following data structure keeps track of all channel activity,
 * including where data is to be picked up, put etc.
 */
typedef struct {
#ifdef VAXC
	struct dsc$descriptor_s desc;
#endif
	struct  {
		short   status;
		short   toffset;
		short   term;
		short   tsize;
	} iosb;
	int             channel;
	char            efn_write;
	char            efn_read;
	unsigned char   ibuf [IBUF_SIZE];
	unsigned int    ibuflen;
#ifdef __linux__
	int	portfd;
	char 	*devname;
#endif
} CHD;

CHD     ch;

FILE    *fp;                    /* file information pointer - one of the    */
				/* major changes. */


int tt_put( CHD *cp, char *buf, int len );
int tt_get( CHD *cp );
void tt_drain( CHD *cp );

void xload_flush( CHD *cp );


int buffer_size;

int status_result;              /* result fo stat function */

#ifdef  VAXC						/* VAXC */
struct  stat    file_info;				/* VAXC */
char    *xm_command = "XM\r%08X\r%08X\r";		/* VAXC */
char    *file_mode = "r";       /* read work on vms */	/* VAXC */
#endif							/* VAXC */


#ifdef  MSDOS						/* MSDOS */
/* only used in dos environment */			/* MSDOS */
#define OBUF_SIZE       1024            		/* MSDOS */
							/* MSDOS */
struct  _stat   file_info;				/* MSDOS */
							/* MSDOS */
/* enough space for both input and output buffers */	/* MSDOS */
char    qbuffer[IBUF_SIZE + OBUF_SIZE];     		/* MSDOS */
							/* MSDOS */
/* The messages have been made global because dos has certain characters in */
/* them that are dos specific. */

/* open dos file in binary mode */			/* MSDOS */
char    *file_mode = "rb";      			/* MSDOS */
char    *xm_command = "XM\r%08lX\r%08lX\r";		/* MSDOS */
int     xload_fast = 0;					/* MSDOS */
#endif							/* MSDOS */


#ifdef  __linux__					/* __linux__ */
struct  stat    file_info;				/* __linux__ */
char    *xm_command = "xm\r%08X\r%08X\r";		/* __linux__ */
char    *file_mode = "r";       			/* __linux__ */
int	xload_fast = 0;					/* __linux__ */
#endif							/* __linux__ */

			  
char    carriage_return [] = {0x0d};
char    checksum_error [] = {"?2A"};


/*----------*/
int main (int argc, char *argv[])
{
	int             i;      
	int             j;
	int             k;
	char            cmd_line [80];
	char            addr[12];
	char            length[12];
	unsigned char   cmd_checksum;
	long            cmd_address;
	unsigned char   data_checksum;
	char            *data_ptr;
	int             block;
	int status;

	printf("\n*** SROM XLOAD Utility V2.1(Stig) ***\n\n");

	/*
	 * Check for enough arguments
	 */
	if (argc < 4 || argc > 5) {
	    for (i=0; *helpmsg [i]; i++) {
		printf ("%s\n", helpmsg [i]);
	    }
	    exit (1);
	}

	if (argc == 5)          /* Any 5th parameter will set 19200 baud */
		xload_fast = 1;

	/*
	 * open the file and set up the default conditions - major change
	 */
	open_file (argv [1]);

	/*
	 * Set up the channel that talks to the console
	 */
	init_channel (&ch, argv [3]);
	tt_drain( &ch );

	/*                                           
	 * Synchronize with the console
	 */
	ch.ibuflen = 0;
	xload_sync(&ch);

	/*
	 * Formulate the command line
	 */
	cmd_address = strtol(argv[2],NULL,16);

	sprintf( cmd_line, xm_command, cmd_address, image_length );


	/*
	 * First send out the command line upto and including <CR>.
	 * Then "sleep" while the console transitions to Xio processing.
	 * Then send out the command checksum.
	 * Last verify that the line was accepted as a valid one.
	 * Note : WAIT_PROMPT will eat the echoed command line.
	 */

	printf("%%XLOAD-I-CMDLIN SROM> %s\n", cmd_line );
	tt_put (&ch, cmd_line, strlen (cmd_line));
	tt_drain( &ch );
	/*
	 * start sending the image a page at a time
	 */
	printf ("%%XLOAD-I-PAGSNT, X loaded page ");
	fflush( stdout );
	block = 1;
	data_left = image_length;   /* image_length set up by open_file */
	data_ptr = image;
	buffer_size = 0;            /* force the first read */

	while (data_left)
	{
	    if (!buffer_size)      /* if the number of characters in the    */
				   /* buffer is zero then read in a new one. */ 
	    {
		if (data_left < IMAGE_SIZE)
		{
		    buffer_size =(int) data_left;
		}
		else
		{
		    buffer_size = IMAGE_SIZE;
		}
		data_ptr = image;
		status=fread(image, 1, buffer_size, fp);
		data_ptr = image;
	    }

	    tt_put( &ch, data_ptr, 512 );
	    data_ptr += 512;
	    data_left -= 512;
	    buffer_size -= 512;
	    printf( "%4d/%4d\10\10\10\10\10\10\10\10\10",
			block, image_length / 512 );
	    fflush( stdout );
	    block++;
	}   
	printf ("\n");
	sleep(2);
	tt_drain( &ch );

	xload_sync( &ch );			/* Now wait for the prompt */

	if (strsrch ("?", ch.ibuf) != strlen (ch.ibuf)) {
	    printf ("%%XLOAD-E-TXDERR, error on sending the data\n");
	    do_exit (1);
	}
}                               
									  
/*
 * Make sure that the console is repsonding.  Send out a CR, and wait for
 * the response (in this case, the SROM prompt).
 */
int xload_sync (CHD *cp)

{
	int     c;
	char *prompt = CRLF SROM_PROMPT;

	/*
	 * Flush any data in the typeahead buffer
	 */
	xload_flush (cp);

	/*
	* send out a CR
	 */                        
	tt_put (cp, carriage_return, 1);
	wait_prompt (cp, prompt );
}


/*
 * wait for the prompt passed in , therefore eating anything
 * that comes along.  What does come along should be stored in the
 * input buffer, so that higher level software may look at any error messages.
 */

#define MAXPROMPT 20

int wait_prompt (CHD *cp, char *prompt_ptr)
{
    int i, L;
    char c;

    i = 0;  
    L = strlen( prompt_ptr );

    do {
	c = tt_get( cp );
	if ( c == 0 )			continue;	/* cut any NUL chars */

	/* otherwise receive another character */
	cp->ibuf[ cp->ibuflen ] = c;
	cp->ibuflen++;
	i++;

	/* can we match the prompt yet? */
	if ( i < L )			continue;	/* not enough input */

	printf("DEBUG: matching %s with %s...\n", 
		prompt_ptr, cp->ibuf + cp->ibuflen - L );

	if ( strncmp( prompt_ptr, cp->ibuf + cp->ibuflen - L, L ) == 0 )
	{
	    printf("DEBUG: found a match for the prompt!\n");
	    break;
	}

    } while( i < MAXPROMPT );
}


/*
 * Stuff a block of characters out to a channel.  This routine does not
 * eat any data echoed back; tt_put_get should be used for that.
 */
int tt_put (CHD *cp, char *buf, int len)

{
	int     status;
	int     function;
	int     count;
	
	int i;

	if (len == 0) 
	    return(0);
#ifdef  VAXC
	function = 
	    IO$_WRITELBLK       |
	    IO$M_NOFORMAT;

	status = sys$qiow (
	    cp->efn_write,              /* event flag           */
	    cp->channel,                /* channel              */
	    function,                   /* no translations etc  */
	    &cp->iosb,                  /* status block         */
	    0,                          /* ast address          */
	    0,                          /* ast parameter        */
	    buf,                        /* address of data      */
	    len,                        /* number of entries    */
	    0,                          /* p3 - timeout (0)     */
	    0,                          /* p4 - terminator      */
	    0,                          /* prompt string        */
	    0                           /* prompt string size   */
	);
	if (status != SS$_NORMAL)
	{
	    printf ("%%XLOAD-E-PUTERR, tt_put, bad main status\n");
	    do_exit (status);
	}

	if (cp->iosb.status != SS$_NORMAL) {
	    printf ("%%XLOAD-E-PUTERR, tt_put, bad iosb status\n");
	    do_exit (cp->iosb.status);
	}
#endif

#ifdef  MSDOS
	for (count = 0;  count < len;  count++)
	{
	    status = wrtch_a1(cp->channel,
		(char)(buf[count]));
	    /* if status is not ok we need to see if it because the queue */
	    /* is full.  If the que is full then we need to wait here */
	    /* until we can que it. */

	    if (status != A_OK)
	    {
		if (status == OUT_Q_FULL)
		    {
			while ((status = wrtch_a1(cp->channel,
			      (char)(buf[count])) != A_OK))
			    {
				;
			    }
		    }

	    }
	}

	if (status != A_OK)
	{
	    printf ("%%XLOAD-E-PUTERR, tt_put, bad main status\n");
	    do_exit (status);
	}
#endif


#ifdef __linux__
#if 0
	printf("DEBUG: tt_put writing %d bytes\n", len );
	status = write( cp->portfd, buf, len );
	tcdrain( cp->portfd );
	printf("DEBUG: tt_put wrote %d bytes\n", status );

	if ( status == -1 )
	{
	    printf( "%%XLOAD-E-PUTERR, tt_put, bad main status\n");
	    perror( "tt_put" );
	    do_exit (status);
	}

#else

	for ( i=0; i<len; i++ )
	{
            status = write( cp->portfd, buf+i, 1 );
            /* tcdrain( cp->portfd ); */

	    /* delay on carriage return to allow SROM to catch up */
	    if ( buf[i] == '\n' || buf[i] == '\r' )	sleep( 1 );

	    if ( status == -1 )
	    {
		printf( "%%XLOAD-E-PUTERR, tt_put, bad main status\n");
		perror( "tt_put" );
		do_exit (status);
	    }
        }
#endif

#endif


}

			     
/*
 * Read in a character from the line that is attached to the console.
 */
int tt_get (CHD *cp)

{
#ifdef  VAXC
	int     status;
	int     function;

	function =
	    IO$_READLBLK        |
	    IO$M_NOFORMAT       |
	    IO$M_NOFILTR        |
	    IO$M_TIMED          |
	    IO$M_NOECHO
	;
		
	status = sys$qiow (
	    cp->efn_read,               /* event flag           */
	    cp->channel,                /* channel              */
	    function,                   /* no translations etc  */
	    &cp->iosb,                  /* status block         */
	    0,                          /* ast address          */
	    0,                          /* ast parameter        */
	    cp->ibuf,                   /* address of data      */
	    1,                          /* number of entries    */
	    DEVICE_TIMEOUT,             /* p3 - timeout (0)     */
	    0,                          /* p4 - terminator      */
	    0,                          /* prompt string        */
	    0                           /* prompt string size   */
	);
#endif
#ifdef  MSDOS

	int     status;
	int     loop_control = 0;
	int     q_size;
	unsigned p_status;

	status = rdch_a1(cp->channel,
			cp->ibuf,
			&q_size,
			&p_status);
	while (status == IN_Q_EMPTY && loop_control < max_count)
	{
	    status = rdch_a1(cp->channel,
			cp->ibuf,
			&q_size,
			&p_status);
	    loop_control++;
	}

	if (status == A_OK)
	    {
		status = SS$_NORMAL;
	    }
#endif

#ifdef __linux__
	int	status;
	int	function;
	
	status = read( cp->portfd, cp->ibuf, 1 );
	if ( status == 1 )		status = SS$_NORMAL;
#endif

	/*
	 * Exit on a timeout
	 */

	if (status != SS$_NORMAL) {
	    printf ("%%XLOAD-E-GETERR, error in tt_get (status = %d)\n",
		status);
	    do_exit (status);
	}
#ifdef VAXC
	if (cp->iosb.status != SS$_NORMAL) {
	    printf ("%%XLOAD-E-GETERR, error in tt_get (iosb)\n");
	    do_exit (cp->iosb.status);
	}
#endif
	return cp->ibuf [0];
}


/*----------*/

/* This routine is to read any output from the SROM that it might be wanting
 * to say to us before we continue */

void tt_drain( CHD *cp )
{
    int c;

    printf("DEUBG: draining...\n" );

    do {
	c = read( cp->portfd, cp->ibuf, 1 );
	if ( c == 1 ) {
	    printf( "%c", *cp->ibuf );
	    fflush( stdout );
	}
    } while ( c == 1 );

    printf( "\n" );
}


/*----------*/
int init_channel (CHD *cp, char *name)

{
	int     i;
	int     status;

#ifdef  MSDOS
	char    temp_name[8] = {'\0','\0','\0','\0','\0','\0','\0','\0'};

	for (i = 0;  i < strlen(name);  i++)
	{
	    name[i] = toupper(name[i]);
	}

	i = strlen(name);       /* see if we need to strip out the : */     
	
	i--;                    /* backup to the : if present */
	
	if (name[i] == ':')
	{
	    name[i-1] = '\0';
	}

	if (!(strcmp (name, "COM1")))
	{
	    cp->channel = 1;
	}
	else if (!(strcmp (name, "COM2")))
	{
	    cp->channel = 2;
	}
	else
	{
	    status = 0;
	    do_exit(status);
	}
	/* open up the channel for useage */
	status = open_a1(cp->channel,       /* channel number */
			 IBUF_SIZE,         /* input buffer size */
			 OBUF_SIZE,         /* output buffer size */
			 0,                 /* default interrupt level */
			 0,                 /* default port address */
			 qbuffer);          /* queue buffer address */
	    if (status != A_OK)
	    {
		do_exit(status);
	    }
/* Now set up the working paramters for the comm line */
	    if (xload_fast) 
	    {
		(void)setop_a1(cp->channel, 1, 8 );     /* 19200 baud */
	    }
	    else 
	    {
		(void)setop_a1(cp->channel, 1, 7 );     /* 9600 baud */
	    }
	    (void)setop_a1(cp->channel, 2, 0 );     /* no parity */
	    (void)setop_a1(cp->channel, 3, 3 );     /* 8 bits */
	    (void)setop_a1(cp->channel, 4, 0 );     /* 1 stop bit */
	    (void)setop_a1(cp->channel, 5, 0);      /* disable remote flow control */
	    (void)setop_a1(cp->channel, 6, 0);      /* disable local flow control */
	    (void)setop_a1(cp->channel, 9, 0);      /* no clear to send */
#endif

#ifdef  VAXC
	/*
	 * Build string descriptors for the channel names and then
	 * assign the channels.     
	 */ 
	cp->desc.dsc$w_length   = strlen (name);
	cp->desc.dsc$b_dtype    = DSC$K_DTYPE_T;
	cp->desc.dsc$b_class    = DSC$K_CLASS_S;
	cp->desc.dsc$a_pointer  = name;
	status = sys$assign (&cp->desc, &cp->channel, 0, 0);
	if (status != SS$_NORMAL) do_exit (status);

	cp->efn_write = 0;
	cp->efn_read = 1;
#endif  

#ifdef __linux__
	struct termios tp;

	if( strncmp(name, "COM1", 4 ) == 0 )
	{
	    cp->channel = 1;
	    cp->devname = "/dev/ttyS0";
	}
	else if( strncmp(name, "COM2", 4 ) == 0 )
	{
	    cp->channel = 2;
	    cp->devname = "/dev/ttyS1";
	}
	else
	{
	    status = 0;
	    do_exit(status);
	}
	cp->portfd = open( cp->devname, O_RDWR | O_NOCTTY );
	if( cp->portfd == -1 ) 
	{
	    fprintf( stderr, "Something's wrong opening the port!\n" );
	    perror( "port open" );
	    exit( -1 );
	}
	printf("opened port %s...\n", cp->devname );

	tp.c_iflag = IGNBRK | IGNPAR;
	tp.c_oflag = NLDLY | CRDLY;
	tp.c_cflag = CS8 | CLOCAL | CREAD /* | CSTOPB */;
	if ( xload_fast ) 	tp.c_cflag |= B19200;
	else			tp.c_cflag |= B9600;
	tp.c_lflag = 0;

	tp.c_cc[VTIME] = 1;
	tp.c_cc[VMIN] = 0;		/* non-blocking read of 1 char */

	tcflush( cp->portfd, TCIFLUSH );
	tcsetattr( cp->portfd, TCSANOW, &tp );
#endif

}

/*----------*/
int open_file (char *name)

{
    if ((fp = fopen (name, file_mode)) == 0)
    {
	printf ("%%XLOAD-E-FILOPN, can't open %s\n", name);
	exit (1);
    }
#ifdef  VAXC
    if (stat(name, &file_info))
#endif
#ifdef  MSDOS
    if (_stat(name, &file_info))
#endif
#ifdef __linux__
    if ( stat( name, &file_info ) )
#endif
    {
	printf ("%%XLOAD-E-FILACC, can't access %s\n", name);
	exit (1);
    }

   image_length = (file_info.st_size + 511) & ~511;     /* block align */

}

/*--------*/
void xload_flush (CHD *cp)

{
	int     status;
	int     function;
	
#ifdef __linux__
	tcdrain( cp->portfd );
#endif

#ifdef VAXC
	function =
	     IO$_READLBLK       |
	     IO$M_NOFORMAT      |
	     IO$M_TIMED         |
	     IO$M_NOFILTR       |
	     IO$M_NOECHO
	;

	status = sys$qiow (
	    cp->efn_read,               /* event flag           */
	    cp->channel,                /* channel              */
	    function,
	    &cp->iosb,                  /* status block         */
	    0,                          /* ast address          */
	    0,                          /* ast parameter        */
	    cp->ibuf,                   /* address of data      */
	    IBUF_SIZE,                  /* number of entries    */
	    0,                          /* p3 - timeout (0)     */
	    0,                          /* p4 - terminator      */
	    0,                          /* prompt string        */
	    0                           /* prompt string size   */
	);
	if (status != SS$_NORMAL)
	{
	   printf ("%%XLOAD-E-FLUSH, flush error\n");
	    do_exit (status);
	}
#endif

#ifdef  MSDOS
	status = iflsh_a1(cp->channel);
	if (status != A_OK)
	{
	   printf ("%%XLOAD-E-FLUSH, flush error\n");
	    do_exit (status);
	}
#endif
}

/*
 * search for pattern P in string S.  Return the index of P in S, or the
 * length of S if P is not in S.
 */
int strsrch (char *p, char *s)

{
	int     i;
	int     len_p;
	int     len_s;

	len_p = strlen (p);
	len_s = strlen (s);

	if (len_p > len_s) return len_s;

	i = 0;
	do {
	    if (strncmp (p, s + i, len_p) == 0) {
		return i;
	    }
	    i++;
	} while (i <= (len_s - len_p));

	return len_s;
}

/*----------*/
int do_exit (unsigned int status)

{
	int     i;

	for (i=0; i<ch.ibuflen; i++) 
	{ 
	    printf ("%%XLOAD-E-STATUS, %3d: %02X %c\n",
		i,
		ch.ibuf [i],
		ch.ibuf [i] >= ' ' ? ch.ibuf [i] : '.'
	    );
	}
	exit (status);
}

#ifdef  MSDOS
/* pauses for the specified number of seconds */

void    sleep(int seconds)
{
    clock_t goal;

    goal = (seconds * CLOCKS_PER_SEC) + clock();
    while (goal > clock())
	;
}
#endif
