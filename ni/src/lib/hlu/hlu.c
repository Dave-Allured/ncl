/*
 *      $Id: hlu.c,v 1.13 1994-05-05 18:17:51 ethan Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1992			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		hlu.c
 *
 *	Author:		Jeff W. Boote
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Fri Aug 28 09:05:49 MDT 1992
 *
 *	Description:	This file contains simple utility functions that
 *			are needed by layer programmers as well as app
 *			writers.
 */

#include <ncarg/hlu/hluP.h>
#include <ncarg/hlu/FortranP.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <ncarg/c.h>
#include <ncarg/hlu/VarArg.h>
#include <ncarg/hlu/BaseP.h>

/************************************************************************
 *									*
 * These functions are used for memory management			*
 *									*
 ************************************************************************/

/*
 * Function:	NhlMalloc
 *
 * Description:	This function is our interface to the regular malloc
 *		system call.  We are using it so we can do error handleing
 *		for memory allocation in one place and so we can impliment
 *		our own memory management code if we need to.
 *
 * In Args:	unsigned int	size	size of memory requested
 *
 * Out Args:	
 *
 * Scope:	Global Public
 * Returns:	pointer to memory of the size requested
 * Side Effect:	
 */
void
*NhlMalloc
#if	NhlNeedProto
(
	unsigned int	size	/* size of memory requested	*/
)
#else
(size)
	unsigned int	size;	/* size of memory requested	*/
#endif
{
	void *ptr;

	if(size == 0)
		return NULL;

	ptr = (void *)malloc(size);

	if(ptr == NULL)
		NhlPError(NhlFATAL,errno,"NhlMalloc Failed");

	return(ptr);
}

/*
 * Function:	NhlCalloc
 *
 * Description:	This function is our interface to the regular calloc
 *		system call.  We are using it so we can do error handleing
 *		for memory allocation in one place and so we can impliment
 *		our own memory management code if we need to.
 *
 * In Args:	unsigned int	num	number of elements
 *		unsigned int	size	size of each element
 *
 * Out Args:	
 *
 * Scope:	Global Public
 * Returns:	pointer to memory of the size requested
 * Side Effect:	
 */
void
*NhlCalloc
#if	NhlNeedProto
(
	unsigned int	num,	/* number of elements		*/
	unsigned int	size	/* size of each element		*/
)
#else
(num,size)
	 unsigned int	num;	/* number of elements		*/
	unsigned int	size;	/* size of each element		*/
#endif
{
	void *ptr;

	if((num * size) == 0)
		return NULL;

	ptr = (void *)calloc(num, size);

	if(ptr == NULL)
		NhlPError(NhlFATAL,errno,"NhlCalloc Failed");

	return(ptr);
}

/*
 * Function:	NhlRealloc
 *
 * Description:	This function is our interface to the regular realloc
 *		system call.  We are using it so we can do error handleing
 *		for memory allocation in one place and so we can impliment
 *		our own memory management code if we need to.
 *
 * In Args:	void		*ptr	pointer to old memory
 *		unsigned int	size	size of memory requested
 *
 * Out Args:	
 *
 * Scope:	Global Public
 * Returns:	pointer to memory of the size requested
 * Side Effect:	
 */
void
*NhlRealloc
#if	NhlNeedProto
(
	void		*ptr,	/* pointer to old memory	*/
	unsigned int	size	/* size of memory requested	*/
)
#else
(ptr,size)
	void		*ptr;	/* pointer to old memory	*/
	unsigned int	size;	/* size of memory requested	*/
#endif
{
	void *tptr;

	if(ptr == NULL)
		return NhlMalloc(size);
	else{
		tptr = (void *)realloc(ptr,size);

		if(tptr == NULL)
			NhlPError(NhlFATAL,errno,"NhlRealloc Failed");

		return(tptr);
	}
}

/*
 * Function:	NhlFree
 *
 * Description:	This function is our interface to the regular free
 *		system call.  We are using it so we can do error handleing
 *		for memory allocation in one place and so we can impliment
 *		our own memory management code if we need to.
 *
 * In Args:	void		*ptr	pointer to memory to free
 *
 * Out Args:	
 *
 * Scope:	Global Public
 * Returns:	NhlErrorTypes
 * Side Effect:	
 */
NhlErrorTypes
NhlFree
#if	NhlNeedProto
(
	void		*ptr	/* pointer to memory to free	*/
)
#else
(ptr)
	void		*ptr;	/* pointer to memory to free	*/
#endif
{

	if(ptr == NULL)
		return(NhlNOERROR);

	else{
#if	defined(__sgi) || defined(_HPUX_SOURCE) || defined(__CLCC__) \
	|| defined(GCC)

		free(ptr);
		return NhlNOERROR;
#else
		register int ret;

		ret = free(ptr);

		if(ret == 0){
			NhlPError(NhlWARNING,errno,"Error in NhlFree");
			return(NhlWARNING);
		}
		else{
			return(NhlNOERROR);
		}
#endif
	}
}

