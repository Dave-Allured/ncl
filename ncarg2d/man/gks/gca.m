.\"
.\"	$Id: gca.m,v 1.11 2003-05-25 17:16:50 haley Exp $
.\"
.TH GCA 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.SH NAME
GCA (Cell array) - draws a cell array starting with a 
rectangle specified by two corner points; this initial 
rectangle is subdivided into subrectangles by 
specifying an arbitrary number of divisions in the X 
and Y directions; each subrectangle (or cell) is 
assigned a color and drawn with that color.
.SH SYNOPSIS
CALL GCA (XP1, YP1, XP2, YP2, NXCLR, NYCLR, IXCLR, IYCLR, NXDO, NYDO, CLRIA)
.SH C-BINDING SYNOPSIS
#include <ncarg/gks.h>
.sp
void gcell_array(const Grect *rect, const Gpat_rep *colr_array);
.SH DESCRIPTION
.IP XP1 12
(Real, Input) - X world coordinate of point 1.
.IP YP1 12
(Real, Input) - Y world coordinate of point 1.
.IP XP2 12
(Real, Input) - X world coordinate of point 2.
.IP YP2 12
(Real, Input) - Y world coordinate of point 2.
.IP NXCLR 12
(Integer, Input) - X dimension of the color index array 
CLRIA.
.IP NYCLR 12
(Integer, Input) - Y dimension of the color index array 
CLRIA.
.IP IXCLR 12
(Integer, Input) - The index of the starting column in 
index array CLRIA.
.IP IYCLR 12
(Integer, Input) - The index of the starting row in 
index array CLRIA.
.IP NXDO 12
(Integer, Input) - Number of cell elements to draw in 
the X dimension.
.IP NYDO 12
(Integer, Input) - Number of cell elements to draw in
the Y dimension.
.IP CLRIA 12
(Integer array, Input) - An array of color indices 
dimensioned NXCLR x NYCLR which is 
used to determine the color of each 
cell drawn.
.SH USAGE
Points 1 and 2 must be diagonally opposite corner 
points of a rectangle to be divided into NXDO cells in 
the X dimension and NYDO cells in the Y dimension.
.SH ACCESS
To use GKS routines, load the NCAR GKS-0A library 
ncarg_gks.
.SH SEE ALSO
Online:
gscr, gcell_array
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
