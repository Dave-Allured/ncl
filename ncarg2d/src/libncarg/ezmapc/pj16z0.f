C
C $Id: pj16z0.f,v 1.1 1999-04-02 23:06:01 kennison Exp $
C
      SUBROUTINE PJ16Z0 (COORD,CRDIO,INDIC)
C
C -- S I N U S O I D A L
C
      IMPLICIT DOUBLE PRECISION (A-Z)
      INTEGER IERROR,IPEMSG,IPELUN,IPPARM,IPPLUN
      INTEGER INDIC
      DIMENSION GEOG(2),PROJ(2),COORD(2),CRDIO(2)
C **** PARAMETERS **** A,LON0,X0,Y0 ************************************
      COMMON /ERRMZ0/ IERROR
      COMMON /PRINZ0/ IPEMSG,IPELUN,IPPARM,IPPLUN
      COMMON /PJ16/ A,LON0,X0,Y0
      DATA HALFPI /1.5707963267948966D0/
      DATA EPSLN /1.0D-10/
C
C -- F O R W A R D   . . .
C
      IF (INDIC .EQ. 0) THEN
C
         GEOG(1) = COORD(1)
         GEOG(2) = COORD(2)
         IERROR = 0
         LON = ADJLZ0 (GEOG(1) - LON0)
         PROJ(1) = X0 + A * LON * COS (GEOG(2))
         PROJ(2) = Y0 + A * GEOG(2)
         CRDIO(1) = PROJ(1)
         CRDIO(2) = PROJ(2)
         RETURN
      END IF
C
C -- I N V E R S E   . . .
C
      IF (INDIC .EQ. 1) THEN
C
         PROJ(1) = COORD(1)
         PROJ(2) = COORD(2)
         IERROR = 0
         X = PROJ(1) - X0
         Y = PROJ(2) - Y0
         GEOG(2) = Y / A
         IF (ABS(GEOG(2)) .LE. HALFPI) GO TO 230
         IF (IPEMSG .EQ. 0) WRITE (IPELUN,2020)
 2020    FORMAT (/' ERROR PJ16Z0'/
     .            ' INPUT DATA ERROR')
         IERROR = 164
         RETURN
  230    CON = ABS (GEOG(2)) - HALFPI
         IF (ABS (CON) .GT. EPSLN) GO TO 240
         GEOG(1) = LON0
         CRDIO(1) = GEOG(1)
         CRDIO(2) = GEOG(2)
         RETURN
  240    GEOG(1) = ADJLZ0 (LON0 + X / (A * COS (GEOG(2))))
         CRDIO(1) = GEOG(1)
         CRDIO(2) = GEOG(2)
         RETURN
      END IF
C
      END
