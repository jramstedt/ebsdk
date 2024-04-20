/*	LIMITS - V3.0	*/

# define CHAR_BIT	8
# define CHAR_MAX	127
# define CHAR_MIN	-128
# define INT_MAX	2147483647
# define INT_MIN	-2147483648
# define LONG_MAX	2147483647
# define LONG_MIN	-2147483648
# define SCHAR_MAX	127	
# define SCHAR_MIN	-128
# define SHRT_MAX	32767
# define SHRT_MIN	-32768

/* Section 2.2.4.2 of the Rationale states "The limits for the maxima
 * and minima of unsigned types are specified as unsigned constants..."
 * The alert reader will notice there are no minima for the unsigned
 * types.   But we will follow the Rationale's advice anyway.
 */
# define UCHAR_MAX	255u
# define UINT_MAX	4294967295u
# define ULONG_MAX	4294967295u
# define USHRT_MAX	65535u
