#define MAX_MEMORY_ID 7
#define u_int unsigned int

struct poem_data {
        u_int   STER;           /* Self-test error CSR */
        u_int   STDERA0;        /* STDERA 0 */
        u_int   STDERA1;        /* STDERA 1 */
        u_int   STDERA2;        /* STDERA 2 */
        u_int   STDERA3;        /* STDERA 3 */
        u_int   STDERB0;        /* STDERB 0 */
        u_int   STDERB1;        /* STDERB 1 */
        u_int   STDERB2;        /* STDERB 2 */
        u_int   STDERB3;        /* STDERB 3 */
        u_int   STDERC0;        /* STDERC 0 */
        u_int   STDERC1;        /* STDERC 1 */
        u_int   STDERC2;        /* STDERC 2 */
        u_int   STDERC3;        /* STDERC 3 */
        u_int   STDERD0;        /* STDERD 0 */
        u_int   STDERD1;        /* STDERD 1 */
        u_int   STDERD2;        /* STDERD 2 */
        u_int   STDERD3;        /* STDERD 3 */
        u_int   STDERE0;        /* STDERE 0 */
        u_int   STDERE1;        /* STDERE 1 */
        u_int   STDERE2;        /* STDERE 2 */
        u_int   STDERE3;        /* STDERE 3 */
        u_int   megs;           /* Size of memory in megabytes (hex) */
        u_int   slot;           /* Slot that memory card is in */
        u_int   memcnt;         /* Number of memories in system */
        u_int   valid;         	/* set to 1 if this table contains valid
				   information */
};

