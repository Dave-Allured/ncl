.TH Areas_params 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
Areas_params - This document briefly describes all Areas 
parameters.
.SH DESCRIPTION
Areas currently supports five parameters, all of type INTEGER. These 
parameters are useful mostly for debugging Areas problems. The 
current values may be retrieved using the routine ARGETI. Parameter 
values may be reset using the routine ARSETI; the DI parameter is 
for retrieval only and may not be given a new value by the user.
.sp
The Areas parameter descriptions appear below in alphabetical 
order. Each description begins with a line giving the parameter name 
and the intrinsic FORTRAN type of the parameter.
.IP "\&'AT' - Integer"
Arithmetic Type flag. AT allows the user to check the type of 
arithmetic to be used by Areas.
.RS
.IP "\(<= 0" 4
Areas decides what sort of arithmetic to use.
.IP 1 4
Real arithmetic is to be used.
.IP 2 4
Double precision arithmetic is to be used.
.IP 3 4
Multiple precision arithmetic is used; Areas should choose the base 
value.
.IP "\(>= 4" 4
AT specifies the base value. (For example, the value 100 would 
specify the use of base-100 multiple-precision integer arithmetic.)
.RE
Default: 0
.sp
Note: You should use a nonzero value of AT only 
on the recommendation of an NCAR consultant.
.IP "\&'DB' - Integer"
DeBug plots flag. DB allows the user to study the area map.
.RS
.IP 0 4
Debugging turned off. This is the default.
.IP "\(>= 1" 4
Debugging is on. At selected break points, ARPRAM produces plots 
showing all edge segments in the area map that belong to the group 
with group identifier DB.
.RE
.IP "\&'DC' - Integer"
Debug Colors index. DC tells ARDBPA to use color indices DC+1 
through DC+5 for debugging colors.
.sp
By default, DC=100, so ARDBPA defines and 
uses color indices 101 through 106.
.IP "\&'DI' - Integer"
Direction Indicator flag. DI tells the user in which direction the edge 
of an area is given. For retrieval only, the value of DI cannot be set 
by the user. The two possible values of DI are:
.RS
.IP1 4
Edge of the area is given in counterclockwise order (with the 
interior to the left).
.IP 2 4
Edge of the area is given in clockwise order (with the interior to the 
right).
.RE
Note: These values are only meaningful when the ARGETI call 
originates from the user-written routine (dummy argument "APR") 
that is called by ARSCAM. It gives you information about the area 
whose polygonal boundary is defined by the values of APR's 
arguments.
.IP "\&'LC' - Integer"
Largest Coordinate. LC lets you specify the largest coordinate 
allowed in an area map. X and Y coordinates in an area map are 
represented by integers in the range from 0 to LC, inclusive; the 
default value of LC is 1000000.
.sp
The minimum value alowed for LC is 1000; attempting to set 
LC<1000 gives it the value 1000. The value of LC must not be greater 
than the largest integer on the machine on which Areas is running; 
its value must also be exactly representable as a real number on 
that machine.

