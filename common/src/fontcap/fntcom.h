C
C	$Id: fntcom.h,v 1.1 1997-01-24 21:59:53 haley Exp $
C
      PARAMETER (IBDIM=15000)
      COMMON /FCAP/BUFFER(IBDIM),BSIZE
C
      COMMON /ERNO/EEOF,ERED,EINM,EORD,EXWD,EYWD,ESWD
      INTEGER      EEOF,ERED,EINM,EORD,EXWD,EYWD,ESWD
C
      COMMON /KVALC/FNAME
      CHARACTER*40 FNAME
C
      PARAMETER (NUMKYS=16)
      COMMON /KEYS/KEYLST
      CHARACTER*20 KEYLST(NUMKYS)
C
      PARAMETER (NUMFTS=12)
      COMMON /FONTNS/FNTNMS
      CHARACTER*40 FNTNMS(NUMFTS)
C
      COMMON /CLINE/LINE
      CHARACTER*80 LINE
C
      COMMON /FNTIO/UNIT  , BLNKL , NBPERI, NBYPWD, PKWID , BITPNT,
     +              VERBOS, LSTPNT, NWRDS , LSIZE
      INTEGER       UNIT  , NBPERI, NBYPWD, PKWID , BITPNT, VERBOS,
     +              LSTPNT, NWRDS
      CHARACTER*80 BLNKL
C
      PARAMETER (NUMNTR=28)
      COMMON /INTRNL/TYPFLG, CHRSTR, CHREND, FRIGHT, FTOP  , FCAPOV, 
     +               FCAP  , FXHOV , FXH   , FHALF , FBASE , FBOT  , 
     +               FCHSWD, FCVSWD, FLLX  , FLLY  , FURX  , FURY  , 
     +               FLLEX , FLLEY , FUREX , FUREY ,
     +               TABPNT, XBITWD, YBITWD, XBIAS , YBIAS , PKFLWD
      INTEGER        TYPFLG, CHRSTR, CHREND, FRIGHT, FTOP  , FCAPOV, 
     +               FCAP  , FXHOV , FXH   , FHALF , FBASE , FBOT  , 
     +               FCHSWD, FCVSWD, FLLX  , FLLY  , FURX  , FURY  , 
     +               FLLEX , FLLEY , FUREX , FUREY ,
     +               TABPNT, XBITWD, YBITWD, XBIAS , YBIAS , PKFLWD 
      INTEGER INTARR(NUMNTR)
      EQUIVALENCE (INTARR,TYPFLG)
C
      COMMON /SFLAGS/SVNUM ,
     +               COORD , ENDR  , BEGINH, ENDH  , BEZIER, ENDC  ,
     +               BEGINC, ENDL
      INTEGER        SVNUM ,
     +               COORD , ENDR  , BEGINH, ENDH  , BEZIER, ENDC  ,
     +               BEGINC, ENDL