/************************************************************************
 *									*
 * These functions manage the global NhlLayer Table			*
 *									*
 ************************************************************************/

static NhlLayer *LayerTable = NULL;
static int num_layers = 0;
static int table_len = 0;

/*
 * Function:	_NhlGetLayer
 *
 * Description:	This function is used to retrieve a layer of the given id
 *		from the internal layer table.
 *
 * In Args:	int	id	The id of the layer requested
 *
 * Out Args:	
 *
 * Scope:	Global Private
 * Returns:	NhlLayer - the layer with the given id or NULL
 * Side Effect:	
 */
NhlLayer
_NhlGetLayer
#if	NhlNeedProto
(
	int	id	/* The layer id of the requested layer	*/
)
#else
(id)
	int	id;	/* The layer id of the requested layer	*/
#endif
{
	register int index = id - 1;

	if((index >= table_len) || (index < 0))
		return((NhlLayer)NULL);

	return(LayerTable[index]);
}

/*
 * Function:	_NhlAddLayer
 *
 * Description:	This function is used to add a layer into the global NhlLayer
 *		Table.  It first determines if there is enough space allocated
 *		and then enters the given layer into the Table. It also
 *		updates the layer->base.pid to the correct entry in the table.
 *
 * In Args:	NhlLayer	l	The layer to enter into the table
 *
 * Out Args:	
 *
 * Scope:	Global Private
 * Returns:	pid or NhlErrorTypes pid > 0 and Error if < 0 - Never return 0
 * Side Effect:	_NhlLayerTable can change - to grow.
 */
int
_NhlAddLayer
#if	NhlNeedProto
(
	NhlLayer	l	/* The layer to enter into the table	*/
)
#else
(l)
	NhlLayer	l;	/* The layer to enter into the table	*/
#endif
{
	register int i;

	/*
	 * Increase size of table if needed
	 */
	if(table_len < num_layers + 1){
		LayerTable = NhlRealloc(LayerTable,
		(unsigned)((table_len + _NhlLAYERLISTINC) * sizeof(NhlLayer)));
		if(LayerTable == NULL){
			NhlPError(NhlFATAL,12,
				"Unable to Increase size of NhlLayer Table");
			return(NhlFATAL);
		}

		for(i=table_len; i < (table_len + _NhlLAYERLISTINC); i++)
			LayerTable[i] = (NhlLayer)NULL;
		table_len += _NhlLAYERLISTINC;
	}

	for(i=0; i < table_len; i++){
		if(LayerTable[i] == (NhlLayer)NULL){
			LayerTable[i] = l;
			num_layers++;
			l->base.id = i+1;
			return(i+1);
		}
	}

	/*
	 * Error message!
	 */
	return(NhlFATAL);
}

/*
 * Function:	_NhlRemoveLayer
 *
 * Description:	This function is used to remove a layer from the global NhlLayer
 *		Table.  
 *
 * In Args:	NhlLayer	l	The layer to remove from the table
 *
 * Out Args:	
 *
 * Scope:	Global Private
 * Returns:	NhlErrorTypes
 * Side Effect:	
 */
NhlErrorTypes
_NhlRemoveLayer
#if	NhlNeedProto
(
	NhlLayer	l	/* The layer to remove from the table	*/
)
#else
(l)
	NhlLayer	l;	/* The layer to remove from the table	*/
#endif
{
	register int index;

	if(l == (NhlLayer)NULL){
		/*
		 * ERROR - request to remove a null layer
		 */
		NhlPError(NhlWARNING,NhlEUNKNOWN,
					"_NhlRemoveLayer:Can't rm NULL layer");
		return(NhlWARNING);
	}

	index = l->base.id-1;
	if(LayerTable[index] != l){
		/*
		 * ERROR - layers id doesn't match internal table
		 */
		NhlPError(NhlWARNING,NhlEUNKNOWN,
			"_NhlRemoveLayer:layer id doesn't match layer Table");
		return(NhlWARNING);
	}

	LayerTable[index] = (NhlLayer)NULL;
	num_layers--;

	return(NhlNOERROR);
}

/*
 * Function:	DestroyLayerTree
 *
 * Description:	This function takes a layer id, and traverses to the top
 *		level parent of that object and destroys it.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	static
 * Returns:	void
 * Side Effect:	
 */
static void
DestroyLayerTree
#if	NhlNeedProto
(
	int	id
)
#else
(id)
	int	id;
#endif
{
	NhlLayer	l = _NhlGetLayer(id);

	if(l == NULL)
		return;

	if(l->base.parent != NULL){
		DestroyLayerTree(l->base.parent->base.id);
		return;
	}

	(void)NhlDestroy(id);

	return;
}

/*
 * Function:	_NhlDestroyLayerTable
 *
 * Description:	This function is used to clean the LayerTable for close.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	Global Private
 * Returns:	void
 * Side Effect:	
 */
