C
C $Id: phi4z0.f,v 1.1 1999-04-02 23:05:55 kennison Exp $
C
      SUBROUTINE PHI4Z0 (ECCNTS,E0,E1,E2,E3,A,B,C,PHI)
C
C SUBROUTINE TO COMPUTE LATITUDE ANGLE (PHI-4).
C
      IMPLICIT DOUBLE PRECISION (A-Z)
      INTEGER IERROR,IPEMSG,IPELUN,IPPARM,IPPLUN
      INTEGER II,NIT
      COMMON /ERRMZ0/ IERROR
      COMMON /PRINZ0/ IPEMSG,IPELUN,IPPARM,IPPLUN
      DATA ONE,TWO,FOUR,SIX /1.0D0,2.0D0,4.0D0,6.0D0/
      DATA TOL,NIT /1.0D-10,15/
C
      PHI = A
      DO 020 II = 1,NIT
      SINPHI = SIN (PHI)
      TANPHI = TAN (PHI)
      C = TANPHI * SQRT (ONE - ECCNTS * SINPHI * SINPHI)
      SIN2PH = SIN (TWO * PHI)
      ML = E0 * PHI - E1 * SIN2PH + E2 * SIN (FOUR * PHI)
     .      - E3 * SIN (SIX * PHI)
      MLP = E0 - TWO * E1 * COS (TWO * PHI) + FOUR * E2 *
     .      COS (FOUR * PHI) - SIX * E3 * COS (SIX * PHI)
      CON1 = TWO * ML + C * (ML * ML + B) - TWO * A *
     .       (C * ML + ONE)
      CON2 = ECCNTS * SIN2PH * (ML * ML + B - TWO * A * ML) / (TWO * C)
      CON3 = TWO * (A - ML) * (C * MLP - TWO / SIN2PH) - TWO * MLP
      DPHI = CON1 / (CON2 + CON3)
      PHI = PHI + DPHI
      IF (ABS(DPHI) .GT. TOL) GO TO 020
      RETURN
  020 CONTINUE
C
      IF (IPEMSG .EQ. 0) WRITE (IPELUN,2000) NIT,E0,E1,E2,E3,A,B,C,
     . ECCNTS
 2000 FORMAT (/' ERROR PHI4Z0' /
     .         ' LATITUDE FAILED TO CONVERGE AFTER',I3,' ITERATIONS'/
     .         ' E0 =',D25.16,'   E1 =',D25.16/
     .         ' E2 =',D25.16,'   E3 =',D25.16/
     .         ' A  =',D25.16,'   B  =',D25.16/
     .         ' C  =',D25.16/
     .         ' ECCENTRICITY SQUARE =',D25.16)
      IERROR = 004
      RETURN
C
      END
