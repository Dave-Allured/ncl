.TH ARINAM 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.SH NAME
ARINAM - Initializes area map.
.SH SYNOPSIS
CALL ARINAM (MAP, LMAP)
.SH C-BINDING SYNOPSIS
#include <ncarg/ncargC.h>
.sp
void c_arinam (int *map, int lmap)
.SH DESCRIPTION
.IP "MAP(LMAP)" 12
(output array, of type INTEGER, dimensioned at least LMAP) - 
An integer array in which 
an area map is to be constructed. Each vertex for 
an edge segment in your area map requires ten 
words in the array MAP. Remember that the total 
number of vertexes includes those added at each 
intersection of edges, and those added when long 
edge segments are broken into shorter edge 
segments.
.IP "LMAP"
(an input expression of type INTEGER) - 
Length of the MAP array.
.SH C-BINDING DESCRIPTION 
The C-binding argument descriptions are the same as the Fortran 
argument descriptions.
.SH ACCESS
To use ARINAM, load the NCAR Graphics libraries ncarg, ncarg_gks,
ncarg_c, and ncarg_loc, preferably in that order. To use c_arinam, 
load the NCAR Graphics libraries ncargC, ncarg_gksC, ncarg, 
ncarg_gks, and ncarg_loc, preferably in that order.
.SH SEE ALSO
Online:
areas, areas_params, ardbpa, ardrln, aredam, argeti, argtai, 
arpram, arscam, arseti, ncarg_cbind
.sp
Hardcopy:
NCAR Graphics Contouring and Mapping Tutorial
.SH COPYRIGHT
Copyright 1987, 1988, 1989, 1991, 1993 University Corporation
for Atmospheric Research
.br
All Rights Reserved