void
_NhlDestroyLayerTable
#if	NhlNeedProto
(
	void
)
#else
()
#endif
{
	int i;

	for(i=0;i < table_len && num_layers > 0;i++){
		if(LayerTable[i] != NULL)
			DestroyLayerTree(i+1);
		if(LayerTable[i] != NULL)
			NhlPError(NhlWARNING,NhlEUNKNOWN,
					"Unable to destroy layer %d ???",i+1);
	}

	if (num_layers > 0)
		NhlPError(NhlWARNING,NhlEUNKNOWN,"Not all Layers destroyed?");

	table_len = 0;
	num_layers = 0;
	(void)NhlFree(LayerTable);
	LayerTable = NULL;

	return;
}

/************************************************************************
*									*
*	The rest of these functions are misc. utilty functions that	*
*	were needed.							*
*									*
************************************************************************/

/*
 * Function:	NhlName
 *
 * Description:	This function returns the name of a given plot specified
 *		by pid.
 *
 * In Args:	int	pid;	id of a plot
 *
 * Out Args:	
 *
 * Scope:	global public
 * Returns:	Const char *
 * Side Effect:	
 */
Const char *
NhlName
#if	NhlNeedProto
(
	int	pid	/* id of a plot	*/
)
#else
(pid)
	int	pid;	/* id of a plot	*/
#endif
{
	NhlLayer	tmp = _NhlGetLayer(pid);

	if(tmp == NULL){
		NhlPError(NhlFATAL,NhlEUNKNOWN,
				"Unable to access plot with pid:%d",pid);
		return NULL;
	}

	return tmp->base.name;
}

/*
 * Function:	nhl_fname
 *
 * Description:	Fortran callable function to return the name of an object.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	global
 * Returns:	fills the "name" parameter with the name or " "'s
 * Side Effect:	
 */
/*ARGSUSED*/
void
_NHLCALLF(nhl_fname,NHL_FNAME)
#if	__STDC__
(
	int		*id,
	_NhlFString	name,
	int		*name_len,
	int		*err
)
#else
(id,name,name_len,err)
	int		*id;
	_NhlFString	name;
	int		*name_len;
	int		*err;
#endif
{
	Const char	*cstr;

	cstr = NhlName(*id);

	*err = _NhlCstrToFstr(name,*name_len,(NhlString)cstr);

	return;
}

/*
 * Function:	NhlClassName
 *
 * Description: Returns class name of object
 *
 * In Args:	pid	integer id of object whose class is requested
 *
 * Out Args:	NONE
 *
 * Return Values: returns result of _NhlClassName call	
 *
 * Side Effects: NONE
 */
Const char *
NhlClassName
#if	NhlNeedProto
(
	int pid
)
#else
(pid)
int pid;
#endif
{
	NhlLayer	l = _NhlGetLayer(pid);

	return(_NhlClassName(_NhlClass(l)));
}

/*
 * Function:	nhl_fclassname
 *
 * Description:	Fortran callable function to return the classname of an object.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	global
 * Returns:	fills the "name" parameter with the name or " "'s
 * Side Effect:	
 */
void
_NHLCALLF(nhl_fclassname,NHL_FCLASSNAME)
#if	__STDC__
(
	int		*id,
	_NhlFString	name,
	int		*name_len,
	int		*err
)
#else
(id,name,name_len,err)
	int		*id;
	_NhlFString	name;
	int		*name_len;
	int		*err;
#endif
{
	Const char	*cstr;

	cstr = NhlClassName(*id);

	*err = _NhlCstrToFstr(name,*name_len,(NhlString)cstr);

	return;
}

/*
 * Function:	_NhlClassName
 *
 * Description:	This function returns the name of a given class
 *
 * In Args:	NhlLayerClass	lc;	pointer to class struct
 *
 * Out Args:	
 *
 * Scope:	global public
 * Returns:	Const char *
 * Side Effect:	
 */
Const char *
_NhlClassName
#if	NhlNeedProto
(
	NhlLayerClass	lc	/* pointer to class struct	*/
)
#else
(lc)
	NhlLayerClass	lc;	/* pointer to class struct	*/
#endif
{
	return lc->base_class.class_name;
}

/*
 * Function:	_NhlClass
 *
 * Description:	This function returns the given NhlLayer's class pointer.
 *
 * In Args:	NhlLayer l;	layer
 *
 * Out Args:	
 *
 * Scope:	Global, Public
 * Returns:	NhlLayerClass
 * Side Effect:	
 */
NhlLayerClass
_NhlClass
#if	NhlNeedProto
(
	NhlLayer l		/* Instance pointer */
)
#else
(l)
	NhlLayer l;
#endif
{
	return(l->base.layer_class);
}

/*
 * Function:	_NhlIsFloatRes
 *
 * Description:	This function takes a resource name and determines if it
 *		is a float resource by checking if the last char is a 'F'
 *
 * In Args:	res_name	resource name
 *
 * Out Args:	
 *
 * Scope:	Global Private
 * Returns:	NhlBoolean - True if res is a float res
 * Side Effect:	
 */
