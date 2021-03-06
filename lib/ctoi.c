#ifdef cray
/****************************************************************************
*
                     Data Retrieval and Storage System

**********************************************************************

			DISCLAIMER

   This software was prepared as an account of work sponsored by an
   agency of the United States Government. Neither the United
   States Government nor the University of California nor any of
   their employees, makes any warranty, express or implied, or
   assumes any liability or responsibility for the accuracy,
   completeness, or usefulness of any information, apparatus,
   product, or process disclosed, or represents that its use would
   not infringe privately owned rights. Reference herein to any
   specific commercial products, process, or service by trade name,
   trademark, manufacturer, or otherwise, does not necessarily
   constitute or imply its endorsement, recommendation, or favoring
   by the United States Government or the University of California.
   The views and opinions of authors expressed herein do not
   necessarily state or reflect those of the United States
   Government or the University of California, and shall not be
   used for advertising or product endorsement purposes.
   
**********************************************************************


* Note: C8TOF4 and F4TOC8 were adapted from HDF routines developed at
* NCSA at U. of Illinois (see disclaimer below.)
* 
* Routines for translation between Cray and IEEE floating point
* representations, and between 64-bit and 32-bit, 2's-complement
* integer representations.
* 
* Author: Bob Drach
* 
* Date: 12/21/89
* Modified: 01/28/91 C8TOF4 (BD) - map small, valid Cray floats to zero.
* 
* ctoi.c,v 2.5 1995/10/16 18:47:12 drach Exp
* ctoi.c,v
 * Revision 2.5  1995/10/16  18:47:12  drach
 * - Added byteswap routine for DEC Alpha
 *
 * Revision 2.4  1995/03/30  00:50:28  drach
 * Added disclaimer
 *
 * Revision 2.3  1994/11/23  19:15:30  drach
 * Fixed unsigned char comparison to get rid of compiler warning.
 *
 * Revision 2.2  1993/07/14  22:30:00  drach
 * Removed text after #endif, etc.
 *
 * Revision 2.1  1993/07/13  01:12:41  drach
 * Merged Sun, Unicos, SGI, and HP versions.
 *
 * Revision 2.0  1992/03/10  00:53:45  drach
 * Entered into RCS
 *
*
* Note: C8TOF4 and F4TOC8 were adapted from HDF routines developed at
* NCSA at U. of Illinois (see disclaimer below.)
* 
* *** THESE ROUTINES ONLY WORK ON THE CRAY! ***
* 
* Externals: memcpy (in library /lib/libc at NMFECC)
****************************************************************************
* 
* include "drscdf.h"
* int
* C8TOF4(in,out,len,halfwordflag)
*   long in[],out[],*len,*halfwordflag
* 
*   Translate array 'in' of 64-bit Cray floating point numbers, to
* array 'out' of IEEE 32-bit floating point numbers. 'len' is the length
* of 'in', in words. The results will be packed, two per word, in array
* 'out'. If halfwordflag==IDRS_RIGHTHALFWORD, then the first output
* value will be packed into the rightmost halfword of out[0], otherwise
* it will be placed in the leftmost halfword of out[0].
*   C8TOF4 returns IDRS_SUCCESS if all translations succeeded, or
* IDRS_BADCRAYFP if an input number was out of range. 
* The translation strategy is as follows: let MINCEXP (= 0x2000)
* and MAXCEXP (= 0x5fff) be the min and max valid Cray exponents,
* respectively, and let MINEXP (= 0x3f83) and MAXEXP (= 0x407f)
* be the min and max translatable exponents) Then
* 
*         exponent               translation           function returns
*         --------               -----------           ----------------
* exponent < MINCEXP             0x7fffffff (NaN)      IDRS_BADCRAYFP
* MINCEXP <= exponent < MINEXP   0                     IDRS_SUCCESS
* MINEXP <= exponent <= MAXEXP   IEEE equivalent       IDRS_SUCCESS
* MAXEXP < exponent <= MAXCEXP   0x7f800000 (Infinity) IDRS_BADCRAYFP
* MAXCEXP < exponent             0x7fffffff (NaN)      IDRS_BADCRAYFP
* 
* Translation continues even if an error occurs.
* 
* 
* int
* F4TOC8(in,out,len,halfwordflag)
*   long in[],out[],*len,*halfwordflag
* 
*   Translate array 'in' of 32-bit IEEE floating-point numbers, packed
* two per word, into array 'out' of 64-bit Cray floating-point numbers.
* 'len' is the number of items to translate.
* If halfwordflag==IDRS_RIGHTHALFWORD, then translation will begin
* with the right halfword of 'in', otherwise it will begin
* with the left halfword.
*   F4TOC8 returns IDRS_SUCCESS if all translations succeeded, or
* IDRS_BADIEEEFP if an input number has zero exponent and nonzero fraction.
* In this case, the output number is set to 0x8000000000000000, 
* and translation continues.
* 
* 
* int
* I8TOI4(in,out,len,halfwordflag)
*   long in[],out[],*len,*halfwordflag
* 
*   Translate array 'in' of 64-bit Cray (2's-complement) integers, to
* array 'out' of IEEE 32-bit (2's-complement) integers. 'len' is the length
* of 'in', in words. The results will be packed, two per word, in array
* 'out'. If halfwordflag==IDRS_RIGHTHALFWORD, then the first output
* value will be packed into the rightmost halfword of out[0], otherwise
* it will be placed in the leftmost halfword of out[0].
*   I8TOI4 returns IDRS_SUCCESS if all translations succeeded, or
* IDRS_BADCRAYINT if an input number was out of range. If this occurs,
* the output value will be set to 0x7FFFFFFF (if the input number was
* positive) or 0x80000000 (if the input number was negative), 
* and translation will continue.
* 
* 
* int I4TOI8(in,out,len,halfwordflag)
*   long in[],out[],*len,*halfwordflag
* 
*   Translate array 'in' of 32-bit IEEE (2's-complement) integers, packed
* two per word, into array 'out' of 64-bit Cray (2's-complement) integers.
* 'len' is the number of items to translate. 
* If halfwordflag==IDRS_RIGHTHALFWORD, translation begins with the right
* halfword of in; otherwise, it begins with the left halfword.
*   I4TOI8 always returns IDRS_SUCCESS.
*
****************************************************************************
*
* Portions developed at the National Center for Supercomputing Applications at
* the University of Illinois at Urbana-Champaign.
* 
* 
* THE UNIVERSITY OF ILLINOIS GIVES NO WARRANTY, EXPRESSED OR IMPLIED, FOR THE
* SOFTWARE AND/OR DOCUMENTATION PROVIDED, INCLUDING, WITHOUT LIMITATION,
* WARRANTY OF MERCHANTABILITY AND WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE
* 
*****************************************************************************/
#include <memory.h>
#include "drscdf.h"

