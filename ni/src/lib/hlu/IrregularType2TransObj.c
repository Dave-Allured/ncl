/*
 *      $Id: IrregularType2TransObj.c,v 1.9 1994-05-05 18:16:35 ethan Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1992			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		IrregularType2TransObj.c
 *
 *	Author:		Ethan Alpert
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Mon Oct 19 09:33:44 MDT 1992
 *
 *	Description:	Takes one coordinate system and maps it into another
 *			Both coordinate systems must be monotonic. This will
 *			be used by contour, etc to map integer grid point
 *			indexes to irregularly spaced points so they can then
 *			be drawn on the linear viewport. Traditionally a
 *			funtion CPSPS2 has been used to interpolate the 
 *			irregular grids onto regular grids. This is not needed 
 *			since conpack can contour using the integer grids and 
 *			use this irregular transformation object to map from the
 *			integer coordinate system to the irregular one.
 *
 *			Eventually the methods here will be called from within
 *			the CPMPXY function used by CONPACK and eventually by
 *			the rest of the 2D utilities.
 *
 *			The type 2 version of irregular grid transformations
 *			differs from the original in that it sets the window
 *			to be identical to the computational coordinates. This
 *			has the effect of placing the irregularly spaced 
 *			coordinates onto the frame and even spacings, thus 
 *			streching or deforming the grid so that all data can
 *			be view. The original returns points back to their
 *			data values before rendering. For some grids this has
 *			the effect of "crowding" several grid points in to one
 *			small screen location.
 *
 *
 */
#include <stdio.h>
#include <ncarg/hlu/hluutil.h>
#include <ncarg/hlu/hluP.h>
#include <ncarg/hlu/IrregularType2TransObjP.h>
#include <ncarg/hlu/View.h>


static NhlResource resources[] = {
	{ NhlNtrXCoordPoints,NhlCtrXCoordPoints,NhlTFloatPtr,sizeof(float*),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.x_coord_points),
		NhlTFloatPtr,NULL,0,(NhlFreeFunc)NhlFree },
	{ NhlNtrXInterPoints,NhlCtrXInterPoints,NhlTFloatPtr,sizeof(float*),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.x_inter_points),
		NhlTFloatPtr,NULL ,0,(NhlFreeFunc)NhlFree},
	{ NhlNtrXMaxF, NhlCtrXMaxF, NhlTFloat, sizeof(float),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.x_max),
		NhlTString, "0.0" ,0,NULL},
	{ NhlNtrXMinF, NhlCtrXMinF, NhlTFloat, sizeof(float),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.x_min),
		NhlTString, "0.0" ,0,NULL},
	{ NhlNtrXNumPoints,NhlCtrXNumPoints,NhlTInteger,sizeof(int),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.x_num_points),
		NhlTString,"0" ,0,NULL},
	{ NhlNtrXReverse, NhlCtrXReverse, NhlTInteger,sizeof(int),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.x_reverse),
		NhlTString,"0" ,0,NULL},
	{ NhlNtrXTensionF, NhlCtrXTensionF, NhlTFloat, sizeof(float),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.x_tension),
		NhlTString,"2.0" ,0,NULL},
	{ NhlNtrXSamples, NhlCtrXSamples, NhlTInteger, sizeof(int),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.x_samples),
		NhlTString,"9" ,0,NULL},
	{ NhlNtrXUseLog, NhlCtrXUseLog, NhlTInteger, sizeof(int),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.x_use_log),
		NhlTString,"0" ,0,NULL},
	{ NhlNtrYCoordPoints,NhlCtrYCoordPoints,NhlTFloatPtr,sizeof(float*),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.y_coord_points),
		NhlTFloatPtr,NULL ,0,(NhlFreeFunc)NhlFree},
	{ NhlNtrYInterPoints,NhlCtrYInterPoints,NhlTFloatPtr,sizeof(float*),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.y_inter_points),
		NhlTFloatPtr,NULL ,0,(NhlFreeFunc)NhlFree},
	{ NhlNtrYMaxF, NhlCtrYMaxF, NhlTFloat, sizeof(float),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.y_max),
		NhlTString, "0.0" ,0,NULL},
	{ NhlNtrYMinF, NhlCtrYMinF, NhlTFloat, sizeof(float),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.y_min),
		NhlTString, "0.0" ,0,NULL},
	{ NhlNtrYNumPoints,NhlCtrYNumPoints,NhlTInteger,sizeof(int),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.y_num_points),
		NhlTString,"0" ,0,NULL},
	{ NhlNtrYReverse, NhlCtrYReverse, NhlTInteger,sizeof(int),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.y_reverse),
		NhlTString,"0" ,0,NULL},
	{ NhlNtrYTensionF, NhlCtrYTensionF, NhlTFloat, sizeof(float),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.y_tension),
		NhlTString,"2.0" ,0,NULL},
	{ NhlNtrYSamples, NhlCtrYSamples, NhlTInteger, sizeof(int),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.y_samples),
		NhlTString,"9" ,0,NULL},
	{ NhlNtrYUseLog, NhlCtrYUseLog, NhlTInteger, sizeof(int),
		NhlOffset(NhlIrregularType2TransObjLayerRec,ir2trans.y_use_log),
		NhlTString,"0" ,0,NULL}
};

/*
* BaseClass Methods defined
*/

