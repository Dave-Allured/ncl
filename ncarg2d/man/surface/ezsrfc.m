.TH EZSRFC 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
EZSRFC - Draws a perspective picture of a function of two
variables with hidden lines removed. The function is
approximated by a two-dimensional array of heights. Use EZSRFC
only if the entire array is to be drawn, the data points are
equally spaced in the X-Y plane, there are no stereo pairs, and
scaling is chosen internally.
.SH SYNOPSIS
CALL EZSRFC (Z,M,N,ANGH,ANGV,WORK)
.SH C-BINDING SYNOPSIS
#include <ncarg/ncargC.h>
.sp
void c_ezsrfc (float *z, int m, int n, float angh, float angv, 
.br
float *work)
.SH DESCRIPTION 
.IP Z 12
The M by N array to be drawn.
.IP M 12
The first dimension of Z.
.IP N 12
The second dimension of Z.
.IP ANGH 12
Angle in degrees in the X-Y plane to the line of sight
(counterclockwise from the plus-X axis).
.IP ANGV 12
Angle in degrees from the X-Y plane to the line of
sight (positive angles are above the middle Z, negative
below).
.IP WORK 12
A scratch storage dimensioned at least 2*M*N+M+N.
.SH C-BINDING DESCRIPTION
The C-binding argument descriptions are the same as the FORTRAN 
argument descriptions, with the following exceptions:
.IP z 12
The n by m array to be drawn.
.IP m 12
The second dimension of z.
.IP n 12
The first dimension of z.
.SH EXAMPLES
Use the ncargex command to see the following relevant
examples: 
fsrezsrf,
tsrfac.
.SH ACCESS
To use EZSRFC or c_ezsrfc, load the NCAR Graphics libraries ncarg, ncarg_gks,
and ncarg_c, preferably in that order.  
.SH SEE ALSO
Online:
surface,
surface_params,
pwrzs,
setr,
srface.
ncarg_cbind.
.sp
Hardcopy:
NCAR Graphics Fundamentals, UNIX Version
.SH COPYRIGHT
Copyright (C) 1987-2003
.br
University Corporation for Atmospheric Research
.br

This documentation is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This software is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
USA.
