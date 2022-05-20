#ifndef _CTRL_TEXTFIELDP_H
#define _CTRL_TEXTFIELDP_H

#include <control/ControlP.h>
#include <control/TextField.h>
#include <control/PrimitiveP.h>
#include <X11/Xft/Xft.h>

typedef struct {
	int foo;                                        /* dummy field */
} CtrlTextFieldClassPart;

typedef struct _CtrlTextFieldClassRec {
	CoreClassPart           core_class;
	CtrlPrimitiveClassPart  primitive_class
	CtrlTextFieldClassPart  text_class;
} CtrlTextFieldClassRec;

extern CtrlTextFieldClassRec    ctrlTextFieldClassRec;

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
	XftFont                *font;
	XftColor               *foreground;
	XftColor               *selected_color;
	Time                    last_time;              /* time of last selection event */
	Time                    blink_rate;             /* rate of blinking text cursor in msec */
	Dimension               threshold;              /* selection threshold */
	Dimension               margin_width;
	Dimension               margin_height;
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
} CtrlTextFieldPart;

typedef struct _CtrlTextFieldRec {
	CorePart                core;
	CtrlPrimitivePart       primitive;
	CtrlTextFieldPart       text;
} CtrlTextFieldRec;

#endif /* _CTRL_TEXTFIELDP_H */