static NhlErrorTypes  IrTransSetValues(
#ifdef NhlNeedProto
        NhlLayer,          /* old */
        NhlLayer,          /* reference */
        NhlLayer,          /* new */
        _NhlArgList,    /* args */
        int             /* num_args*/
#endif
);

static NhlErrorTypes IrTransInitialize(
#ifdef NhlNeedProto
        NhlLayerClass,     /* class */
        NhlLayer,          /* req */
        NhlLayer,          /* new */
        _NhlArgList,    /* args */
        int             /* num_args */
#endif
);


/*
* TransObjClass Methods defined
*/

static NhlErrorTypes IrSetTrans(
#ifdef NhlNeedProto
NhlLayer	/*instance*/,
NhlLayer  /*parent*/
#endif
);

static NhlErrorTypes IrWinToNDC(
#ifdef NhlNeedProto
NhlLayer	/*instance*/,
NhlLayer	/* parent */,
float*	/*x*/,
float*   /*y*/,
int	/* n*/,
float*	/*xout*/,
float*	/*yout*/,
float*	/*xmissing*/,
float*	/*ymissing*/,
int * 	/* status */
#endif
);


static NhlErrorTypes IrNDCToWin(
#ifdef NhlNeedProto
NhlLayer	/*instance*/,
NhlLayer	/*parent */,
float*	/*x*/,
float*   /*y*/,
int	/* n*/,
float*	/*xout*/,
float*	/*yout*/,
float*	/*xmissing*/,
float*	/*ymissing*/,
int *   /* status */
#endif
);


static NhlErrorTypes IrDataToCompc(
#ifdef NhlNeedProto
NhlLayer   /*instance */,
NhlLayer   /*parent */,
float*  /*x*/,
float*   /*y*/,
int     /* n*/,
float*  /*xout*/,
float*  /*yout*/,
float*	/*xmissing*/,
float*	/*ymissing*/,
int *   /* status */
#endif
);

static NhlErrorTypes IrCompcToData(
#ifdef NhlNeedProto
NhlLayer   /*instance */,
NhlLayer   /*parent */,
float*  /*x*/,
float*   /*y*/,
int     /* n*/,
float*  /*xout*/,
float*  /*yout*/,
float*	/*xmissing*/,
float*	/*ymissing*/,
int *   /* status */
#endif
);

static NhlErrorTypes IrWinToCompc(
#ifdef NhlNeedProto
NhlLayer   /*instance */,
NhlLayer   /*parent */,
float*  /*x*/,
float*   /*y*/,
int     /* n*/,
float*  /*xout*/,
float*  /*yout*/,
float*	/*xmissing*/,
float*	/*ymissing*/,
int *   /* status */
#endif
);



static NhlErrorTypes IrNDCLineTo(
#if     NhlNeedProto
NhlLayer   /* instance */,
NhlLayer   /* parent */,
float   /* x */,
float   /* y */,
int     /* upordown */
#endif
);
static NhlErrorTypes IrDataLineTo(
#if     NhlNeedProto
NhlLayer   /* instance */,
NhlLayer   /* parent */,
float   /* x */,
float   /* y */,
int     /* upordown */
#endif
);

static NhlErrorTypes IrWinLineTo(
#if     NhlNeedProto
NhlLayer   /* instance */,
NhlLayer   /* parent */,
float   /* x */,
float   /* y */,
int     /* upordown */
#endif
);

static SetUpTrans(
#if NhlNeedProto
        NhlLayer   /*new*/,
        NhlLayer   /*old*/,
	int	/*c_or_s*/,
	_NhlArgList /*args*/, 
	int /*nargs*/
#endif
);

#define CREATE  1
#define SET 0


NhlIrregularType2TransObjLayerClassRec NhlirregularType2TransObjLayerClassRec = {
        {
/* class_name			*/	"IrregularType2TransObj",
/* nrm_class			*/	NrmNULLQUARK,
/* layer_size			*/	sizeof(NhlIrregularType2TransObjLayerRec),
/* class_inited			*/	False,
/* superclass			*/	(NhlLayerClass)&NhltransObjLayerClassRec,

/* layer_resources		*/	resources,
/* num_resources		*/	NhlNumber(resources),
/* all_resources		*/	NULL,

/* class_part_initialize	*/	NULL,
/* class_initialize		*/	NULL,
/* layer_initialize		*/	IrTransInitialize,
/* layer_set_values		*/	IrTransSetValues,
/* layer_set_values_hook	*/	NULL,
/* layer_get_values		*/	NULL,
/* layer_reparent		*/	NULL,
/* layer_destroy		*/	NULL
        },
        {
/* set_trans		*/	IrSetTrans,
/* trans_type		*/	NULL,
/* win_to_ndc		*/	IrWinToNDC,
/* ndc_to_win		*/	IrNDCToWin,
/* data_to_win		*/	IrDataToCompc, 
/* win_to_data		*/	IrCompcToData, 
/* data_to_compc	*/	IrDataToCompc,
/* compc_to_data	*/	IrCompcToData,
/* win_to_compc		*/	IrWinToCompc,
/* compc_to_win		*/	IrWinToCompc,
/* data_lineto 		*/      IrDataLineTo,
/* compc_lineto		*/      IrWinLineTo,
/* win_lineto 		*/      IrWinLineTo,
/* NDC_lineto 		*/      IrNDCLineTo
        }
};

