#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <stdarg.h>

static char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789"
"+/";

/*
** ENCODE RAW into BASE64
*/

/* Encode source from raw data into Base64 encoded string */
int encode(unsigned s_len, char *src, unsigned d_len, char *dst)
{
unsigned triad;

for (triad = 0; triad < s_len; triad += 3)
{
unsigned long int sr;
unsigned byte;

for (byte = 0; (byte<3)&&(triad+byte<s_len); ++byte)
{
sr <<= 8;
sr |= (*(src+triad+byte) & 0xff);
}

sr <<= (6-((8*byte)%6))%6; /*shift left to next 6bit alignment*/

if (d_len < 4) return 1; /* error - dest too short */

*(dst+0) = *(dst+1) = *(dst+2) = *(dst+3) = '=';
switch(byte)
{
case 3:
*(dst+3) = base64[sr&0x3f];
sr >>= 6;
case 2:
*(dst+2) = base64[sr&0x3f];
sr >>= 6;
case 1:
*(dst+1) = base64[sr&0x3f];
sr >>= 6;
*(dst+0) = base64[sr&0x3f];
}
dst += 4; d_len -= 4;
}

return 0;
}


