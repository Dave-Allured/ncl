C
C $Id: pj02z0.f,v 1.1 1999-04-02 23:05:56 kennison Exp $
C
      SUBROUTINE PJ02Z0 (COORD,CRDIO,INDIC)
C
C -- S T A T E   P L A N E
C
      IMPLICIT DOUBLE PRECISION (A-Z)
      INTEGER IERROR,IPEMSG,IPELUN,IPPARM,IPPLUN
      INTEGER ITYPE
      INTEGER INDIC, FWD, INV
      DIMENSION GEOG(2),PROJ(2),COORD(2),CRDIO(2)
      COMMON /ERRMZ0/ IERROR
      COMMON /PRINZ0/ IPEMSG,IPELUN,IPPARM,IPPLUN
      COMMON /PJ02/ ITYPE
C
      PARAMETER (FWD=0, INV=1)
C
C -- F O R W A R D   . . .
C
      IF (INDIC .EQ. 0) THEN
C
         GEOG(1) = COORD(1)
         GEOG(2) = COORD(2)
         IERROR = 0
C
C     TRANSVERSE MERCATOR PROJECTION
C
         IF (ITYPE .EQ. 1) THEN
            CALL PJ09Z0 (GEOG,PROJ,FWD)
         END IF
C
C     LAMBERT CONFORMAL PROJECTION
C
         IF (ITYPE .EQ. 2) THEN
            CALL PJ04Z0 (GEOG,PROJ,FWD)
         END IF
C
C     POLYCONIC PROJECTION
C
         IF (ITYPE .EQ. 3) THEN
            CALL PJ07Z0 (GEOG,PROJ,FWD)
         END IF
C
C     OBLIQUE MERCATOR PROJECTION
C
         IF (ITYPE .EQ. 4) THEN
            CALL PJ20Z0 (GEOG,PROJ,FWD)
         END IF
C
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
C
C     TRANSVERSE MERCATOR PROJECTION
C
         IF (ITYPE .EQ. 1) THEN
            CALL PJ09Z0 (PROJ,GEOG,INV)
         END IF
C
C     LAMBERT CONFORMAL PROJECTION
C
         IF (ITYPE .EQ. 2) THEN
            CALL PJ04Z0 (PROJ,GEOG,INV)
         END IF
C
C     POLYCONIC PROJECTION
C
         IF (ITYPE .EQ. 3) THEN
            CALL PJ07Z0 (PROJ,GEOG,INV)
         END IF
C
C     OBLIQUE MERCATOR PROJECTION
C
         IF (ITYPE .EQ. 4) THEN
            CALL PJ20Z0 (PROJ,GEOG,INV)
         END IF
C
         CRDIO(1) = GEOG(1)
         CRDIO(2) = GEOG(2)
         RETURN
      END IF
C
      END