NhlLayerClass NhlirregularType2TransObjLayerClass =
			(NhlLayerClass)&NhlirregularType2TransObjLayerClassRec;




/*
 * Function:	IrTransSetValues
 *
 * Description:	SetValues method for IrregularType2Trans Objects
 *
 * In Args: 	All standard ones for set_values method.
 *
 * Out Args:	Same as all set_values methods;
 *
 * Return Values: Error status
 *
 * Side Effects: Allocates and copies space for array resources
 */
/*ARGSUSED*/
static NhlErrorTypes IrTransSetValues
#if __STDC__
(NhlLayer old, NhlLayer reference, NhlLayer new, _NhlArgList args, int num_args)
#else
(old,reference, new,args,num_args)
	NhlLayer old;
	NhlLayer reference;
	NhlLayer new;
	_NhlArgList args;
	int	num_args;
#endif
{
	return(SetUpTrans(new,old,SET,args,num_args));	
}


/*
 * Function:	IrTransInitialize
 *
 * Description: Initialize function for IrregularType2TransObjs. Performs same
 *		operations as set_values for copying array resources
 *
 * In Args:  	Standard layer_initialize arguments.
 *
 * Out Args:	Standard layer_initialize output.
 *
 * Return Values: Error Status
 *
 * Side Effects: allocates space and copies valus of array resources.
 */
/*ARGSUSED*/
static NhlErrorTypes IrTransInitialize
#if __STDC__
( NhlLayerClass class, NhlLayer req, NhlLayer new, _NhlArgList args, int num_args)
#else
(class,req,new,args,num_args)
        NhlLayerClass	class;
        NhlLayer		req;
        NhlLayer		new;
        _NhlArgList	args;
        int		num_args;
#endif
{
	return(SetUpTrans(new,NULL,CREATE,args,num_args));
}

static SetUpTrans
#if __STDC__
(NhlLayer new, NhlLayer old, int c_or_s,_NhlArgList args, int nargs)
#else
(new,old,c_or_s,args, nargs)
	NhlLayer 	new;
	NhlLayer	old;
	int c_or_s;
	_NhlArgList args;
	int nargs;
