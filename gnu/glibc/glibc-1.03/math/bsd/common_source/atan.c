/*
 * Copyright (c) 1985 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef lint
static char sccsid[] = "@(#)atan.c	5.5 (Berkeley) 10/9/90";
#endif /* not lint */

/* ATAN(X)
 * RETURNS ARC TANGENT OF X
 * DOUBLE PRECISION (IEEE DOUBLE 53 bits, VAX D FORMAT 56 bits)
 * CODED IN C BY K.C. NG, 4/16/85, REVISED ON 6/10/85.
 *
 * Required kernel function:
 *	atan2(y,x) 
 *
 * Method:                  
 *	atan(x) = atan2(x,1.0). 
 *
 * Special case:
 *	if x is NaN, return x itself.
 *
 * Accuracy:
 * 1)  If atan2() uses machine PI, then
 * 
 *	atan(x) returns (PI/pi) * (the exact arc tangent of x) nearly rounded;
 *	and PI is the exact pi rounded to machine precision (see atan2 for
 *      details):
 *
 *	in decimal:
 *		pi = 3.141592653589793 23846264338327 ..... 
 *    53 bits   PI = 3.141592653589793 115997963 ..... ,
 *    56 bits   PI = 3.141592653589793 227020265 ..... ,  
 *
 *	in hexadecimal:
 *		pi = 3.243F6A8885A308D313198A2E....
 *    53 bits   PI = 3.243F6A8885A30  =  2 * 1.921FB54442D18	error=.276ulps
 *    56 bits   PI = 3.243F6A8885A308 =  4 * .C90FDAA22168C2    error=.206ulps
 *	
 *	In a test run with more than 200,000 random arguments on a VAX, the 
 *	maximum observed error in ulps (units in the last place) was
 *	0.86 ulps.      (comparing against (PI/pi)*(exact atan(x))).
 *
 * 2)  If atan2() uses true pi, then
 *
 *	atan(x) returns the exact atan(x) with error below about 2 ulps.
 *
 *	In a test run with more than 1,024,000 random arguments on a VAX, the 
 *	maximum observed error in ulps (units in the last place) was
 *	0.85 ulps.
 */

double atan(x)
double x;
{
	double atan2(),one=1.0;
	return(atan2(x,one));
}