NhlBoolean
_NhlIsFloatRes
#if	NhlNeedProto
(
	NhlString	res_name	/* resource name	*/
)
#else
(res_name)
	NhlString	res_name;	/* resource name	*/
#endif
{
	char	*index = res_name;

	while(*index != '\0') index++;

	if(*(index - 1) == 'F')
		return(True);
	else
		return(False);
}

/*
 * Function:	_NhlResolvePath
 *
 * Description:	This function takes a pathname and returns a pathname with
 *		all "~"'s and environment "$var"'s resolved.
 *
 * In Args:	char 	*rawfname	fname as provided
 *
 * Out Args:	
 *
 * Scope:	Global Private
 * Returns:	char *
 * Side Effect:	
 */
Const char
*_NhlResolvePath
#if	NhlNeedProto
(
	Const char	*rawfname	/* fname as provided	*/
)
#else
(rawfname)
	Const char	*rawfname;	/* fname as provided	*/
#endif
{
	static char	fname[_NhlMAXFNAMELEN];
	char		tmpfname[_NhlMAXFNAMELEN];
	char		buffer[_NhlMAXFNAMELEN];
	char		*piece = NULL;
	char		*tptr;
	struct passwd	*pw = NULL;
	NhlBoolean	first = True;

	if(rawfname == NULL){
		return(NULL);
	}

	fname[0] = '\0';

	strcpy(tmpfname,rawfname);
	strcpy(buffer,_NhlPATHDELIMITER);
	if(tmpfname[0] == buffer[0])
		strcpy(fname,_NhlPATHDELIMITER);
	piece = strtok(tmpfname,_NhlPATHDELIMITER);

	while(piece != NULL){

		if(first)
			first = False;
		else
			strcat(fname,_NhlPATHDELIMITER);

		switch(*piece){

			case '~':

				if(*(piece+1) != '\0'){	/* different username */

					strcpy(buffer,(piece + 1));
					pw = getpwnam(buffer);
				}
				else			/* this username      */
					pw = getpwuid(getuid());

				if(pw == NULL){
					NhlPError(NhlFATAL,NhlEUNKNOWN,
						"Unable to Resolve \'~\' in %s",
								rawfname);
					return(NULL);
				}
				strcat(fname,pw->pw_dir);

				break;

			case '$':

				strcpy(buffer,(piece + 1));
				tptr = getenv(buffer);

				if(tptr == NULL){
					NhlPError(NhlFATAL,NhlEUNKNOWN,
						"Unable to Resolve %s in %s",
								piece,rawfname);
					return(NULL);
				}
				strcat(fname,tptr);

				break;

			default:

				strcat(fname,piece);
		}

		piece = strtok(NULL,_NhlPATHDELIMITER);
	}

	return(fname);
}

/*
 * Function:	_NhlGetSysResFile
 *
 * Description:	This function returns the name of the system resource file.
 *		It saves the name in static data so the next time it is
 *		requested it doesn't need to look in the system again.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	Global Private
 * Returns:	const char *
 * Side Effect:	
 */
Const char
*_NhlGetSysResFile
#if	NhlNeedProto
(
	void	/* No args	*/
)
#else
()
#endif
{
	static char		fname[_NhlMAXFNAMELEN];
	const char		*tmp = NULL;
	static NhlBoolean	init = False;

	if(!init){

		tmp = getenv(NhlSYSRESENVNAME);
		if(tmp == NULL){
			tmp = GetNCARGPath("lib");

			if(tmp == NULL)
				return NULL;

			strcpy(fname,tmp);
			strcat(fname,"/ncarg/");
			strcat(fname,NhlDEFSYSRESFNAME);
		}
		else
			strcpy(fname,_NhlResolvePath(tmp));

		init = True;
	}

	return(fname);
}

/*
 * Function:	_NhlGetUsrResFile
 *
 * Description:	This function returns the name of the user resource file.
 *		It saves the name in static data so the next time it is
 *		requested it doesn't need to look in the system again.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	Global Private
 * Returns:	const char *
 * Side Effect:	
 */
Const char 
*_NhlGetUsrResFile
#if	NhlNeedProto
(
	void	/* No args	*/
)
#else
()
#endif
{
	static char		fname[_NhlMAXFNAMELEN];
	char			*tmp = NULL;
	static NhlBoolean	init = False;

	if(!init){

		tmp = getenv(NhlUSRRESENVNAME);
		if(tmp == NULL)
			tmp = NhlDEFUSRRESFNAME;

		(void)strcpy(fname,_NhlResolvePath(tmp));
		init = True;
	}

	return(fname);
}

/*
 * Function:	NhlSetSArg
 *
 * Description:	This function sets an Arg in an arglist so the arg list
 *		can be used in a NhlALCreate, NhlALSetValues.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	global public
 * Returns:	void
 * Side Effect:	
 */
NhlDOCTAG(NhlSetSArg)
/*VARARGS2*/
void
NhlSetSArg
#if	NeedVarArgProto
(
	NhlSArg		*arg,		/* arg to set		*/
	NhlString	resname,	/* resource to set	*/
	...				/* value to set arg to	*/
)
#else
(arg,resname,va_alist)
	NhlSArg		*arg;		/* arg to set		*/
	NhlString	resname;	/* resource to set	*/
	va_dcl				/* value to set arg to	*/
