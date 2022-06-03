#ifndef _CONTROL_PRIVATE_H
#define _CONTROL_PRIVATE_H

#include <control.h>
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/ShellP.h>

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

typedef struct {
	int foo;                                        /* dummy field */
} CtrlShellClassPart;

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

typedef struct _CtrlShellClassRec {
	CoreClassPart           core_class;
	CompositeClassPart      composite_class;
	ShellClassPart          shell_class;
	WMShellClassPart        wm_shell_class;
	VendorShellClassPart    vendor_shell_class;
	TopLevelShellClassPart top_level_shell_class;
	ApplicationShellClassPart application_shell_class;
	SessionShellClassPart   session_shell_class;
	CtrlShellClassPart      ctrl_shell_class;
} CtrlShellClassRec;

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
	/* resources */
	XtCallbackList          activate_callback;
	XtCallbackList          value_changed_callback;
	XtPointer               selforeground;
	Pixel                   selbackground;
	String                  value;
	Dimension               columns;                /* number of character columns in the text input field */

	/* internal */
	XtPointer               undo_list;              /* list of editions */
	XtPointer               undo_current;           /* current position in the undo list */
	String                  preedit_value;
	String                  clipboard_value;        /* contents of last Ctrl-C */
	String                  primary_value;
	Time                    last_time;              /* time of last selection event */
	Boolean                 last_modify;            /* last modification operation */
	Boolean                 overstrike;
	Boolean                 under_preedit;
	Boolean                 select_word;            /* whether the user have done the first click to select a word */
	int                     h_offset;
	int                     text_size;              /* size of allocated value string */
	int                     text_length;            /* used size of value string */
	int                     caret_position;
	int                     selection_position;     /* position of the other selection extremity */
	int                     cursor_position;        /* position of the insertion cursor and one selection extremity */
	int                     preedit_size;
	int                     preedit_length;
	int                     clipboard_size;
	int                     primary_size;
	XIC                     xic;
} CtrlTextFieldPart;

typedef struct {
	/* resources */
	XtCallbackList          close_callback;
	Boolean                 follow_focus;

	/* internal */
	XIM                     xim;
	XIMStyle                pref_style;
} CtrlShellPart;

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

typedef struct _CtrlShellRec {
	CorePart                core;
	CompositePart           composite;
	ShellPart               shell;
	WMShellPart             wm;
	VendorShellPart         vendor;
	TopLevelShellPart       topLevel;
	ApplicationShellPart    application;
	SessionShellPart        session;
	CtrlShellPart           ctrl;
} CtrlShellRec;

/* external objects */
extern CtrlShellClassRec        ctrlShellClassRec;
extern CtrlPrimitiveClassRec    ctrlPrimitiveClassRec;
extern CtrlTextFieldClassRec    ctrlTextFieldClassRec;

#endif /* _CONTROL_PRIVATE_H */
