.\"
.\"	$Id: ginq_marker_type.m,v 1.11 2003-05-25 17:16:52 haley Exp $
.\"
.TH GINQ_MARKER_TYPE 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.SH NAME
ginq_marker_type (Inquire marker type) - retrieves the current value for the marker type.
.SH SYNOPSIS
#include <ncarg/gks.h>
.sp
void ginq_marker_type(Gint *err_ind, Gint *marker_type);
.SH DESCRIPTION
.IP err_ind 12
(Output) - If the inquired value cannot be returned correctly,
a non-zero error indicator is returned in err_ind, otherwise a zero is returned.
Consult "User's Guide for NCAR GKS-0A Graphics" for a description of the
meaning of the error indicators.
.IP marker_type 12
(Output) - Returns the current value for the polymarker type as
set by default or by a call to gset_marker_type.
.RS
.IP "1" 
 . (dot)
.IP "2" 
 + (plus)
.IP "3" 
 * (asterisk) This is the default
.IP "4" 
 o (circle)
.IP "5" 
 X (cross)
.RE
.SH ACCESS
To use the GKS C-binding routines, load the ncarg_gks and
ncarg_c libraries.
.SH SEE ALSO
Online: 
.BR gpolymarker(3NCARG),
.BR gset_marker_type(3NCARG),
.BR gset_marker_size(3NCARG),
.BR gset_colr_rep(3NCARG),
.BR gset_marker_colr_ind(3NCARG),
.BR ginq_marker_size(3NCARG),
.BR ginq_marker_colr_ind(3NCARG),
.BR point(3NCARG),
.BR points(3NCARG),
.BR gks(3NCARG),
.BR ncarg_gks_cbind(3NCARG)
.sp
Hardcopy: 
User's Guide for NCAR GKS-0A Graphics;
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
