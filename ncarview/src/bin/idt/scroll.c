/*
 *	$Id: scroll.c,v 1.10 1992-12-14 23:08:19 clyne Exp $
 */
/*
 *	scroll.c
 *
 *	Author		John Clyne
 *
 *	Date		Fri Oct 19 12:58:42 MDT 1990
 *
 *	The module contains widget independent code used by the scroll
 *	widget. 
 */
#include <sys/types.h>
#include "commands.h"
#include "display.h"
#include "scroll.h"

/*
 *	ScrollTo
 *	[exported]
 *
 *	This routine is called in response to the scroll bar being scrolled.
 *	It determines whether the scroll bar has moved sufficiently to warrant
 *	generating a plot command to a translator
 *
 * on entry
 *	*wd			: widget data.
 *	wd->start_segment	: the starting segment frame number
 *	wd->stop_segment	: the ending segment frame number
 *	percent			: percent of scroll bar thumb wheel was moved to
 */
void	ScrollTo(WidgetData *wd, float percent)
{
	int	start, stop;
	int	frame;
	char	buf[10];



	/*
	 * find out how many frames are covered by the segment defined
	 * for this translator
	 */
	start = wd->pcv.start_segment;
	stop = wd->pcv.stop_segment;

	/*
	 * map percent into a frame number
	 */
	frame = (int) ((float) (stop - start + 1) * percent) + start;

	/*
	 * If the last frame from matches the
	 * current requested frame do nothing
	 */
	if (frame == wd->current_frame_num) {
		return;	/* do nothing	*/
	}

	wd->current_frame_num = frame;	/* record current frame	*/

	(void) sprintf(buf, "%d", frame);
	if (wd->do_animate) {
		AnimateDisplayImage(wd->a, frame-1);
	}
	else {
		/*
		 * send the plotting command
		 */
		Command(wd->id, GOTO_STRING, buf);
	}

	/*
	 * update the displayed frame count
	 */
	UpdateFrameLabel(wd, buf);
}