#define MINEXP    0x3f83000000000000  /* min valid (translatable) Cray masked exponent */
#define MAXEXP    0x407f000000000000  /* max valid (translatable) Cray masked exponent */
#define MINCEXP   0x2000000000000000  /* min valid Cray masked exponent */
#define MAXCEXP   0x5fff000000000000  /* max valid Cray masked exponent */

#define C_FMASK   0x00007fffff000000  /* Cray fraction mask (1st 23 bits) */
				      /* Note that implicit bit (47) is */
				      /* always 1 for normalized floats. */
#define C_EMASK   0x7fff000000000000  /* Cray exponent mask */
#define C_SMASK   0x8000000000000000  /* Cray sign mask */
#define C_IMPLICIT 0x0000800000000000 /* Cray implicit bit */
#define NAN       0x7fffffff00000000  /* IEEE NaN mask */
#define INFINITY  0x7f80000000000000  /* IEEE Infinity mask */

#define I_FMASK   0x007fffff          /* IEEE fraction mask */
#define I_EMASK   0x7f800000          /* IEEE exponent mask */
#define I_SMASK   0x80000000          /* IEEE sign mask     */

#define IEEE_BIAS 0177 /* IEEE exponent bias */
#define CRAY_BIAS 040000 /* CRAY exponent bias */

#define CRAY_INTMASK 0x000000007fffffff /* CRAY integer mask */
#define CRAY_INTMASK2 0x7fffffff00000000 /* CRAY_INTMASK << 32 */

static long C2I_diff = (IEEE_BIAS - CRAY_BIAS - 1) << 48;
static long I2C_diff = (CRAY_BIAS - IEEE_BIAS + 1) << 23;

	/*  convert from Cray floating point format to IEEE 32-bit format */