#endif
{
	NhlIrregularType2TransObjLayer inew = (NhlIrregularType2TransObjLayer)new;
	NhlIrregularType2TransObjLayer iold = (NhlIrregularType2TransObjLayer)old;
	char *error_lead;
	float *tmp;
	float tmpf;
	int call_spline_create;
	NhlErrorTypes ret = NhlNOERROR;
	NhlStatus xstatus,ystatus;

	if(c_or_s == SET) {
		error_lead = "IrTransSetValues";
		call_spline_create = 0;
	} else {
		error_lead = "IrTransInitialize";
		call_spline_create = 1;
	}

	if((inew->ir2trans.x_coord_points == NULL)||
			(inew->ir2trans.y_coord_points==NULL)||
			(inew->ir2trans.x_num_points < 2)||
			(inew->ir2trans.y_num_points < 2)) {
		if(c_or_s == CREATE) {
			NhlPError(NhlFATAL,NhlEUNKNOWN,
			"%s: Not enough information to set up transformations",
								error_lead);
			return(NhlFATAL);
		} else {
			memcpy((char*)&inew->ir2trans,(char*)&iold->ir2trans,
				sizeof(NhlIrregularType2TransObjLayerPart));
			
			
			NhlPError(NhlWARNING,NhlEUNKNOWN,"%s: Not enough information to set up transformations, reseting to previous values",error_lead);	
			return(NhlWARNING);
		}
	} else {
		if(c_or_s == SET){
			if(_NhlArgIsSet(args,nargs,NhlNtrXCoordPoints)) {
				call_spline_create = 1;
				if(iold->ir2trans.x_coord_points != NULL) {
					NhlFree(iold->ir2trans.x_coord_points);
				}
				tmp = inew->ir2trans.x_coord_points;
				inew->ir2trans.x_coord_points 
					= (float*)NhlMalloc((unsigned)
					sizeof(float) *(inew->ir2trans.x_num_points));
				memcpy((char*)inew->ir2trans.x_coord_points,
					(char*)tmp,sizeof(float) *
						inew->ir2trans.x_num_points);
				
			}
			if(_NhlArgIsSet(args,nargs,NhlNtrYCoordPoints)) {
				call_spline_create = 1;
				if(iold->ir2trans.y_coord_points != NULL) {
					NhlFree(iold->ir2trans.y_coord_points);
				}
				tmp = inew->ir2trans.y_coord_points;
				inew->ir2trans.y_coord_points 
					= (float*)NhlMalloc((unsigned)
					sizeof(float) *(inew->ir2trans.y_num_points));
				memcpy((char*)inew->ir2trans.y_coord_points,
					(char*)tmp,sizeof(float) *
						inew->ir2trans.y_num_points);
			}
			
		} else {
			tmp = inew->ir2trans.x_coord_points;
			inew->ir2trans.x_coord_points = (float*)NhlMalloc((unsigned)
					sizeof(float) *(inew->ir2trans.x_num_points));
			memcpy((char*)inew->ir2trans.x_coord_points,(char*)tmp,
				sizeof(float)*inew->ir2trans.x_num_points);

			tmp = inew->ir2trans.y_coord_points;

			inew->ir2trans.y_coord_points = (float*)NhlMalloc((unsigned)
					sizeof(float) *(inew->ir2trans.y_num_points));
			memcpy((char*)inew->ir2trans.y_coord_points,(char*)tmp,
				sizeof(float)*inew->ir2trans.y_num_points);

		}


		if((inew->ir2trans.x_min == 0.0)&&
			(inew->ir2trans.x_max == 0.0)) {
			inew->ir2trans.x_min = MIN(inew->ir2trans.x_coord_points[0],inew->ir2trans.x_coord_points[inew->ir2trans.x_num_points-1]);
			inew->ir2trans.x_max = MAX(inew->ir2trans.x_coord_points[0],inew->ir2trans.x_coord_points[inew->ir2trans.x_num_points-1]);
		} else if(inew->ir2trans.x_min > inew->ir2trans.x_max) {
			tmpf = inew->ir2trans.x_min;
			inew->ir2trans.x_min = inew->ir2trans.x_max;
			inew->ir2trans.x_max = tmpf;
		}
		if(inew->ir2trans.x_min < MIN(inew->ir2trans.x_coord_points[0],inew->ir2trans.x_coord_points[inew->ir2trans.x_num_points-1])) {
			NhlPError(NhlWARNING,NhlEUNKNOWN,"%s: Minimum value is less than minimum value of coordinate points array, resetting to minimum",error_lead);
			inew->ir2trans.x_min = MIN(inew->ir2trans.x_coord_points[0],inew->ir2trans.x_coord_points[inew->ir2trans.x_num_points-1]);
		}
		if(inew->ir2trans.x_max > MAX(inew->ir2trans.x_coord_points[0],inew->ir2trans.x_coord_points[inew->ir2trans.x_num_points-1])) {
			NhlPError(NhlWARNING,NhlEUNKNOWN,"%s: Maximum value is greater than maximum value of coordinate points array, resetting to maximum",error_lead);
			inew->ir2trans.x_max = MAX(inew->ir2trans.x_coord_points[0],inew->ir2trans.x_coord_points[inew->ir2trans.x_num_points-1]);
		}
		if((inew->ir2trans.y_min == 0.0)&&
			(inew->ir2trans.y_max == 0.0)) {
			inew->ir2trans.y_min = MIN(inew->ir2trans.y_coord_points[0],inew->ir2trans.y_coord_points[inew->ir2trans.y_num_points-1]);
			inew->ir2trans.y_max = MAX(inew->ir2trans.y_coord_points[0],inew->ir2trans.y_coord_points[inew->ir2trans.y_num_points-1]);
		} else if(inew->ir2trans.y_min > inew->ir2trans.y_max) {
			tmpf = inew->ir2trans.y_min;
			inew->ir2trans.y_min = inew->ir2trans.y_max;
			inew->ir2trans.y_max = tmpf;
		}
		if(inew->ir2trans.y_min < MIN(inew->ir2trans.y_coord_points[0],inew->ir2trans.y_coord_points[inew->ir2trans.y_num_points-1])) {
			NhlPError(NhlWARNING,NhlEUNKNOWN,"%s: Minimum value is less than minimum value of coordinate points array, resetting to minimum",error_lead);
			inew->ir2trans.y_min = MIN(inew->ir2trans.y_coord_points[0],inew->ir2trans.y_coord_points[inew->ir2trans.y_num_points-1]);
		}
		if(inew->ir2trans.y_max > MAX(inew->ir2trans.y_coord_points[0],inew->ir2trans.y_coord_points[inew->ir2trans.y_num_points-1])) {
			NhlPError(NhlWARNING,NhlEUNKNOWN,"%s: Maximum value is greater than maximum value of coordinate points array, resetting to maximum",error_lead);
			inew->ir2trans.y_max = MAX(inew->ir2trans.y_coord_points[0],inew->ir2trans.y_coord_points[inew->ir2trans.y_num_points-1]);
		}
		
	}
	if(_NhlArgIsSet(args,nargs,NhlNtrXInterPoints)) {
		if((c_or_s == SET)&&(iold->ir2trans.x_inter_points != NULL)) {
			NhlFree(iold->ir2trans.x_inter_points);
		}
		tmp = inew->ir2trans.x_inter_points;
		inew->ir2trans.x_inter_points = (float*)NhlMalloc((unsigned)
				sizeof(float) * (inew->ir2trans.x_num_points));
		memcpy((char*)inew->ir2trans.x_inter_points,(char*)tmp,
			sizeof(float)*inew->ir2trans.x_num_points);
		call_spline_create = 1;
	}
	
	if(_NhlArgIsSet(args,nargs,NhlNtrYInterPoints)) {
		if((c_or_s == SET)&&(iold->ir2trans.y_inter_points != NULL)) {
			NhlFree(iold->ir2trans.y_inter_points);
		}
		tmp = inew->ir2trans.y_inter_points;
		inew->ir2trans.y_inter_points = (float*)NhlMalloc((unsigned)
				sizeof(float) * (inew->ir2trans.y_num_points));
		memcpy((char*)inew->ir2trans.y_inter_points,(char*)tmp,
			sizeof(float)*inew->ir2trans.y_num_points);
		call_spline_create = 1;
	}
	if((c_or_s == SET)&&(inew->ir2trans.y_tension != 
		iold->ir2trans.y_tension)) {
		call_spline_create = 1;
	}
	if((c_or_s == SET)&&(inew->ir2trans.x_tension != 
		iold->ir2trans.x_tension)) {
		call_spline_create = 1;
	}

	if(call_spline_create) {
		ret = _NhlCreateSplineCoordApprox(&(inew->ir2trans.thecoord),
			inew->ir2trans.x_use_log,
			inew->ir2trans.x_coord_points,
			inew->ir2trans.x_inter_points,
			inew->ir2trans.x_num_points,
			inew->ir2trans.y_use_log,
			inew->ir2trans.y_coord_points,
			inew->ir2trans.y_inter_points,
			inew->ir2trans.y_num_points,
			inew->ir2trans.x_tension,inew->ir2trans.y_tension,
			inew->ir2trans.x_samples,inew->ir2trans.y_samples,
			&xstatus,&ystatus);	
	}
	_NhlEvalSplineCoordForward(&inew->ir2trans.thecoord,
		inew->ir2trans.x_min,
		inew->ir2trans.y_min,
		&(inew->ir2trans.ul),
		&(inew->ir2trans.ub),
		NULL,NULL);
	_NhlEvalSplineCoordForward(&inew->ir2trans.thecoord,
		inew->ir2trans.x_max,
		inew->ir2trans.y_max,
		&(inew->ir2trans.ur),
		&(inew->ir2trans.ut),
		NULL,NULL);

	if(inew->ir2trans.x_reverse) {
		tmpf = inew->ir2trans.ur;
		inew->ir2trans.ur = inew->ir2trans.ul;
		inew->ir2trans.ul = tmpf;
	}
	if(inew->ir2trans.y_reverse) {
		tmpf = inew->ir2trans.ut;
		inew->ir2trans.ut = inew->ir2trans.ub;
		inew->ir2trans.ub = tmpf;
	}
	inew->ir2trans.compc_x_min = MIN(inew->ir2trans.ul,inew->ir2trans.ur);
	inew->ir2trans.compc_x_max = MAX(inew->ir2trans.ul,inew->ir2trans.ur);
	inew->ir2trans.compc_y_min = MIN(inew->ir2trans.ut,inew->ir2trans.ub);
	inew->ir2trans.compc_y_max = MAX(inew->ir2trans.ut,inew->ir2trans.ub);
	return(ret);
}

