#ifndef _CONTROL_PRIVATE_H
#define _CONTROL_PRIVATE_H

#include <control.h>
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>

/* class part structures */

typedef struct {
	XtWidgetProc            highlight;
	XtWidgetProc            unhighlight;
	XtWidgetProc            press;
	XtWidgetProc            unpress;
	XtWidgetProc            tooltip_post;
	XtWidgetProc            tooltip_unpost;
	XtActionProc            activate;
	XtWidgetProc            draw;
} CtrlPrimitiveClassPart;

typedef struct {
	int foo;                                        /* dummy field */
} CtrlTextFieldClassPart;

/* class structures */

typedef struct _CtrlPrimitiveClassRec {
	CoreClassPart           core_class;
	CtrlPrimitiveClassPart  primitive_class;
} CtrlPrimitiveClassRec;

typedef struct _CtrlTextFieldClassRec {
	CoreClassPart           core_class;
	CtrlPrimitiveClassPart  primitive_class;
	CtrlTextFieldClassPart  text_class;
} CtrlTextFieldClassRec;

/* widget part structures */

typedef struct {
	/* resource fields */
	XtPointer               foreground;

	XtPointer               font;
	Dimension               font_average_width;     /* average character width */
	Dimension               font_height;
	Dimension               font_ascent;
	Dimension               font_descent;
	Dimension               margin_width;
	Dimension               margin_height;

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

typedef struct {
	XtCallbackList          activate_callback;
	XtCallbackList          destination_callback;
	XtCallbackList          focus_callback;
	XtCallbackList          losing_focus_callback;
	XtCallbackList          gain_primary_callback;
	XtCallbackList          lose_primary_callback;
	XtCallbackList          gain_clipboard_callback;
	XtCallbackList          lose_clipboard_callback;
	XtCallbackList          value_changed_callback;
	XtPointer               selforeground;
	Pixel                   selbackground;
	String                  value;
	Time                    last_time;              /* time of last selection event */
	Time                    blink_rate;             /* rate of blinking text cursor in msec */
	Position                text_size;              /* size of allocated value string */
	Position                text_length;            /* used size of value string */
	Position                columns;                /* number of character columns in the text input field */
	Position                h_offset;
	Position                cursor_position;        /* position of the insertion cursor and one selection extremity */
	Position                selection_position;     /* position of the other selection extremity */
	Position                preedit_position;       /* position of input cursor while compositing */
	Position                caret_position;
	Position                preedit_start;
	Position                preedit_end;
	Dimension               select_threshold;
	Boolean                 blink_on;
	Boolean                 has_focus;
	Boolean                 has_primary_selection;
	Boolean                 has_clipboard_selection;
	Boolean                 has_destination_selection;
	Boolean                 selection_move;
	Boolean                 overstrike;
	Boolean                 under_preedit;
	XtIntervalId            timer_id;
	XIC                     xic;
} CtrlTextFieldPart;

/* widget structures */

typedef struct _CtrlPrimitiveRec {
	CorePart                core;
	CtrlPrimitivePart       primitive;
} CtrlPrimitiveRec;

typedef struct _CtrlTextFieldRec {
	CorePart                core;
	CtrlPrimitivePart       primitive;
	CtrlTextFieldPart       text;
} CtrlTextFieldRec;

/* external objects */
extern CtrlPrimitiveClassRec    ctrlPrimitiveClassRec;
extern CtrlTextFieldClassRec    ctrlTextFieldClassRec;

#endif /* _CONTROL_PRIVATE_H */
