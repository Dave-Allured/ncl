#include <stdio.h>
#include <math.h>
#include "wrapper.h"

extern void NGCALLF(dhyi2hyob,DHYI2HYOB)(double*,double*,double*,double*,
                                         int*,int*,int*,double*,double*,
                                         double*,int*,double*,double*,
                                         double*);

NhlErrorTypes hyi2hyo_W( void )
{
/*
 * Input array variables
 */
  void *p0, *hyai, *hybi, *ps, *xi, *hyao, *hybo;
  double *tmp_p0, *tmp_ps, *tmp_xi; 
  double *tmp_hyai, *tmp_hybi, *tmp_hyao, *tmp_hybo;
  int ndims_ps, dsizes_ps[NCL_MAX_DIMENSIONS];
  int ndims_xi, dsizes_xi[NCL_MAX_DIMENSIONS];
  int dsizes_hyao[1], dsizes_hybo[1], dsizes_hyai[1], dsizes_hybi[1];
  NclBasicDataTypes type_p0, type_ps, type_xi;
  NclBasicDataTypes type_hyai, type_hybi, type_hyao, type_hybo;

/*
 * Work arrays.
 */
  double *pi, *po;

/*
 * Output array variables
 */
  void *xo;
  double *tmp_xo;
  int *dsizes_xo, size_xo, size_leftmost;
  NclBasicDataTypes type_xo;

/*
 * Declare various variables for random purposes.
 */
  int i, index_ps, index_xi, index_xo, l, m;
  int nlat, mlon, klevi, klevo, nlatmlon, klevinlatmlon, klevonlatmlon;

/*
 * Retrieve arguments.
 */
  p0 = (void*)NclGetArgValue(
          0,
          7,
          NULL,
          NULL,
          NULL,
          NULL,
          &type_p0,
          2);

  hyai = (void*)NclGetArgValue(
          1,
          7,
          NULL,
          dsizes_hyai,
          NULL,
          NULL,
          &type_hyai,
          2);

  hybi = (void*)NclGetArgValue(
          2,
          7,
          NULL,
          dsizes_hybi,
          NULL,
          NULL,
          &type_hybi,
          2);

  ps = (void*)NclGetArgValue(
          3,
          7,
          &ndims_ps,
          dsizes_ps,
          NULL,
          NULL,
          &type_ps,
          2);

  if( ndims_ps < 2 ) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"hyi2hyo: The input array 'ps' must be at least 2 dimensions");
    return(NhlFATAL);
  }

  xi = (void*)NclGetArgValue(
          4,
          7,
          &ndims_xi,
          dsizes_xi,
          NULL,
          NULL,
          &type_xi,
          2);

  hyao = (void*)NclGetArgValue(
          5,
          7,
          NULL,
          dsizes_hyao,
          NULL,
          NULL,
          &type_hyao,
          2);

  hybo = (void*)NclGetArgValue(
          6,
          7,
          NULL,
          dsizes_hybo,
          NULL,
          NULL,
          &type_hybo,
          2);

/*
 * Check dimensions.
 */
  if( ndims_ps < 2 ) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"hyi2hyo: The input array 'ps' must be at least 2 dimensions");
    return(NhlFATAL);
  }
  if( ndims_xi < 3 ) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"hyi2hyo: The input array 'xi' must be at least 3 dimensions");
    return(NhlFATAL);
  }

/*
 * Get nlat, mlon, klevi, klevo.
 */ 
  klevi = dsizes_xi[ndims_xi-3];
  nlat  = dsizes_xi[ndims_xi-2];
  mlon  = dsizes_xi[ndims_xi-1];
  klevo = dsizes_hyao[0];
  
/*
 * Check rest of arrays against these dimensions.
 */
  if(dsizes_ps[ndims_ps-2] != nlat || dsizes_ps[ndims_ps-1] != mlon) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"hyi2hyo: The input array 'ps' must have the same two rightmost dimensions as 'xi'");
    return(NhlFATAL);
  }

  if(dsizes_hyai[0] != klevi || dsizes_hybi[0] != klevi) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"hyi2hyo: The input arrays 'hyai' and 'hybi' must be the same as the third rightmost dimension of 'xi'");
    return(NhlFATAL);
  }

  if(dsizes_hybo[0] != klevo) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"hyi2hyo: The output arrays 'hyao' and 'hybo' must be the same length");
    return(NhlFATAL);
  }

  nlatmlon      = nlat * mlon;
  klevinlatmlon = klevi * nlatmlon;
  klevonlatmlon = klevo * nlatmlon;

/*
 * xi must have the same dimensions as ps, only with one more dimension
 * 'klevi'.
 */
  if( ndims_xi != ndims_ps+1 ) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"hyi2hyo: The input array 'xi' must have one more dimension than 'ps'");
    return(NhlFATAL);
  }

/*
 * Check dimension sizes of xi and ps.
 */
  for( i = 0; i < ndims_xi-3; i++ ) {
    if( dsizes_xi[i] != dsizes_ps[i] ) {
      NhlPError(NhlFATAL,NhlEUNKNOWN,"hyi2hyo: The rightmost dimensions of 'ps' and 'xi' must be the same");
      return(NhlFATAL);
    }
  }

/*
 * Compute the size of the leftmost dimensions of the output array
 * (minus the nlat,mlon,klevi dims).
 */
  size_leftmost = 1;
  for( i = 0; i < ndims_xi-3; i++ ) size_leftmost *= dsizes_xi[i]; 
  size_xo = size_leftmost*klevonlatmlon;

