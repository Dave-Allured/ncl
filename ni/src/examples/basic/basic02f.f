C
C The first frame in this example demonstrates how to set the view port 
C for a contour plot.
C Note: no data is used in this example, so the output appears
C only as a bounding box with tickmarks.
C
C The second frame in this example demonstrates how to produce multiple
C plots on a single frame.
C

        program basic02f
        implicit none

        external NhlFappLayerClass
        external NhlFResListLayerClass
        external NhlFXWorkstationLayerClass
        external NhlFContourLayerClass

        integer appid,wks,con1,rlist,ierr

C Initialize the graphics libraries and create a resource list that
C is normally used to assign name/value pairs within objects.  Then
C clear (empty) this list, and create an application object.  This
C object manages multiple resource databases used by seperate objects.

        call NhlFInitialize
        call NhlFRLCreate(rlist,'SETRL')

        call NhlFRLClear(rlist)
        call NhlFCreate(appid,"basic02",NhlFappLayerClass,0,rlist,ierr)


C ###########
C # FRAME 1 #
C ###########
C Choose the type of output you want to create.  You may write your
C output to an NCGM, file, X workstation window, or a PostScript file. 
C This example writes to an X Workstation.
 
        call NhlFRLClear(rlist)
        call NhlFCreate(wks,"wks",NhlFxWorkstationLayerClass,0,
     $       rlist,ierr)

C Create a plot object.  In this example, we will create a contour plot.
C
C Four view class resources, vpXF, vpYF, vpWidthF, and vpHeightF, are
C assigned values in the following create call.  The combination of
C these four resources determines where the plot will display in the
C output window.  The values of these resources are specified in 
C Normalized Device Coordinates (NDCs).  In this two-dimensional coordinate 
C system (0,0) specifies the lower-left corner and (1,1) specifies the 
C uper-right corner of a plot.

        call NhlFRLClear(rlist)
	call NhlFRLSetFloat(rlist,"vpXF",0.05,ierr) 
	call NhlFRLSetFloat(rlist,"vpYF",0.95,ierr) 
	call NhlFRLSetFloat(rlist,"vpWidthF",0.4,ierr) 
	call NhlFRLSetFloat(rlist,"vpHeightF",0.4,ierr) 
        call NhlFCreate(con1,"con1",NhlFcontourLayerClass,wks,
     $       rlist,ierr)

C Draw the plot. 

	call NhlFDraw(con1,ierr)

C The frame call updates and then clears the workstation.
C Anything written to the workstation after a frame call is made will be
C drawn in a subsequent frame. 

	call NhlFFrame(wks,ierr)

C ###########
C # FRAME 2 #
C ###########
C
C This example demonstrates drawing multiple plots in a single frame.
C
C Calling draw again will produce the identical plot that was drawn in the
C first frame.

	call NhlFDraw(con1,ierr)

C To add another plot to the same frame, we first need to reset the 
C viewport resources so that the next plot does not overwrite the first
C one.  The setvalues expression is used to set resources after an object
C has already been created.  The first argument, "con1", in the setvalues
C expression specifes an object id of a plot that was generated earlier
C with the create call.  This is then followed by a list of resource value
C pairs that apply to the object.

        call NhlFRLClear(rlist)
	call NhlFRLSetFloat(rlist,"vpXF",0.55,ierr) 
	call NhlFRLSetFloat(rlist,"vpYF",0.45,ierr) 
	call NhlFRLSetFloat(rlist,"vpWidthF",0.2,ierr) 
	call NhlFRLSetFloat(rlist,"vpHeightF",0.2,ierr) 
        call NhlFSetValues(con1,rlist,ierr)

C Because of the new viewport resource settings, calling draw produces 
C a plot in the lower-right quadrant of the frame.

	call NhlFDraw(con1,ierr)

C Updates and clear the workstation.

	call NhlFFrame(wks,ierr)

C Clean up (deleting the parent object recursively deletes all of its 
C children).

	call NhlFDestroy(con1,ierr)
	call NhlFClose

	stop
	end