int
C8TOF4(in,out,len,halfword)
	long *in, *out,*len,*halfword;
{
	char *p;
	int i,err;
	long tmp;

	err = IDRS_SUCCESS;
	p = (char *) out;
	if (*halfword == IDRS_RIGHTHALFWORD) p += 4;
	
	for(i=0; i<*len; i++, p+=4, in++)
	{
		                /* Translate +/- 0 to same */
		if((*in & ~C_SMASK) == 0) tmp = *in;
				/* Translate valid IEEE floats */
		else if ((tmp = (C_EMASK & *in)) <= MAXEXP && tmp >= MINEXP){
			tmp = (( tmp + C2I_diff ) << 7)
				| ( (*in & C_FMASK) << 8 )
					| ( (*in & C_SMASK));
		}
				/* Invalid Cray floats map to IEEE NaN*/
		else if (tmp < MINCEXP || tmp > MAXCEXP){
			err = IDRS_BADCRAYFP;
			tmp = NAN;
		}
				/* Cray underflow is translated to 0 */
		else if (tmp < MINEXP){
			tmp = 0;
		}
				/* Large Cray floats map to IEEE Infinity */
		else{
			err = IDRS_BADCRAYFP;
			tmp = INFINITY;
		}
		/* Copy leftmost 4 bytes of tmp to *p */
		memcpy(p, (char *) &tmp, 4);
	}
	return(err);
}


 
/* Conversion from IEEE 32-bit floating point format to Cray format */
 
int
F4TOC8(in,out,n,halfword)
	long *in,*out,*n,*halfword;
{
	long tmp;
	int i,err;
	char *p;
	
	err = IDRS_SUCCESS;
	p=(char *) in;
	if(*halfword == IDRS_RIGHTHALFWORD) p+=4;
	
	for(i=0; i < *n; i++, p+=4, out++)
	{
		tmp = 0;
		memcpy(((char *) &tmp) + 4, p, 4); /* Move in[i] to right 4 bytes */
		if((tmp & ~I_SMASK) == 0)
			*out = tmp << 32;
		else if ((*out = tmp & I_EMASK) == 0)
		{ /* If tmp is nonzero, but exponent is zero, signal error */
			err = IDRS_BADIEEEFP;
			*out = C_SMASK;
		}
		else
		{
			*out += I2C_diff;
			*out = (*out << 25)
				|( (tmp & I_FMASK) << 24)
					| ( (tmp & I_SMASK) << 32)
						| C_IMPLICIT;
		}
	}
	return(err);
}	

int
I8TOI4(in,out,len,halfword)
	long *in, *out,*len,*halfword;
{
	char *p;
	int i,err;
	long tmp,sign;

	err = IDRS_SUCCESS;
	p = (char *) out;
	if (*halfword == IDRS_RIGHTHALFWORD) p += 4;
	
	for(i=0; i<*len; i++, p+=4, in++)
	{ /* If the sign bit is 0 and any of the leftmost 33 bits */
	  /* is nonzero, the integer is too large to translate: */
	  /* signal error and return largest 32-bit integer. */
	  /* */
	  /* If the sign bit is 1 and any of the leftmost 33 bits */
	  /* are not 1, then integer is too small to translate: */
	  /* signal error and return smallest 32-bit integer. */
	  /* */
		if(((sign = *in & C_SMASK) == 0)
		   && ((*in & ~CRAY_INTMASK) != 0))
		{
			err=IDRS_BADCRAYINT;
			tmp=CRAY_INTMASK2;
		}
		else if((sign == C_SMASK) && (~*in & ~CRAY_INTMASK) != 0)
		{
			err=IDRS_BADCRAYINT;
			tmp=C_SMASK;
		}
		else
			tmp= (*in & CRAY_INTMASK)<<32 | sign;

		memcpy(p,(char *) &tmp, 4);
	}
	return(err);
}


int
I4TOI8(in,out,len,halfword)
	long *in, *out, *len, *halfword;
{
	int i,err;
	char *p;
	
	err = IDRS_SUCCESS;
	p=(char *) in;
	if(*halfword == IDRS_RIGHTHALFWORD) p+=4;

	for(i=0; i < *len; i++, p+=4, out++)
	{
		*out = 0;
		
		memcpy(((char *) out) + 4, p, 4); /* Move to right 4 bytes of out */
		/* If input is negative, set leftmost 33 bits of *out to 1 */
		if ((*out & I_SMASK) == I_SMASK)
			*out = *out | ~CRAY_INTMASK;
	}
	return(err);
}
#else				/* end DRS_CRAY_MACHINE */
/*
**********************************************************************

     The subroutines in this section are adapted from code written originally
     by Chuck D'Ambra (NCAR). His disclaimer appears below.

     ctoi.c,v 2.5 1995/10/16 18:47:12 drach Exp
     ctoi.c,v
 * Revision 2.5  1995/10/16  18:47:12  drach
 * - Added byteswap routine for DEC Alpha
 *
 * Revision 2.4  1995/03/30  00:50:28  drach
 * Added disclaimer
 *
 * Revision 2.3  1994/11/23  19:15:30  drach
 * Fixed unsigned char comparison to get rid of compiler warning.
 *
 * Revision 2.2  1993/07/14  22:30:00  drach
 * Removed text after #endif, etc.
 *
 * Revision 2.1  1993/07/13  01:12:41  drach
 * Merged Sun, Unicos, SGI, and HP versions.
 *
 * Revision 2.0  1992/03/07  00:12:24  drach
 * Entered into RCS
 *

*/

