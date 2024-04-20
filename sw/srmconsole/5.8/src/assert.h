/*
 *  This is the standard C assert macro modified for use
 *  in the SRM console build environment.
 *
 *  The message is output via err_printf().
 *
 *
 */
#ifndef assert_h
#define assert_h

#if DEBUG
   int err_printf(const char*, ...);
   #define assert(expr) if (!(expr)) err_printf("\nCondition \"%s\" is " \
                 "false in %s, line %d.\n", #expr, __FILE__, __LINE__)
#else
   #define assert(ignore) ((void) 0)
#endif

#endif
