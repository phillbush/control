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
	String                  translations;
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

typedef struct {
	XtCallbackList          activate_callback;
	XtCallbackList          destination_callback;
	XtCallbackList          focus_callback;
	XtCallbackList          losing_focus_callback;
	XtCallbackList          gain_primary_callback;
	XtCallbackList          lose_primary_callback;
	XtCallbackList          gain_clipboard_callback;
	XtCallbackList          lose_clipboard_callback;
	XtCallbackList          modify_verify_callback;
	XtCallbackList          motion_verify_callback;
	XtCallbackList          value_changed_callback;
	String                  value;
	Cardinal                text_size;              /* size of allocated value string */
	Cardinal                text_length;            /* used size of value string */
	Cardinal                cursor_position;        /* position of the insertion cursor and one selection extremity */
	Cardinal                selection_position;     /* position of the other selection extremity */
	Cardinal                first_visible;          /* position of first visible character */
	Cardinal                preedit_position;       /* position of input cursor while compositing */
	Cardinal                preedit_start;
	Cardinal                preedit_end;
	Cardinal                columns;                /* number of character columns in the text input field */
	XtPointer               font;
	XtPointer               foreground;
	XtPointer               selected_color;
	Time                    last_time;              /* time of last selection event */
	Time                    blink_rate;             /* rate of blinking text cursor in msec */
	Position                h_offset;
	Dimension               select_threshold;
	Dimension               margin_width;
	Dimension               margin_height;
	Dimension               font_average_width;     /* average character width */
	Dimension               font_height;
	Dimension               font_ascent;
	Dimension               font_descent;
	Boolean                 blink_on;
	Boolean                 has_focus;
	Boolean                 has_primary_selection;
	Boolean                 has_clipboard_selection;
	Boolean                 has_destination_selection;
	Boolean                 selection_move;
	Boolean                 overstrike;
	Boolean                 under_preedit;
	XtIntervalId            timer_id;
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
