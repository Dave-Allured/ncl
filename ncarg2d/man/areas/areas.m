.TH AREAS 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.SH NAME
Areas - A set of routines to create an area map from a set of
edges.
.SH SYNOPSIS
ARINAM - Initializes areas.
.br
AREDAM - Adds edges to an area map.
.br
ARPRAM - Preprocesses an area map.
.br
ARSCAM - Obtains definitions of areas created by edges inserted
into area map.
.br
ARDRLN - Draws a polyline masked by a given area map.
.br
ARGTAI - Gets area identifiers associated with a given point.
.br
ARGETI - Retrieves an Areas integer parameter value.
.br
ARSETI - Sets an Areas integer parameter value.
.br
ARDBPA - For debugging, produces a picture of that part of
the area map that belongs to a specified group.
.SH C-BINDING SYNOPSIS
#include <ncarg/ncargC.h>
.sp
c_arinam
.br
c_aredam
.br
c_arpram
.br
c_arscam
.br
c_ardrln
.br
c_argtai
.br
c_argeti
.br
c_arseti
.br
c_ardbpa
.SH ACCESS
To use Areas, load the NCAR Graphics libraries ncarg, ncarg_gks,
ncarg_c, and ncarg_loc, preferably in that order. To use the Areas 
C-bindings, load the NCAR Graphics libraries ncargC, ncarg_gksC, 
ncarg, ncarg_gks, ncarg_c, and ncarg_loc, preferably in that order.
.SH SEE ALSO
Online:
areas_params, ardbpa, ardrln, aredam, argeti, argtai, arinam, 
arpram, arscam, arseti, ncarg_cbind
.sp
Hardcopy:
NCAR Graphics Contouring and Mapping Tutorial
.SH COPYRIGHT
Copyright 1987, 1988, 1989, 1991, 1993 University Corporation
for Atmospheric Research
.br
All Rights Reserved