#endif
{
	va_list		ap;
	double		tmp;

	arg->name = resname;

	VA_START(ap,resname);
	if(_NhlIsFloatRes(resname)){
		tmp = va_arg(ap,double);
		arg->value.fltval = (float)tmp;
	}
	else
		arg->value.lngval = va_arg(ap,long);

	va_end(ap);
}

/*
 * Function:	_NhlSArgToSetArgList
 *
 * Description:	This function is used to allocate a _NhlArgList given an
 *		NhlSArgList.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	global private
 * Returns:	NhlErrorTypes
 * Side Effect:	
 */
void
_NhlSArgToSetArgList
#if	NhlNeedProto
(
	_NhlArgList	args,	/* arglist return	*/
	NhlSArgList	sargs,	/* public arglist	*/
	int		nargs	/* num args		*/
)
#else
(args,sargs,nargs)
	_NhlArgList	args;	/* arglist return	*/
	NhlSArgList	sargs;	/* public arglist	*/
	int		nargs;	/* num args		*/
#endif
{
	register int	i;

	for(i=0;i < nargs; i++){
		args[i].quark = NrmStringToQuark(sargs[i].name);
		args[i].value = sargs[i].value;
		args[i].type = NrmNULLQUARK;
	}

	return;
}
/*
 * Function:	NhlSetGArg
 *
 * Description:	This function sets an Arg in an arglist so the arg list
 *		can be used in NhlALGetValues.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	global public
 * Returns:	void
 * Side Effect:	
 */
/*VARARGS2*/
void
NhlSetGArg
#if	NeedVarArgProto
(
	NhlGArg		*arg,		/* arg to set		*/
	NhlString	resname,	/* resource to set	*/
	...				/* value to set arg to	*/
)
#else
(arg,resname,va_alist)
	NhlGArg		*arg;		/* arg to set		*/
	NhlString	resname;	/* resource to set	*/
	va_dcl				/* value to set arg to	*/
#endif
{
	va_list		ap;

	arg->resname = resname;

	VA_START(ap,resname);
	arg->value.ptrval = va_arg(ap,NhlPointer);
	va_end(ap);
}

/*
 * Function:	_NhlInherit
 *
 * Description:	This function is used to provide a pointer to check against
 *		if a function is supposed to be inherited from it's
 *		superclass.  This function should never be called so
 *		it prints out an error message if it is.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	Private Global
 * Returns:	void
 * Side Effect:	
 */
void
_NhlInherit
#if	NhlNeedProto
(
	void
)
#else
()
#endif
{
	NhlPError(NhlFATAL,NhlEUNKNOWN,
				"_NhlInherit- Inheritance resolved improperly");
	return;
}

/*
 * Function:	_NhlArgIsSet
 *
 * Description:	returns true if the given resource name is set in the given
 *		arg list.  otherwise returns false.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	Global Private
 * Returns:	NhlBoolean
 * Side Effect:	
 */
NhlBoolean _NhlArgIsSet
#if     NhlNeedProto
(
        _NhlArgList args,
        int     num_args,
        char    *resource_name
)
#else
(args,num_args,resource_name)
        _NhlArgList args;
        int     num_args;
        char    *resource_name;
#endif
{
	int quark = NrmStringToQuark(resource_name),i;
	_NhlArgList step = args;

	for(i = 0; i<num_args; i++) {
		if(step[i].quark == quark) 
			return(True);
	}
	return(False);
}

/*
 * Function:	_NhlCreateGenArray
 *
 * Description:	This function is used to define the size/shape of arrays passed
 *		in as resources. If num_dimensions is -1111 then pass back an
 *		empty NhlGenArray - Data=NULL,num_dimensions=num_elements=0
 *		and len_dimensions = NULL.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	global public
 * Returns:	NhlGenArray
 * Side Effect:	
 */
NhlGenArray
_NhlCreateGenArray
#if	NhlNeedProto
(
	NhlPointer	data,		/* data array		*/
	NhlString	type,		/* type of each element	*/
	unsigned int	size,		/* size of each element	*/
	int		num_dimensions,	/* number of dimensions	*/
	int		*len_dimensions,/* number of dimensions	*/
	NhlBoolean	copy_data	/* copy data pointer?	*/
)
#else
(data,type,size,num_dimensions,len_dimensions,copy_data)
	NhlPointer	data;			/* data array		*/
	NhlString	type;			/* type of each element	*/
	unsigned int	size;			/* size of each element	*/
	int		num_dimensions;		/* number of dimensions	*/
	int		*len_dimensions;	/* number of dimensions	*/
	NhlBoolean	copy_data;		/* copy data pointer?	*/