/*  c8tof4() convert Cray 64-bit floating point number to IEEE 32 bit floating point
    i8toi4() convert Cray 64-bit (2's-complement) integer to 32-bit 2's complement integer
    f4toc8() and i4toi8() currently no-op
    it is assumed that sizeof(char) == 8 bits on all machines.
 *  Written April 1987 by Chuck D'Ambra (NCAR).
 *  Last modified 15 August 1988 by CD.
 *  Author makes no warranty whatsoever as to the usefulness of these routines
    and accepts no responsibility for any errors, loss of productivity or other
    damages that may result (use at your own risk).
 *  Modified 15 Nov 1989 by Bob Drach
 */

#define CBIAS		040000	/*  Cray f.p. exponent bias  */
#define MBIAS		126	/*  680x0 f.p. exponent bias  */
#define CSIGNMASK	0200	/*  Mask to get 1st of 8 bits  */
#define LCDIF		sizeof(long) - sizeof(char)

int c8tof4_();
int i8toi4_();
int f4toc8_();			/* No-op */
int i4toi8_();			/* No-op */

/*-------------------------------------------------------------------*/
/* CFT (Cray) interface */

int C8TOF4(in, out, n, offset)	/*  Handles difference between f77 and CFT  */
unsigned char in[], out[];
int *n, *offset;
{
  return(c8tof4_(in, out, n, offset));
}


int I8TOI4(in, out, n, offset)	/*  Handles difference between f77 and CFT  */
unsigned char in[], out[];
int *n, *offset;
{
  return(i8toi4_(in, out, n, offset));
}

int F4TOC8(in, out, n, offset)	/*  Handles difference between f77 and CFT  */
unsigned char in[], out[];
int *n, *offset;
{
  return(f4toc8_(in, out, n, offset));
}

int I4TOI8(in, out, n, offset)	/*  Handles difference between f77 and CFT  */
unsigned char in[], out[];
int *n, *offset;
{
  return(i4toi8_(in, out, n, offset));
}

/*-------------------------------------------------------------------*/

int c8tof4_(in, out, n, offset)
unsigned char in[], out[];
int *n, *offset;
/* Translate Cray 64-bit reals to IEEE 32-bit reals
 *
 * in: array of Cray 64-bit reals
 * out: float array of result IEEE 32-bit reals
 * n: length of in, out 
 *
 * return: 0 if no errors, 1 if an error occurred
 *
 * Note: if an invalid input real is found, the corresponding result
 * is set to NaN; the routine continues to translate subsequent values.
 */
{
  register int i, j, joff;
  register unsigned char *p, sign, *expp;
  unsigned char *manp[3];
  long exp;
  int err;
  

/*  Cray 64 bit float representation:
 *	bits 0-47:	mantissa; bit 47 is 1 for normalized floating point
 *	bits 48-62:	exponent
 *	bit 63:		sign
 */

  err=0;
  
  for (i = 0,p=in,joff=0; i < *n; i++,p+=8,joff+=4)
  {
    sign = *(p) & CSIGNMASK;	/*  sign bit  */
    for (j = 0; j < 3; j++)
      manp[j] = p+j+2;		/*  point at mantissa  */

    exp = (*(p+1)) | ((*p & 0x7f) << 8);
    if (exp != 0)
      exp = exp - CBIAS + MBIAS;

/*  If input is outside range representable in 32 bits, set to NaN */

    if (exp < 0 || exp > 253)	/* Out of range */
    {
	    out[joff]='\177';
	    out[joff+1]=out[joff+2]=out[joff+3]='\377';
	    err=1;
	    continue;
    }
    
    
/*  Pack it into 32 bit 680x0 float representation:
 *	bits 0-22:	mantissa, leading 1 bit implicit for normalized reals
 *	bits 23-30:	exponent
 *	bit 31:		sign
 */
    if(exp==0)			/* Test for zero */
    {
	    if(*(p+2)=='\0' && *(p+3)=='\0' && *(p+4)=='\0' &&
	       *(p+5)=='\0' && *(p+6)=='\0' && *(p+7)=='\0')
	    {
		    out[joff]= sign | '\0';
		    out[joff+1]=out[joff+2]=out[joff+3]='\0';
		    continue;
		    
	    }
	    else		/* Set to NaN if nonzero mantissa */
	    {
		    out[joff]='\177';
		    out[joff+1]=out[joff+2]=out[joff+3]='\377';
		    err=1;
		    continue;
	    }
    }
	    
    expp = ((unsigned char *)(&exp)) + LCDIF; /* Valid on Sun or Cray */
    out[joff] = sign | (*expp >> 1);
    out[joff+1] = (*manp[0] & (~CSIGNMASK)) | ((*expp & 1) << 7);
    out[joff+2] = *manp[1];
    out[joff+3] = *manp[2];
  }
  return(err);
}