/*
 * Function:	IrSetTrans
 *
 * Description: set_trans method for IrregularType2TransObjs. The current instance
 *		and the parent of the instance are needed. The parent 
 *		provides current screen position information (x,y,width,height)
 *		these are not set through resources because one transformation
 *		needs to possibly be shared by multiple plots.
 *
 * In Args:	instance    is the instance of the IrregularType2TransObj 
 *		parent	    is the parent of the transform
 *
 * Out Args:	NONE
 *
 * Return Values: Error Status
 *
 * Side Effects:  GKS state altered.
 */

static NhlErrorTypes IrSetTrans
#if __STDC__
(NhlLayer instance, NhlLayer parent) 
#else
(instance, parent)
NhlLayer   instance;
NhlLayer   parent;
#endif
{
	float x;
	float y;
	float width;
	float height;
	NhlIrregularType2TransObjLayer iinstance = (NhlIrregularType2TransObjLayer)instance;
	NhlErrorTypes ret;
	

	ret = NhlVAGetValues(parent->base.id,
		NhlNvpXF,&x,
		NhlNvpYF,&y,
		NhlNvpWidthF,&width,
		NhlNvpHeightF,&height,NULL);
	if(ret < NhlWARNING) {
		return(ret);
	}
	
		
		

	c_set(x,x+width,y-height,y,iinstance->ir2trans.ul,iinstance->ir2trans.ur,
		iinstance->ir2trans.ub,iinstance->ir2trans.ut,1);

	
	return(NhlNOERROR);
	
}


/*
 * Function:	IrWinToNDC
 *
 * Description: Computes the current forward tranformation of the points x and
 *		y to NDC based on the current viewport of the parent. It is
 *		important that this routine not depend on a static screen 
 *		orientation because the parents view may have been transformed.
 *
 * In Args:	instance is the IrregularType2TransObj and parent is the plot.
 *		(x,y) are the coordinates in data space.
 *		(xout,yout) are the coordinate in Normalized device coordinates.
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects:
 */

static NhlErrorTypes IrWinToNDC
#if  __STDC__
(NhlLayer instance,NhlLayer parent ,float *x,float *y,int n,float* xout,float* yout,float* xmissing,float* ymissing,int *status)
#else
(instance, parent,x,y,n,xout,yout,xmissing,ymissing,status)
	NhlLayer   instance;
	NhlLayer   parent;
	float   *x;
	float   *y;
	int	n;
	float*  xout;
	float*  yout;
	float*  xmissing;
	float*	ymissing;
	int * status;