#endif
{
	static NrmQuark		QString = NrmNULLQUARK;
	NhlGenArray		gen = NULL;
	int			i;

	if(QString == NrmNULLQUARK)
		QString = NrmStringToQuark(NhlTString);

	if((num_dimensions < 1) && (num_dimensions != -1111)){
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,
		"NhlGenArrayCreate:Arrays must have at least one dimension"));
		return NULL;
	}

	gen = NhlMalloc(sizeof(NhlGenArrayRec));

	if(gen == NULL)
		return NULL;

	gen->typeQ = NrmStringToQuark(type);
	gen->size = size;

	if(num_dimensions == -1111){
		gen->num_dimensions = 0;
		gen->len_dimensions = NULL;
		gen->num_elements = 0;
		gen->data = NULL;
		gen->my_data = False;
	}
	else{
		gen->num_dimensions = num_dimensions;
		if(gen->num_dimensions == 1){
			gen->num_elements = *len_dimensions;
			gen->len_dimensions = &gen->num_elements;
		}
		else{
			gen->len_dimensions =
					NhlMalloc(num_dimensions * sizeof(int));
			if(gen->len_dimensions == NULL)
				return NULL;
			gen->num_elements = 1;
			for(i=0;i < num_dimensions;i++){
				gen->len_dimensions[i] = len_dimensions[i];
				gen->num_elements *= len_dimensions[i];
			}
		}

		if(copy_data){
			gen->data = NhlMalloc(gen->num_elements * gen->size);
			if(gen->data == NULL)
				return NULL;

			/*
			 * If the individual elements are NhlString's then we
			 * know how to copy them.
			 */
			if((gen->typeQ == QString) &&
					(gen->size == sizeof(NhlString))){
				NhlString	*otable = data;
				NhlString	*ntable = gen->data;

				for(i=0;i<gen->num_elements;i++){
					if(otable[i] == NULL){
						ntable[i] = NULL;
					}
					else{
						ntable[i] =
						NhlMalloc(strlen(otable[i])+1);
						if(ntable[i] == NULL)
							return NULL;
						strcpy(ntable[i],otable[i]);
					}
				}
			}
			else{
				memcpy(gen->data,data,
						gen->num_elements * gen->size);
			}
			gen->my_data = True;
		}
		else{
			gen->data = data;
			gen->my_data = False;
		}
	}

	return gen;
}

/*
 * Function:	NhlCreateGenArray
 *
 * Description:	This function is used by the user to define the size/shape
 *		of arrays passed in as resources. It does not copy the
 *		data - it only keeps a pointer to it.  The data pointer
 *		memory belongs to the user.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	global public
 * Returns:	NhlGenArray
 * Side Effect:	
 */
NhlGenArray
NhlCreateGenArray
#if	NhlNeedProto
(
	NhlPointer	data,		/* data array		*/
	NhlString	type,		/* type of each element	*/
	unsigned int	size,		/* size of each element	*/
	int		num_dimensions,	/* number of dimensions	*/
	int		*len_dimensions	/* number of dimensions	*/
)
#else
(data,type,size,num_dimensions,len_dimensions)
	NhlPointer	data;			/* data array		*/
	NhlString	type;			/* type of each element	*/
	unsigned int	size;			/* size of each element	*/
	int		num_dimensions;		/* number of dimensions	*/
	int		*len_dimensions;	/* number of dimensions	*/
#endif
{
	return
	_NhlCreateGenArray(data,type,size,num_dimensions,len_dimensions,False);
}

/*
 * Function:	_NhlCopyGenArray
 *
 * Description:	This function copies an NhlGenArray and allocates an
 *		NhlGenArray. It copies the "data" part of the GenArray
 *		if copy_data is true - otherwise the new GenArray just
 *		references the same data pointer.
 *
 * In Args:	
 *		NhlGenArray	gen		generic array pointer
 *		NhlBoolean	copy_data	copy data?
 *
 * Out Args:	
 *
 * Scope:	Global Private
 * Returns:	NhlGenArray
 * Side Effect:	
 */
NhlGenArray
_NhlCopyGenArray
#if	NhlNeedProto
(
	NhlGenArray	gen,		/* generic array pointer	*/
	NhlBoolean	copy_data	/* copy data?			*/
)
#else
(gen,copy_data)
	NhlGenArray	gen;		/* generic array pointer	*/
	NhlBoolean	copy_data;	/* copy data?			*/
#endif
{
	return _NhlCreateGenArray(gen->data,NrmQuarkToString(gen->typeQ),
		gen->size,gen->num_dimensions,gen->len_dimensions,copy_data);
}

/*
 * Function:	NhlFreeGenArray
 *
 * Description:	This function is used by the user to destroy an NhlGenArray
 *		description record after they are done using it.  This
 *		function does not free the "data" part of the NhlGenArray
 *		the "data" pointer memory belongs to the user.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	global public
 * Returns:	NhlGenArray
 * Side Effect:	
 */
void
NhlFreeGenArray
#if	NhlNeedProto
(
	NhlGenArray	gen	/* gen array to free	*/
)
#else
(gen)
	NhlGenArray	gen;	/* gen array to free	*/
