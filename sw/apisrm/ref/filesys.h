#ifndef filesys_h
#define filesys_h

struct FILE *fopen (char *pathname, char *mode);
int fread( char *buffer, int size, int number, struct FILE *fp );
int fwrite ( char *buffer, int size, int number, struct FILE *fp );
int fclose (struct FILE *fp);
int fseek (struct FILE *fp, INT offset, int direction);
                        
#endif	/* filesys_h wrapper */
