.TH GETSI 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.SH NAME
.na
.nh
GETSI - returns the exponents which define the plotting range
in the plotter address unit coordinate system.
.SH STATUS
Plotter Address Units (PAUs) are no longer used in NCAR Graphics;
thus, GETSI is considered an obsolete routine.
.sp
The current recognized coordinate systems are GKS world coordinates,
GKS normalized device coordinates, NCAR Graphics fractional
coordinates, and NCAR Graphics user coordinates.  See the NCAR Graphics
document "The Use of X/Y Coordinates in NCAR Graphics" for a description
of these coordinate systems.
.sp
GETSI continues to be provided for compatibility of early NCAR Graphics
codes.
.sp
The following definition of the PAU Coordinate System is included
for the purpose of interpreting and converting PAU codes:
.sp
The plotter coordinates of a point are integers IPX and IPY, where
IPX is between 1 and 2**MX and IPY is between 1 and 2**MY.  MX and
MY are internal parameters of SPPS; each has the default value 10.
Values of MX and MY can be set by routine SETI and retrieved by
routine GETSI.
.SH SYNOPSIS
CALL GETSI (IX,IY)
.SH DESCRIPTION 
.IP IX 12
(an output variable of type INTEGER) is the X axis exponent for plotter
coordinates defining a range of 1 to 2**IX - 1.  IX has a default value
of 10.
.IP IY 12
(an output variable of type INTEGER) is the Y axis exponent for plotter
coordinates defining a range of 1 to 2**IY - 1.  IY has a default value
of 10.
.SH ACCESS
To use GETSI, load the NCAR Graphics libraries ncarg, ncarg_gks,
and ncarg_c, preferably in that order.
.SH SEE ALSO
Online:
spps, seti
.sp
Hardcopy:  
NCAR Graphics Fundamentals, UNIX Version;
User's Guide for NCAR GKS-0A Graphics
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