#endif
{
	static NhlBoolean	first_time = True;
	static NrmQuark		QString;

	if(first_time){
		QString = NrmStringToQuark(NhlTString);
		first_time = False;
	}

	if(gen == NULL)
		return;

	if(gen->my_data){
		if(gen->typeQ == QString){
			int i;
			NhlString	*table = gen->data;

			for(i=0;i<gen->num_elements;i++)
				(void)NhlFree(table[i]);
		}
		(void)NhlFree(gen->data);
	}
	if(gen->len_dimensions != &gen->num_elements)
		(void)NhlFree(gen->len_dimensions);

	(void)NhlFree(gen);

	return;
}


/*
 * Function:    _NhlValidatedGenArrayCopy
 *
 * Description: Copies Generic Arrays, with checking, from gfrom to gto.
 *
 *		Pointer gto must point to an allocated Generic Array that
 *		is assumed to be valid and serves as a template for 
 *		validating the user-supplied gfrom Generic Array. It need not
 *		contain any actual data (num_elements may be 0).
 *		The type, type size, number of dimensions, and
 *		length of all dimensions except the first, must match 
 *		between both arrays for the copy to proceed. The first
 *		dimension of the gfrom array may be greater or smaller 
 *		than the first dimension of the gto array. If greater the
 *		gto array is freed if non-NULL, and the routine performs a 
 *		complete copy. Otherwise, it copies the data only into
 *		the space already assigned to the gto array. In this 
 *		situation, the setting of the exact_count parameter controls
 *		whether the num_elements field of the gto array is possibly
 *		adjusted (downward only) to reflect the actual number of 
 *		elements in the gfrom array.
 *
 * In Args:     NhlGenArray	gfrom -- the generic array that is copied. 
 *		int		max_el   -- the maximum number of elements
 *					    permitted
 *		NhlBoolean	copy_data -- whether to allocate storage
 *					     space for the data.
 *		NhlBoolean	exact_count -- whether to copy the size
 *					       info when a data only copy is
 *					       performed. If False, the
 *					     num_elements and len_dimension[0]
 *					     of gto will reflect the allocated
 *					       space in gto rather than the
 *					       number of elements copied from
 *					       the gfrom array.
 *		char *		res_name -- The associated resource name
 *		char *		caller -- name of the user calling routine
 *
 * Out Args:	NhlGenArray *	gto -- Pointer to a destination Generic Array.
 *				       All its fields are assumed to contain 
 *				       valid information.
 *
 * Scope:       Global Private
 *
 * Returns:     If successful NhlNOERROR; NhlFATAL on memory allocation errors;
 *              NhlWARNING if the from GenArray is invalid in some way.
 *
 * Side Effect: When string data is copied, all NULL string pointers are 
 *		replaced with single-byte strings containing only a NULL
 *		terminator.
 *
 */

NhlErrorTypes _NhlValidatedGenArrayCopy
#if NhlNeedProto
	(NhlGenArray	*gto, 
	 NhlGenArray	gfrom,
	 int		max_el,
	 NhlBoolean	copy_data,
	 NhlBoolean	exact_count,
	 char		*res_name,
	 char		*caller)
#else
(gto,gfrom,max_el,copy_data,exact_count,res_name,caller)
	NhlGenArray	*gto; 
	NhlGenArray	gfrom;
	int		max_el;
	NhlBoolean	copy_data;
	NhlBoolean	exact_count;
	char		*res_name;
	char		*caller;
#endif
{
	char		*e_text;
	int		i;
	int		el_count;
	static NrmQuark Qstring;
	static NhlBoolean first = True;

	if (first) {
		Qstring = NrmStringToQuark(NhlTString);
		first = False;
	}

	if (gfrom == NULL || (*gto) == NULL) {
		e_text = 
		 "%s: %s NULL array passed in: copy not performed";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,caller,res_name);
		return NhlWARNING;
	}
	if (gfrom->num_elements <= 0) {
		e_text = 
		 "%s: %s invalid element count: ignoring";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,caller,res_name);
		return NhlWARNING;
	}
	else if (gfrom->num_elements > max_el) {
		e_text =
		 "%s: %s exceeds maximum number of elements, %d: ignoring";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,caller,res_name,max_el);
		return NhlWARNING;
	}
	if (gfrom->num_dimensions != (*gto)->num_dimensions) {
		e_text = 
		 "%s: %s invalid dimensionality: copy not performed";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,caller,res_name);
		return NhlWARNING;
	}
