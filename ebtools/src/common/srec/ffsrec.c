/*
===========================
ffsrec.c  - subroutines from Beta console to understand S record files
===========================
*/
/* ffsrec.c
   L. Stewart
   Thu Nov  1 14:50:08 1984
 */

#include <mon.h>

char recType;
short int badRecOK;
short int badRec;
unsigned recLength;
long unsigned recAddress;
long unsigned recEntry;
unsigned recChecksum;
unsigned recCount;
unsigned recByteCount;
long unsigned recLowAddress;
long unsigned recHighAddress;
int srecport;

unsigned GetHex1()
{
  char c;
  if (badRec) return(0);
  c = uart_getchar (srecport);
  if (IsDigit (c)) return (c - '0');
  c = tolower(c);
  if (c >= 'a' && c <= 'f') return (c - 'a' + 10);
  if (badRecOK)
  {
    badRec = TRUE;
    return(0);
  }
  printf("!");
  return(0);
}

unsigned GetHex2(unsigned in)
{
  unsigned   value;
  value = GetHex1 () << 4;
  value += GetHex1 ();
  recChecksum += value;
  recLength -= 1;
  return ((in << 8) + value);
}

unsigned GetHex4(unsigned in)
{
  return (GetHex2 (GetHex2 (in)));
}

void GetData()
{
  if (recAddress < recLowAddress) recLowAddress = recAddress;
  while(recLength > 1) {
    WriteB((unsigned char *) recAddress, GetHex2(0));
    recAddress += 1;
    recByteCount += 1;
  }
  if (recAddress > recHighAddress) recHighAddress = recAddress - 1;
}

void CheckChecksum ()
{
  unsigned     temp;
  temp = (~recChecksum) & 0377;
  if (badRecOK) return;
  if (((GetHex2 (0) & 0377) != temp) || (recLength != 0))
    printf("bad checksum\n");
}

void GetRecord (int port)
{
  char c;
  srecport = port;
  recType = uart_getchar (srecport);
  badRecOK = (recType >= '7');
  recChecksum = 0;
  recLength = GetHex2 (0);
  switch (recType) {
    case '1':
      {		/* 2-byte address */
        recAddress = GetHex4 (0);
        GetData();
        break;
      }
    case '2':
      {		/* 3-byte address */
        recAddress = GetHex2 (GetHex4 (0));
        GetData();
        break;
      }
    case '3':
      {		/* 4-byte address */
        recAddress = GetHex4 (GetHex4 (0));
        GetData();
        break;
      }
    case '7':
      {		/* 2-byte transfer address */
        recEntry = GetHex4 (0);
        break;
      }
    case '8':
      {		/* 3-byte transfer address */
        recEntry = GetHex2 (GetHex4 (0));
        break;
      }
    case '9':
      {		/* 4-byte transfer address */
        recEntry = GetHex4 (GetHex4 (0));
        break;
      }
    default:
      {		/* ??? */
        recType = ' ';
        for (;;)
        {
          c = uart_getchar (srecport);
          if (c== '\r' || c == '\n') return;
        }
      }
  }
  CheckChecksum();
}

void Download ()
{
  badRec = FALSE;
  recType = ' ';
  recCount = 0;
  recByteCount = 0;
  recLowAddress = 0x7fffffff;
  recHighAddress = 0;
  while ((!badRec) && (recType < '7')) {
    while (uart_getchar(inputport) != 'S');
    GetRecord (inputport);
    recCount += 1;
/*    PutChar('.');
    if ((recCount & 63) == 0) PutChar('\n');
 */
    }
  printf("\nLoaded %d records, %d bytes", recCount, recByteCount);
  printf(" low %06x high %06x check ", recLowAddress, recHighAddress);
  ChecksumMem(recLowAddress, recHighAddress);
}

