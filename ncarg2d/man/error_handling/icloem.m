.TH ICLOEM 3NCARG "March 1994" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
ICLOEM - Called to get the length of the non-blank portion of an error message.
.SH SYNOPSIS
LNGTH = ICLOEM(MESSG)
.SH C-BINDING SYNOPSIS
#include <ncarg/ncargC.h>
.sp
int c_icloem(char *messg)  ???
.SH DESCRIPTION 
The value of the expression "ICLOEM(MESSG)" is the actual length of the
non-blank portion of the input character variable MESSG.  Thus,
"MESSG(1:ICLOEM(MESSG))" is the non-blank part of MESSG.  (It is assumed
that the non-blank part of MESSG is left-justified in the character string,
with blank fill to the right, as is the case for all NCAR Graphics error
messages in calls to SETER.)
.sp
The argument of ICLOEM is as follows:
.sp
.IP "MESSG" 12
(an input variable or constant of type CHARACTER) - Most likely an error
message returned by a reference to the function SEMESS.
.SH C-BINDING DESCRIPTION 
The C-binding argument descriptions are the same as the FORTRAN 
argument descriptions.
.SH EXAMPLES
Use the ncargex command to see the following relevant
examples: 
tseter,
arex02.
.SH ACCESS
To use ICLOEM, load the NCAR Graphics libraries ncarg, ncarg_gks,
ncarg_c, and ncarg_loc, preferably in that order. To use c_icloem,
load the NCAR Graphics libraries ncargC, ncarg_gksC, ncarg, 
ncarg_gks, and ncarg_loc, preferably in that order.
.SH SEE ALSO
Online:
entsr, eprin, errof, error_handling, fdum, icfell, nerro, retsr,
semess, seter, ncarg_cbind
.SH COPYRIGHT
Copyright 1987, 1988, 1989, 1991, 1993 University Corporation
for Atmospheric Research
.br
All Rights Reserved