/*
 * All dimensions except the first (index 0) must match in length
 */
	el_count = gfrom->len_dimensions[0];
	for (i=1; i<gfrom->num_dimensions; i++) {
		el_count *= gfrom->len_dimensions[i];
		if (gfrom->len_dimensions[i] != (*gto)->len_dimensions[i]) {
			e_text = 
			    "%s: %s dimensional mismatch: copy not performed";
			NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,caller,res_name);
			return NhlWARNING;
		}
	}
	if (el_count != gfrom->num_elements) {
		e_text = 
		 "%s: %s invalid element count: copy not performed";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,caller,res_name);
		return NhlWARNING;
	}

	if (gfrom->typeQ != (*gto)->typeQ) {
		e_text = "%s: %s type mismatch: copy not performed";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,caller,res_name);
		return NhlWARNING; 
	}
	if (gfrom->size != (*gto)->size) {
		e_text = "%s: %s type size mismatch: copy not performed";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,caller,res_name);
		return NhlWARNING; 
	}
	
	if (gfrom->num_elements > (*gto)->num_elements) {
		NhlFreeGenArray((*gto));
		if (((*gto) = _NhlCopyGenArray(gfrom,copy_data)) == NULL) {
			e_text = "%s: error copying %s GenArray";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,caller,res_name);
			return NhlFATAL;
		}
		return NhlNOERROR;
	}
	else if (!copy_data)
			(*gto)->data = gfrom->data;
	else if (gfrom->typeQ != Qstring) {
		memcpy((void *)(*gto)->data, (Const void *)gfrom->data,
		       gfrom->num_elements * gfrom->size);
	} 
	else {
		NhlString *from = (NhlString *) gfrom->data;
		NhlString *to = (NhlString *) (*gto)->data;
		for (i=0; i<gfrom->num_elements; i++,to++,from++) {
			if (*from == NULL) {
				*to = (NhlString) 
					NhlRealloc(*to,
						   strlen("")+1);
				if (*to == NULL) {
					e_text = "%s: error copying %s string";
					NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,
						  caller,res_name);
					return NhlFATAL;
				}
				strcpy(*to,"");
			}
			else if (*to == NULL) {
				*to = (NhlString) NhlMalloc(strlen(*from)+1);
				if (*to == NULL) {
					e_text = "%s: error copying %s string";
					NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,
						  caller,res_name);
					return NhlFATAL;
				}
				strcpy(*to,*from);
			}
			else if (strcmp(*to,*from)) {
				*to = (NhlString) 
					NhlRealloc(*to, strlen(*from)+1);
				if (*to == NULL) {
					e_text = "%s: error copying %s string";
					NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,
						  caller,res_name);
					return NhlFATAL;
				}
				strcpy(*to,*from);
			}
		}
		if (exact_count) {
			to = (NhlString *)(*gto)->data + gfrom->num_elements;
			for (i=gfrom->num_elements;
			     i<(*gto)->num_elements; i++) {
				NhlFree(*to++);
			}
		}
				
	}

/* 
 * ASSERT: (*gto)->num_elements >= gfrom->num_elements
 */
	if (exact_count) {
		(*gto)->num_elements = gfrom->num_elements;
		(*gto)->len_dimensions[0] = gfrom->len_dimensions[0];
	}

	return NhlNOERROR;

}

/*
 * Function:	_NhlTmpFile
 *
 * Description:	This function emulates the Standard C function 'tmpfile'
 *		except that it allows the directory location of the file
 *		to be controlled. If the environment variable "TMPDIR"
 *		is set the file will be located in TMPDIR; else it will
 *		located in the directory given by GetNCARGPath("tmp"). 
 *		Since the file is immediately unlinked, its name is 
 *		irrelevant.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	Global Private
 * Returns:	FILE * or NULL on failure
 * Side Effect:	
 */
FILE *
_NhlTmpFile
#if	NhlNeedProto
(
	void
)
#else
()
#endif
{
	char *e_text;
	char *entry_name = "_NhlTmpName";
	char *fname;
	FILE *fp;

	if ((fname = tempnam((char *)GetNCARGPath("tmp"),NULL)) == NULL) {
		e_text = "%s, error getting temporary file name";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return NULL;
	}

	if ((fp = fopen(fname,"w+")) == NULL) {
		e_text = "%s, error opening temporary file";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return NULL;
	}

	if (unlink(fname) < 0) {
		e_text = "%s, error unlinking temporary file";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return NULL;
	}
	NhlFree(fname);
	
	return fp;
}

/*
 * Function:	_NhlCopyToVal
 *
 * Description:	copies the memory pointed to by "src" to an _NhlArgVal union.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
void
_NhlCopyToVal
#if	NhlNeedProto
(
	NhlPointer	src,
	_NhlArgVal	*dst,
	unsigned int	size
)
#else
(src,dst,size)
	NhlPointer	src;
	_NhlArgVal	*dst;
	unsigned int	size;
#endif
{ 
    if      (size == sizeof(long)) dst->lngval = *(long*)src;
    else if (size == sizeof(short)) dst->shrtval = *(short*)src;
    else if (size == sizeof(NhlPointer)) dst->ptrval = *(NhlPointer*)src;
    else if (size == sizeof(char))	dst->charval = *(char*)src;
    else if (size == sizeof(char*))	dst->strval = *(char**)src;
    else if (size == sizeof(_NhlArgVal)) *dst = *(_NhlArgVal*)src;
    else
        memcpy((NhlPointer)dst,(NhlPointer)&src,size);
}
