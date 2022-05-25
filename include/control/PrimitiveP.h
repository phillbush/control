#ifndef _CTRL_PRIMITIVEP_H
#define _CTRL_PRIMITIVEP_H

#include <control/ControlP.h>
#include <control/Primitive.h>
#include <X11/CoreP.h>

typedef struct {
	XtWidgetProc            highlight;
	XtWidgetProc            unhighlight;
	XtWidgetProc            press;
	XtWidgetProc            unpress;
	XtWidgetProc            tooltip_post;
	XtWidgetProc            tooltip_unpost;
	XtActionProc            activate;
	XtWidgetProc            draw;
	String                  translations;
} CtrlPrimitiveClassPart;

typedef struct _CtrlPrimitiveClassRec {
	CoreClassPart           core_class;
	CtrlPrimitiveClassPart  primitive_class;
} CtrlPrimitiveClassRec;

extern CtrlPrimitiveClassRec    ctrlPrimitiveClassRec;

typedef struct {
	/* resource fields */
	Pixel                   foreground;

	Dimension               shadow_thickness;
	Pixel                   shadow_light_pixel;
	Pixmap                  shadow_light_pixmap;
	Pixel                   shadow_dark_pixel;
	Pixmap                  shadow_dark_pixmap;

	Dimension               highlight_thickness;
	Pixel                   highlight_pixel;
	Pixmap                  highlight_pixmap;

	Boolean                 is_tab_group;
	Boolean                 traverseable;

	String                  tooltip;

	Cursor                  cursor;

	/* widget state */
	Boolean                 is3d;
	Boolean                 focusable;
	Boolean                 highlighted;    /* whether the highlight border is drawn around the widget */
	Boolean                 pressed;        /* whether the shadow borders are inverted */
	Boolean                 have_traversal; /* whether the widget has the traversal focus */
	Pixmap                  pixsave;        /* pixmap where to draw widget content to */
} CtrlPrimitivePart;

typedef struct _CtrlPrimitiveRec {
	CorePart                core;
	CtrlPrimitivePart       primitive;
} CtrlPrimitiveRec;

#endif /* _CTRL_PRIMITIVEP_H */