#endif
{
	float x0;
	float y0;
	float width;
	float height;
	NhlIrregularType2TransObjLayer iinstance = (NhlIrregularType2TransObjLayer)instance;
	int i;
	NhlErrorTypes ret;
	

	*status = 0;	
	ret= NhlVAGetValues(parent->base.id,
		NhlNvpXF,&x0,
		NhlNvpYF,&y0,
		NhlNvpWidthF,&width,
		NhlNvpHeightF,&height,NULL);
	if(ret < NhlWARNING) {
		return(ret);
	}

	if((xmissing == NULL)&&(ymissing == NULL)) {
		for(i = 0; i< n ; i++) {
/*
* Compc and Window are identical coordinates in this object
*/
			if((x[i] > iinstance->ir2trans.compc_x_max)
				||(x[i] < iinstance->ir2trans.compc_x_min)
				||(y[i] > iinstance->ir2trans.compc_y_max)
				||(y[i] < iinstance->ir2trans.compc_y_min)) {
				*status = 1;
				xout[i]=yout[i] =iinstance->trobj.out_of_range;
			} else {
				strans(iinstance->ir2trans.ul,
					iinstance->ir2trans.ur,
					iinstance->ir2trans.ub,
					iinstance->ir2trans.ut,
				 	x0,x0+width,y0-height,y0,x[i],y[i],
					&(xout[i]),&(yout[i]));
			}
		}
	} else {
		for(i = 0; i< n ; i++) {
			if(((xmissing != NULL)&&(*xmissing == x[i]))
				||((ymissing != NULL)&&(*ymissing == y[i]))
				||(x[i] > iinstance->ir2trans.compc_x_max)
				||(x[i] < iinstance->ir2trans.compc_x_min)
				||(y[i] > iinstance->ir2trans.compc_y_max)
				||(y[i] < iinstance->ir2trans.compc_y_min)) {
				*status = 1;
				xout[i]=yout[i]=iinstance->trobj.out_of_range;
			} else {
				strans(iinstance->ir2trans.ul,
					iinstance->ir2trans.ur,
					iinstance->ir2trans.ub,	
					iinstance->ir2trans.ut,
					 x0,x0+width,y0-height,y0,x[i],y[i],
					&(xout[i]),&(yout[i]));
			}
		}
	}

	return(NhlNOERROR);
}


/*
 * Function:	IrNDCToWin
 *
 * Description:
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects:
 */
static NhlErrorTypes IrNDCToWin
#if  __STDC__
(NhlLayer instance,NhlLayer parent ,float *x,float *y,int n,float* xout,float* yout,float * xmissing, float *ymissing,int *status)
#else
(instance, parent,x,y,n,xout,yout,xmissing,ymissing,status)
	NhlLayer   instance;
	NhlLayer   parent;
	float   *x;
	float   *y;
	int	n;
	float*  xout;
	float*  yout;
	float*	xmissing;
	float*	ymissing;
	int* status;
#endif
{
	float x0, x1;
	float y0, y1;
	float width;
	int i;
	float height;
	NhlIrregularType2TransObjLayer iinstance = (NhlIrregularType2TransObjLayer)instance;
	NhlErrorTypes ret;
	
	
	*status = 0;
	ret = NhlVAGetValues(parent->base.id,
		NhlNvpXF,&x0,
		NhlNvpYF,&y0,
		NhlNvpWidthF,&width,
		NhlNvpHeightF,&height,NULL);
	if( ret < NhlWARNING)
		return(ret);
	x1 = x0 + width;
	y1 = y0 - height;
	if((xmissing == NULL)&&(ymissing == NULL)) {
		for(i = 0; i< n; i++) {
			if((x[i] > x1)
				||(x[i] < x0)
				||(y[i] > y0)
				||(y[i] < y1)) {
				
				*status = 1;
				xout[i] = yout[i]= iinstance->trobj.out_of_range;
			} else {
				strans(x0,x0+width,y0-height,y0,iinstance->ir2trans.ul,
					iinstance->ir2trans.ur, iinstance->ir2trans.ub,
					iinstance->ir2trans.ut, x[i],y[i],
					&(xout[i]),&(yout[i]));
			}
		}
	} else {
		for(i = 0; i< n; i++) {
			if(((xmissing != NULL)&&(*xmissing == x[i]))
				||((ymissing != NULL)&&(*ymissing == y[i]))
				||(x[i] > x1)
				||(x[i] < x0)
				||(y[i] > y0)
				||(y[i] < y1)) {
				*status = 1;
				xout[i]=yout[i]=iinstance->trobj.out_of_range;
			} else {
				strans(x0,x0+width,y0-height,y0,
					iinstance->ir2trans.ul,
					iinstance->ir2trans.ur, 
					iinstance->ir2trans.ub,
					iinstance->ir2trans.ut, x[i],y[i],
					&(xout[i]),&(yout[i]));
			}
		}
	}
	return(NhlNOERROR);
}


/*
 * Function:	IrDataToCompc
 *
 * Description: Transforms data coordinates into the computation coordinate
 *		system
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects:
 */
/*ARGSUSED*/
static NhlErrorTypes IrDataToCompc
#if  __STDC__
(NhlLayer instance,NhlLayer parent ,float *x,float *y,int n,float* xout,float* yout,float *xmissing,float *ymissing, int *status)
#else
(instance, parent,x,y,n,xout,yout,xmissing,ymissing,status)
        NhlLayer   instance;
        NhlLayer   parent;
        float   *x;
        float   *y;
        int     n;
        float*  xout;
        float*  yout;
	float*  xmissing;
	float*	ymissing;
	int * status;
