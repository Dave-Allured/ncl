/*
 *      $Id: NcgmWorkstation.c,v 1.5 1994-05-05 18:17:01 ethan Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1992			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		NcgmWorkstation.c
 *
 *	Author:		Ethan Alpert
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Tue Sep 15 10:00:09 MDT 1992
 *
 *	Description:	Responsible for managing the NCGM workstation element
 */

#include <stdio.h>
#include <strings.h>
#include <ncarg/hlu/hluP.h>
#include <ncarg/hlu/NcgmWorkstationP.h>

#define DEFAULT_META_NAME "gmeta"

static NhlResource resources[] = {
	{ NhlNwkMetaName, NhlCwkMetaName, NhlTString, sizeof(char*),
	NhlOffset(NhlNcgmWorkstationLayerRec,ncgm.meta_name),NhlTString,DEFAULT_META_NAME,0,(NhlFreeFunc)NhlFree }
};

/*
* NcgmWorkstation base_class method declarations
*/

static NhlErrorTypes NcgmWorkstationInitialize(
#ifdef NhlNeedProto
        NhlLayerClass,     /* class */
        NhlLayer,          /* req */
        NhlLayer,          /* new */
        _NhlArgList,        /* args */
        int             /* num_args */
#endif
);


static NhlErrorTypes NcgmWorkstationClassPartInitialize(
#ifdef NhlNeedProto
        NhlLayerClass      /* lc */
#endif
);

static NhlErrorTypes NcgmWorkstationDestroy(
#ifdef NhlNeedProto
        NhlLayer           /* inst */
#endif
);

static NhlErrorTypes NcgmWorkstationSetValues(
#ifdef NhlNeedProto
        NhlLayer,		/* old */
        NhlLayer,		/* reference */
        NhlLayer,		/* new */
        _NhlArgList,	/* args */
        int		/* num_args*/
#endif
);

/*
* NcgmWorkstation work_class method declarations
*/

static NhlErrorTypes NcgmWorkstationOpen(
#ifdef NhlNeedProto
	NhlLayer /* instance */
#endif
);

/*
* A pointer to this is assigned to the cgm_inited field of this
* class. this pointer is then propagated down the subclass heirarchy
* with the class part initialize function.
*/
static _NhlNcgmStatus ncgm_is_initialized = _NhlUNINITED;
static int default_conid = NCGM_DEFAULT_CONID;

NhlNcgmWorkstationLayerClassRec NhlncgmWorkstationLayerClassRec = {
        {
/* class_name			*/	"NcgmWorkstation",
/* nrm_class			*/	NrmNULLQUARK,
/* layer_size			*/	sizeof(NhlNcgmWorkstationLayerRec),
/* class_inited			*/	False,
/* superclass			*/	(NhlLayerClass)&NhlworkstationLayerClassRec,

/* layer_resources		*/	resources,
/* num_resources		*/	NhlNumber(resources),
/* all_resources		*/	NULL,

/* class_part_initialize	*/	NcgmWorkstationClassPartInitialize,
/* class_initialize		*/	NULL,
/* layer_initialize		*/	NcgmWorkstationInitialize,
/* layer_set_values		*/	NcgmWorkstationSetValues,
/* layer_set_values_hook	*/	NULL,
/* layer_get_values		*/	NULL,
/* layer_reparent		*/	NULL,
/* layer_destroy		*/	NcgmWorkstationDestroy,

/* child_resources		*/	NULL,

/* layer_draw			*/	NULL,

/* layer_pre_draw		*/	NULL,
/* layer_draw_segonly		*/	NULL,
/* layer_post_draw		*/	NULL,
/* layer_clear			*/	NULL
        },
        {
/* open_work		*/	NcgmWorkstationOpen,
/* close_work		*/	NULL,
/* activate_work	*/	NULL,
/* deactivate_work	*/	NULL,
/* update_work		*/	NhlInheritUpdate,
/* clear_work		*/	NhlInheritClear,
/* lineto_work		*/	NULL,
/* fill_work		*/	NhlInheritFill,
/* marker_work		*/	NhlInheritMarker
	},
	{
/* cgm_inited	*/	&ncgm_is_initialized
	}
};

NhlLayerClass NhlncgmWorkstationLayerClass = (NhlLayerClass)&NhlncgmWorkstationLayerClassRec;



/*
 * Function:	NcgmWorkstationInitialize
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
/*ARGSUSED*/
static NhlErrorTypes NcgmWorkstationInitialize
#if __STDC__
(NhlLayerClass class, NhlLayer req, NhlLayer new, _NhlArgList args, int num_args)
#else
(class,req,new,args,num_args)
        NhlLayerClass class;
        NhlLayer req;
        NhlLayer new;
        _NhlArgList args;
        int num_args; 
