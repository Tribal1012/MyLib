#ifndef __TRIBAL_DEFAULT_H__
#define __TRIBAL_DEFAULT_H__
#include <stdio.h>

/*
 * typedef
 */
typedef char               int8;
typedef short              int16;
typedef int                int32;
typedef long long          int64;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;

/*
 * Default defined
 */
#define True  1
#define False 0
#define Error -1

#define BUF_MAXSIZE 0x10000

/*
* for debugging(Copy dumpcode)
*/
#define printchar(c) printf("%c", isprint(c) ? c : '.');
#define fprintchar(fp, c) fprintf(fp, "%c", isprint(c) ? c : '.');
void hexdump(uint8 *p, uint32 len)
{
	uint32 i;
 
	for(i=0; i<len; i++)
	{
		if(i%16==0)
			printf("0x%08x  ",&buff[i]);
		printf("%02x ",buff[i]);
		if(i%16-15==0)
		{
			uint32 j;
			printf("  ");
			for(j=i-15;j<=i;j++)
            	printchar(buff[j]);
			printf("\n");
		}
	}
	if(i%16!=0)
	{
		uint32 j;
		uint32 spaces=(len-i+16-i%16)*3+2;

		for(j=0;j<spaces;j++)
			printf(" ");
		for(j=i-i%16;j<len;j++)
			printchar(buff[j]);
	}
	printf("\n");
}
void fhexdump(FILE* fp, uint8 *p, uint32 len)
{
	uint32 i;
 
	for(i=0; i<len; i++)
	{
		if(i%16==0)
			fprintf(fp, "0x%08x  ",&buff[i]);
		fprintf(fp, "%02x ",buff[i]);
		if(i%16-15==0)
		{
			uint32 j;
			fprintf(fp, "  ");
			for(j=i-15;j<=i;j++)
            	fprintchar(fp, buff[j]);
			fprintf(fp, "\n");
		}
	}
	if(i%16!=0)
	{
		uint32 j;
		uint32 spaces=(len-i+16-i%16)*3+2;

		for(j=0;j<spaces;j++)
			fprintf(fp, " ");
		for(j=i-i%16;j<len;j++)
			fprintchar(fp, buff[j]);
	}
	fprintf(fp, "\n");
}

#endif