#endif
{
	NhlErrorTypes ret = NhlNOERROR;
	NhlIrregularType2TransObjLayer iinstance = (NhlIrregularType2TransObjLayer)instance;
	int i;

	*status = 0;
	for(i=0; i< n;i++) {
		if(((xmissing != NULL)&&(*xmissing == x[i]))
			||((ymissing != NULL)&&(*ymissing == y[i]))
			||(x[i] < iinstance->ir2trans.x_min)	
			||(x[i] > iinstance->ir2trans.x_max)
			||(y[i] < iinstance->ir2trans.y_min)
			||(y[i] > iinstance->ir2trans.y_max)) {
		
			*status = 1;
			xout[i] = yout[i] = iinstance->trobj.out_of_range;	
		} else {
			ret = _NhlEvalSplineCoordForward(&(iinstance->ir2trans.thecoord),
				x[i],y[i],&(xout[i]),&(yout[i]),NULL,NULL);
		}
	}
	return(ret);
}


/*
 * Function:	IrCompcToData
 *
 * Description:	transforms computational coordinates into data coordinates.
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects:
 */
/*ARGSUSED*/
static NhlErrorTypes IrCompcToData
#if  __STDC__
(NhlLayer instance,NhlLayer parent ,float *x,float *y,int n,float* xout,float* yout,float *xmissing,float *ymissing,int* status)
#else
(instance, parent,x,y,n,xout,yout,xmissing,ymissing,status)
        NhlLayer   instance;
        NhlLayer   parent;
        float   *x;
        float   *y;
        int     n;
        float*  xout;
        float*  yout;
	float*	xmissing;
	float*	ymissing;
	int *   status;
#endif
{
	NhlErrorTypes ret = NhlNOERROR;
	NhlIrregularType2TransObjLayer iinstance = (NhlIrregularType2TransObjLayer)instance;
	int i;

	*status = 0;
	for(i = 0; i< n ; i++) {
		if(((xmissing != NULL)&&(*xmissing == x[i]))
			||((ymissing != NULL)&&(*ymissing == y[i]))
			||(x[i] > iinstance->ir2trans.compc_x_max)
			||(x[i] < iinstance->ir2trans.compc_x_min)
			||(y[i] > iinstance->ir2trans.compc_y_max)
			||(y[i] < iinstance->ir2trans.compc_y_min)) {
			*status = 1;
			xout[i] = yout[i] = iinstance->trobj.out_of_range;
		} else {
				ret = _NhlEvalSplineCoordInverse(&(iinstance->ir2trans.thecoord),
					x[i],y[i],&(xout[i]),&(yout[i]),NULL,NULL);	
		}
	}
	return(ret);
}


static NhlErrorTypes IrDataLineTo
#if __STDC__
(NhlLayer instance, NhlLayer parent,float x, float y, int upordown )
#else
(instance, parent,x, y, upordown )
NhlLayer instance;
NhlLayer parent;
float x;
float y;
int upordown;
#endif
{
	NhlIrregularType2TransObjLayer ir2inst = (NhlIrregularType2TransObjLayer)instance;
	static float lastx,lasty;
	static call_frstd = 1;
	float currentx,currenty;
	float xpoints[2];
	float ypoints[2];
	float holdx,holdy;
	int status;

/*
* if true the moveto is being performed
*/
	if(upordown) {
		lastx = x;
		lasty = y;
		call_frstd = 1;
		return(NhlNOERROR);
	} else {
		currentx = x;
		currenty = y;
		holdx = lastx;
		holdy = lasty;
		_NhlTransClipLine(ir2inst->ir2trans.x_min,
			ir2inst->ir2trans.x_max,
			ir2inst->ir2trans.y_min,
			ir2inst->ir2trans.y_max,
			&lastx,
			&lasty,
			&currentx,
			&currenty,
			ir2inst->trobj.out_of_range);
		if((lastx == ir2inst->trobj.out_of_range)
			||(lasty == ir2inst->trobj.out_of_range)
			||(currentx == ir2inst->trobj.out_of_range)
			||(currenty == ir2inst->trobj.out_of_range)){
/*
* Line has gone completely out of window
*/
			lastx = x;	
			lasty = y;
			call_frstd = 1;
			return(NhlNOERROR);
		} else {
			xpoints[0] = lastx;
			xpoints[1] = currentx;
			ypoints[0] = lasty;
			ypoints[1] = currenty;
/*
* Compc and window are identical in this object
*/
			IrDataToCompc(instance,parent,xpoints,ypoints,2,xpoints,ypoints,NULL,NULL,&status);
			if((lastx != holdx)||(lasty!= holdy)) {
				call_frstd = 1;
			}
			if(call_frstd == 1) {
				_NhlWorkstationLineTo(parent->base.wkptr,c_cufx(xpoints[0]),c_cufy(ypoints[0]),1);
				call_frstd = 2;
			}
			_NhlWorkstationLineTo(parent->base.wkptr,c_cufx(xpoints[1]),c_cufy(ypoints[1]),0);
			lastx = x;
			lasty = y;
			return(NhlNOERROR);
		}
	}
}