#endif
{
	NhlNcgmWorkstationLayerClass wclass = (NhlNcgmWorkstationLayerClass)class;
	NhlNcgmWorkstationLayer	wnew = (NhlNcgmWorkstationLayer) new;
	NhlNcgmWorkstationLayer	wreq = (NhlNcgmWorkstationLayer) req;



	/* SUPPRESS 112 */
	if(strcmp(wreq->ncgm.meta_name,DEFAULT_META_NAME) != 0) {
		/* SUPPRESS 112 */
		wnew->ncgm.meta_name =
			(char*)NhlMalloc(strlen(wreq->ncgm.meta_name) + 1);
		/* SUPPRESS 112 */
		strcpy(wnew->ncgm.meta_name,wreq->ncgm.meta_name);
	}
	if(*(wclass->ncgm_class.cgm_inited) == _NhlUNINITED){
		wnew->work.gkswkstype = NCGM_WORKSTATION_TYPE;
		wnew->work.gkswksconid = default_conid++;
		*(wclass->ncgm_class.cgm_inited) = _NhlINITED;
		return(NhlNOERROR);
	} else if( *(wclass->ncgm_class.cgm_inited) == _NhlINITED) {
		NhlPError(NhlFATAL,NhlEUNKNOWN,"NcgmWorkstationInitialize: Only one NCGM workstation is allowed by NCAR Graphics");
		return(NhlFATAL);
	} 
}


/*
 * Function:	NcgmWorkstationClassPartInitialize
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

static NhlErrorTypes NcgmWorkstationClassPartInitialize
#if	__STDC__
(NhlLayerClass lc)
#else
(lc)
	NhlLayerClass lc;
#endif
{
	NhlNcgmWorkstationLayerClass wlc = (NhlNcgmWorkstationLayerClass)lc;
	NhlLayerClass	sc = wlc->base_class.superclass;

	if(sc != (NhlLayerClass)&NhlworkstationLayerClassRec) {
		wlc->ncgm_class.cgm_inited = 
			((NhlNcgmWorkstationLayerClass)sc)->ncgm_class.cgm_inited;
	}
	return(NhlNOERROR);
}

/*
 * Function:    NcgmWorkstationDestroy
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
static NhlErrorTypes NcgmWorkstationDestroy
#if     __STDC__
(NhlLayer inst)
#else
(inst)
        NhlLayer inst;
#endif
{
	NhlNcgmWorkstationLayer winst = (NhlNcgmWorkstationLayer)inst;
	NhlNcgmWorkstationLayerClass wclass = (NhlNcgmWorkstationLayerClass)inst->base.layer_class;

	if(strcmp(winst->ncgm.meta_name,DEFAULT_META_NAME) != 0)
		NhlFree(winst->ncgm.meta_name);
	*(wclass->ncgm_class.cgm_inited) = _NhlUNINITED;
	return(NhlNOERROR);
}


/*
 * Function:	NcgmWorkstationSetValues
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
/*ARGSUSED*/
static NhlErrorTypes NcgmWorkstationSetValues
#if __STDC__
(NhlLayer old, NhlLayer reference, NhlLayer new, _NhlArgList args, int num_args)
#else
(old,reference,new,args,num_args)
        NhlLayer old;
        NhlLayer reference;
        NhlLayer new; 
        _NhlArgList args;
        int num_args;
#endif
{
	NhlNcgmWorkstationLayer wold = (NhlNcgmWorkstationLayer) old;
	NhlNcgmWorkstationLayer wnew = (NhlNcgmWorkstationLayer) new;

	if(wnew->ncgm.meta_name != wold->ncgm.meta_name ) {
		NhlPError(NhlWARNING,NhlEUNKNOWN,"NcgmWorkstationSetValues: metafile name cannot change after initialization");
		return(NhlWARNING);
	}
	return(NhlNOERROR);
}

/*
 * Function:	NcgmWorkstationOpen
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
static NhlErrorTypes NcgmWorkstationOpen
#if __STDC__
(NhlLayer instance)
#else
(instance)
	NhlLayer instance;
#endif
{
	Gescape_in_data indat;
	Gescape_out_data *outdat;
	NhlNcgmWorkstationLayer winstance = (NhlNcgmWorkstationLayer) instance;

	indat.escape_r1.size = strlen(winstance->ncgm.meta_name) + 1;
	indat.escape_r1.data = (void*)winstance->ncgm.meta_name;
	
	gescape(-1391,&indat,NULL,&outdat);

	return(NhlNOERROR);
}