int i8toi4_(in, out, n, offset)
unsigned char in[], out[];
int *n, *offset;
/* Translate array 'in' of 64-bit Cray integers to array 'out' of
   32-bit (2's-complement) integers.

   in: array of 64-bit (2's-complement) Cray integers
   out: result array of 32-bit 2's-complement integers
   n: length of 'in','out'

   return 0 if no error, 1 if an error occurred

   Note: all input values will be translated, even if an error occurs.

 */
 
{
  register int i, j, joff;
  register unsigned char *pin, sign;
  unsigned char *p[4];
  int err;
  
  err=0;
  
  for (i = 0, pin=in,joff=0; i < *n; i++,pin+=8,joff+=4)
  {

    sign = *(pin) & CSIGNMASK;	/*  sign bit  */
    for (j = 0; j < 4; j++)
      p[j] = pin+j+4;		/*  point to last 4 bytes  */


  if (sign == 0)	/*  int >= 0; 1st 33 bits must be 0.  */
  {
    if ((*(pin+4) & CSIGNMASK) != 0  ||  *pin != 0  ||  *(pin+1) != 0
        || *(pin+2) != 0  ||  *(pin+3) != 0)
    {
	    out[joff]= '\177';
	    out[joff+1]=out[joff+2]=out[joff+3]='\377';
	    err=1;
	    continue;
    }
  }
  else			/*  int < 0; 1st 33 bits must be 1.  */
  {
    if ((*(pin+4) & CSIGNMASK) != 0200 ||  *pin != 0377  ||  *(pin+1) != 0377
        || *(pin+2) != 0377  ||  *(pin+3) != 0377)
    {
	    out[joff]='\200';
	    out[joff+1]=out[joff+2]=out[joff+3]='\0';
	    err=1;
	    continue;
    }
  }


/*  Pack it into 4 byte 680x0 integer representation  */

    out[joff] = sign | (*p[0] & (~CSIGNMASK));
    out[joff+1] = *p[1];
    out[joff+2] = *p[2];
    out[joff+3] = *p[3];
  }
  return(err);
  
}

int f4toc8_(in, out, n, offset)
unsigned char in[], out[];
int *n, *offset;
{
	return(1);
}

int i4toi8_(in, out, n, offset)
unsigned char in[], out[];
int *n, *offset;
{
	return(1);
}
#if defined(__alpha)
					     /* Byte swap for DEC Alpha int, float */
void byteswap4_(void *buf, int *n){
	int i;
	unsigned int temp;
	unsigned char *ctemp, *cbuf;

	cbuf = (unsigned char *)buf;
	ctemp = (unsigned char *)&temp;
	for(i=0; i<*n; i++){
		temp = *(int *)cbuf;
		*cbuf = *(ctemp+3);
		*(cbuf+1) = *(ctemp+2);
		*(cbuf+2) = *(ctemp+1);
		*(cbuf+3) = *ctemp;
		cbuf += 4;
	}
}
#endif /* alpha */
#if defined(__linux)
					     /* Byte swap for linux int, float */
#if defined(__linux_pgi) || defined(__linux_gfortran)
void byteswap4_(void *buf, int *n){
#else
void byteswap4(void *buf, int *n){
#endif
	int i;
	unsigned int temp;
	unsigned char *ctemp, *cbuf;

	cbuf = (unsigned char *)buf;
	ctemp = (unsigned char *)&temp;
	for(i=0; i<*n; i++){
		temp = *(int *)cbuf;
		*cbuf = *(ctemp+3);
		*(cbuf+1) = *(ctemp+2);
		*(cbuf+2) = *(ctemp+1);
		*(cbuf+3) = *ctemp;
		cbuf += 4;
	}
}
#endif /* linux */
#endif			/* DRS_SUN_MACHINE */
