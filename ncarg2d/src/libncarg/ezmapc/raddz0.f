C
C $Id: raddz0.f,v 1.1 1999-04-02 23:06:05 kennison Exp $
C
      SUBROUTINE RADDZ0 (RAD,SGNA,DEGS,MINS,SECS)
C
C SUBROUTINE TO CONVERT ANGLE FROM RADIANS TO SIGNED DMS
C SGNA : SIGN OF ANGLE
C DEGS : DEGREES PORTION OF ANGLE
C MINS : MINUTES PORTION OF ANGLE
C SECS : SECONDS PORTION OF ANGLE
C
      DOUBLE PRECISION RAD,CON,RADSEC,ZERO,TOL
      REAL SECS
      INTEGER DEGS,MINS
      CHARACTER*1 SGNA,BLANK,NEG
      DATA RADSEC /206264.806247D0/
      DATA ZERO,TOL /0.0D0,1.0D-4/
      DATA BLANK,NEG /' ','-'/
C
C CONVERT THE ANGLE TO SECONDS.
C
      CON = ABS(RAD) * RADSEC
      ISEC = INT(CON + TOL)
C
C DETERMINE THE SIGN OF THE ANGLE.
C
      SGNA = BLANK
      IF (RAD .LT. ZERO .AND. CON .GE. 0.00005D0) SGNA = NEG
      IF (CON .LT. 0.00005D0) CON = ZERO
C
C COMPUTE DEGREES PART OF THE ANGLE.
C
      INTG = ISEC / 3600
      DEGS = INTG
      ISEC = INTG * 3600
      CON = CON - ISEC
      ISEC = INT(CON + TOL)
C
C COMPUTE MINUTES PART OF THE ANGLE.
C
      MINS = ISEC / 60
      ISEC = MINS * 60
      CON = CON - ISEC
C
C COMPUTE SECONDS PART OF THE ANGLE.
C
      SECS = CON
C
C     INCREASE MINS IF SECS CLOSE TO 60.000
C
      IF(SECS .LT. 59.9995D0) RETURN
      MINS = MINS + 1
      SECS = 0.0
C
C     INCREASE DEGS IF MINS EQUAL 60
C
      IF(MINS .LE. 59) RETURN
      MINS = 0
      DEGS = DEGS + 1
C
      RETURN
      END