/*
 * Allocate space for ps if necessary.
 */
  if(type_ps != NCL_double) {
    tmp_ps = (double *)calloc(nlatmlon,sizeof(double));
    if( tmp_ps == NULL ) {
      NhlPError(NhlFATAL,NhlEUNKNOWN,"hyi2hyo: Unable to allocate memory for coercing ps array to double precision");
      return(NhlFATAL);
    }
  }

/*
 * Allocate space for xi if necessary.
 */
  if(type_xi != NCL_double) {
    tmp_xi = (double *)calloc(klevinlatmlon,sizeof(double));
    if( tmp_xi == NULL ) {
      NhlPError(NhlFATAL,NhlEUNKNOWN,"hyi2hyo: Unable to allocate memory for coercing xi array double precision");
      return(NhlFATAL);
    }
  }

/*
 * Coerce input arrays to double if necessary.
 */
  tmp_p0   = coerce_input_double(p0,type_p0,1,0,NULL,NULL);
  tmp_hyai = coerce_input_double(hyai,type_hyai,klevi,0,NULL,NULL);
  tmp_hybi = coerce_input_double(hybi,type_hybi,klevi,0,NULL,NULL);
  tmp_hyao = coerce_input_double(hyao,type_hyao,klevo,0,NULL,NULL);
  tmp_hybo = coerce_input_double(hybo,type_hybo,klevo,0,NULL,NULL);
  
  if(tmp_hyao == NULL || tmp_hybo == NULL ||
     tmp_hyai == NULL || tmp_hybi == NULL ||
     tmp_p0   == NULL) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"hyi2hyo: Unable to allocate memory for coercing input arrays to double precision");
    return(NhlFATAL);
  }

/*
 * Allocate space for output dimensions.
 */
  dsizes_xo = (int*)calloc(ndims_xi,sizeof(int));  
  if( dsizes_xo == NULL ) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"hyi2hyo: Unable to allocate memory for holding dimension sizes");
    return(NhlFATAL);
  }
  for( i = 0; i < ndims_xi-3; i++ ) {
    dsizes_xo[i] = dsizes_xi[i];
  }
  dsizes_xo[ndims_xi-3] = klevo;
  dsizes_xo[ndims_xi-2] = nlat;
  dsizes_xo[ndims_xi-1] = mlon;

/*
 * Allocate space for output value.
 */
  if(type_ps != NCL_double && type_xi != NCL_double) {
    type_xo = NCL_float;

    tmp_xo = (double *)calloc(klevonlatmlon,sizeof(double));
    xo     = (float *)NclMalloc(size_xo*sizeof(float));

    if(tmp_xo == NULL || xo == NULL ) {
      NhlPError(NhlFATAL,NhlEUNKNOWN,"hyi2hyo: Unable to allocate memory for output array");
      return(NhlFATAL);
    }
  }
  else {
    type_xo = NCL_double;
    xo = (double *)NclMalloc(size_xo*sizeof(double));
    if( xo == NULL ) {
      NhlPError(NhlFATAL,NhlEUNKNOWN,"hyi2hyo: Unable to allocate memory for output array");
      return(NhlFATAL);
    }
  }
/*
 * Allocate space for scratch arrays.
 */
  pi = (double *)NclMalloc(klevi*sizeof(double));
  po = (double *)NclMalloc(klevo*sizeof(double));

  if( pi == NULL || po == NULL) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"hyi2hyo: Unable to allocate memory for work arrays");
    return(NhlFATAL);
  }
/*
 * Call the Fortran version of this routine.
 */
  index_ps = index_xi = index_xo = 0;
  for( i = 0; i < size_leftmost; i++ ) {
/*
 * Coerce subsection of ps/tv array to double.
 */
    if(type_ps != NCL_double) {
      coerce_subset_input_double(ps,tmp_ps,index_ps,type_ps,nlatmlon,0,
                                 NULL,NULL);
    }
    else {
      tmp_ps = &((double*)ps)[index_ps];
    }
    if(type_xi != NCL_double) {
      coerce_subset_input_double(xi,tmp_xi,index_xi,type_xi,klevinlatmlon,0,
                                 NULL,NULL);
    }
    else {
      tmp_xi = &((double*)xi)[index_xi];
    }

    if(type_xo == NCL_double) tmp_xo = &((double*)xo)[index_xo];

    NGCALLF(dhyi2hyob,DHYI2HYOB)(tmp_p0,tmp_hyai,tmp_hybi,tmp_ps,
                                 &mlon,&nlat,&klevi,tmp_xi,tmp_hyao,
                                 tmp_hybo,&klevo,tmp_xo,pi,po);
/*
 * Coerce output to float if necessary.
 */
    if(type_xo != NCL_double) {
      coerce_output_float_only(xo,tmp_xo,klevonlatmlon,index_xo);
     }
    index_ps += nlatmlon;
    index_xi += klevinlatmlon;
    index_xo += klevonlatmlon;
  }

/*
 * Free memory.
 */
  NclFree(pi);
  NclFree(po);

  if(type_p0   != NCL_double) NclFree(tmp_p0);
  if(type_ps   != NCL_double) NclFree(tmp_ps);
  if(type_xi   != NCL_double) NclFree(tmp_xi);
  if(type_hyai != NCL_double) NclFree(tmp_hyai);
  if(type_hybi != NCL_double) NclFree(tmp_hybi);
  if(type_hyao != NCL_double) NclFree(tmp_hyao);
  if(type_hybo != NCL_double) NclFree(tmp_hybo);
  if(type_xo   != NCL_double) NclFree(tmp_xo);

/*
 * Return.
 */
  return(NclReturnValue(xo,ndims_xi,dsizes_xo,NULL,type_xo,0));
}


