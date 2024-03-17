/*	ranlib.h	4.1	83/05/03	*/

/*
 * HISTORY:
 * 13-Sep-92  Alessandro Forin (af) at Carnegie-Mellon University
 *	Removed crap, ameliorated description.
 *
 * $Log: ranlib.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:30  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  89/07/18  10:02:40  mbj
 * 	Added ran_comsize_or_dt field for the Multimax.
 * 	[89/07/18  10:01:54  mbj]
 * 
 */

/*
 * Structure of the __.SYMDEF table of contents for an archive.
 * __.SYMDEF begins with a native word [keeps things aligned]
 * giving the number of ranlib structures which immediately follow,
 * and then continues with a string table consisting of an int
 * [keeps things compatible] giving the number of bytes of strings
 * which follow and then the strings themselves.
 * The ran_strx fields index the string table whose first byte is numbered 0.
 */
typedef long ranlib_nelm_t;

struct	ranlib {
	union {
		off_t	ran_strx;	/* string table index of.. */
		char	*ran_name;	/* ..symbol defined by.. */
	} ran_un;
	off_t	ran_off;		/* ..library member at this offset */
};

typedef int ranlib_ssiz_t;

#define	RANLIB_MEMBERNAME	"__.SYMDEF"
