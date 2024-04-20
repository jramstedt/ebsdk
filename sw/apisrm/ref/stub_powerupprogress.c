void PowerUpProgress( unsigned char value, char *string,
	int a, int b, int c, int d )
{
if( value == 0xff )
    err_printf( string, a, b, c, d );
else
    qprintf( string, a, b, c, d );
}