/*ARGSUSED*/
static NhlErrorTypes IrWinLineTo
#if __STDC__
(NhlLayer instance, NhlLayer parent, float x, float y, int upordown)
#else
(instance, parent, x, y, upordown)
NhlLayer instance;
NhlLayer parent;
float x;
float y;
int upordown;
#endif
{
	NhlIrregularType2TransObjLayer ir2inst = (NhlIrregularType2TransObjLayer)instance;
	static float lastx,lasty;
	static call_frstd = 1;
	float currentx,currenty;
	float holdx, holdy; /* * if true the moveto is being performed */

	if(upordown) {
		lastx = x;
		lasty = y;
		call_frstd = 1;
		return(NhlNOERROR);
	} else {
		currentx = x;
		currenty = y;
		holdx = lastx;
		holdy = lasty;
		_NhlTransClipLine(
/*
* Window and compc are identical for this object
*/
			ir2inst->ir2trans.compc_x_min, 
			ir2inst->ir2trans.compc_x_max, 
			ir2inst->ir2trans.compc_y_min, 
			ir2inst->ir2trans.compc_y_max,
			&lastx, &lasty, &currentx, &currenty,
			ir2inst->trobj.out_of_range);
		if((lastx == ir2inst->trobj.out_of_range)
			||(lasty == ir2inst->trobj.out_of_range)
			||(currentx == ir2inst->trobj.out_of_range)
			||(currenty == ir2inst->trobj.out_of_range)){
/*
* Line has gone completely out of window
*/
			lastx = x;	
			lasty = y;
			call_frstd = 1;
			return(_NhlWorkstationLineTo(parent->base.wkptr,c_cufx(x),c_cufy(y),1));
		} else {
                        if((lastx != holdx)||(lasty!= holdy)) {
                                call_frstd = 1;
                        }

			if(call_frstd == 1) {
				_NhlWorkstationLineTo(parent->base.wkptr,c_cufx(lastx),c_cufy(lasty),1);

				call_frstd = 2;
			}
			_NhlWorkstationLineTo(parent->base.wkptr,c_cufx(currentx),c_cufy(currenty),0);

			lastx = x;
			lasty = y;
			return(NhlNOERROR);
		}
			
			
	}
	
}


/*ARGSUSED*/
static NhlErrorTypes IrNDCLineTo
#if __STDC__
(NhlLayer instance, NhlLayer parent,float x, float y, int upordown )
#else
(instance, parent,x, y, upordown )
NhlLayer instance;
NhlLayer parent;
float x;
float y;
int upordown;
#endif
{
	NhlIrregularType2TransObjLayer iinstance= (NhlIrregularType2TransObjLayer)instance;
	static float lastx,lasty;
	static call_frstd = 1;
	float currentx,currenty;
	float xvp,yvp,widthvp,heightvp;
	float holdx,holdy;

/*
* if true the moveto is being performed
*/
	if(upordown) {
		lastx = x;
		lasty = y;
		call_frstd = 1;
		return(NhlNOERROR);
	} else {
		currentx = x;
		currenty = y;
		holdx = lastx;
		holdy = lasty;
                NhlVAGetValues(parent->base.id,
                        NhlNvpXF,&xvp,
                        NhlNvpYF,&yvp,
                        NhlNvpWidthF,&widthvp,
                        NhlNvpHeightF,&heightvp,NULL);

		_NhlTransClipLine(
			xvp, xvp+widthvp, yvp-heightvp, yvp,
			&lastx, &lasty, &currentx, &currenty,
			iinstance->trobj.out_of_range);
		if((lastx == iinstance->trobj.out_of_range)
			||(lasty == iinstance->trobj.out_of_range)
			||(currentx == iinstance->trobj.out_of_range)
			||(currenty == iinstance->trobj.out_of_range)){
/*
* Line has gone completely out of window
*/
			lastx = x;	
			lasty = y;
			call_frstd = 1;
			return(_NhlWorkstationLineTo(parent->base.wkptr,x,y,1));

		} else {
                        if((lastx != holdx)||(lasty!= holdy)) {
                                call_frstd = 1;
                        }

			if(call_frstd == 1) {
				call_frstd = 2;
				_NhlWorkstationLineTo(parent->base.wkptr,lastx,lasty,1);
			}
			_NhlWorkstationLineTo(parent->base.wkptr,currentx,currenty,0);

			lastx = x;
			lasty = y;
			return(NhlNOERROR);
		}
			
			
	}
	
}

/*ARGSUSED*/
static NhlErrorTypes IrWinToCompc
#if  __STDC__
(NhlLayer instance, NhlLayer parent,float* x,float* y,int n,float* xout,float* yout,float* xmissing,float* ymissing,int* status)
#else
(instance,parent,x,y,n,xout,yout,xmissing,ymissing,status)
NhlLayer instance;
NhlLayer parent;
float* x;
float* y;
int n;
float* xout;
float* yout;
float* xmissing;
float* ymissing;
int* status;
#endif
{
        NhlErrorTypes ret = NhlNOERROR;
        NhlIrregularType2TransObjLayer iinstance = (NhlIrregularType2TransObjLayer)instance;
        int i;

        *status = 0;
        for(i = 0 ; i< n; i++) {
                if((xmissing != NULL)&&(*xmissing == x[i])
                        || (ymissing != NULL)&&(*ymissing == y[i])
                        ||(x[i] < iinstance->ir2trans.compc_x_min)
                        ||(x[i] > iinstance->ir2trans.compc_x_max)
                        ||(y[i] < iinstance->ir2trans.compc_y_min)
                        ||(y[i] > iinstance->ir2trans.compc_y_max)) {

                        yout[i]=xout[i]=iinstance->trobj.out_of_range;

                        *status = 1;
                } else {
                        yout[i] = y[i];
                        xout[i] = x[i];
                }
        }
        return(ret);
}
