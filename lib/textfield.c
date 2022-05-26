#include <stdio.h>
#include <string.h>

#include <control_private.h>

#include "common.h"

/* core methods */
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Realize(Widget, XtValueMask *, XSetWindowAttributes *);
static void Destroy(Widget);
static void Resize(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);

/* primitive methods */
static void Draw(Widget);

/* actions */
static void InsertChar(Widget, XEvent *, String *, Cardinal *);

/* preedit callback functions */
static int PreeditStart(XIC, XPointer, XPointer);
static int PreeditDone(XIC, XPointer, XPointer);
static int PreeditDraw(XIC, XPointer, XPointer);
static int PreeditCaret(XIC, XPointer, XPointer);
static int PreeditDestroy(XIC, XPointer, XPointer);

/* helper widget-internal functions */
static void AdjustText(CtrlTextFieldWidget, Cardinal);

static XtActionsRec actions[] = {
	/* text replacing bindings */
	{"insert-char",                 InsertChar},
};

static XtResource resources[] = {
	{
		.resource_name   = CtrlNcolumns,
		.resource_class  = CtrlCColumns,
		.resource_type   = CtrlRCardinal,
		.resource_size   = sizeof(Cardinal),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.columns),
		.default_type    = CtrlRImmediate,
		.default_addr    = (XtPointer)DEF_TEXT_COLUMNS,
	},
	{
		.resource_name   = CtrlNfont,
		.resource_class  = CtrlCFont,
		.resource_type   = CtrlRFont,
		.resource_size   = sizeof(XtPointer),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.font),
		.default_type    = CtrlRString,
		.default_addr    = (XtPointer)DEF_FONT,
	},
	{
		.resource_name   = CtrlNactivateCallback,
		.resource_class  = CtrlCCallback,
		.resource_type   = CtrlRCallback,
		.resource_size   = sizeof(XtCallbackList),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.activate_callback),
		.default_type    = CtrlRCallback,
		.default_addr    = (XtPointer)NULL,
	},
	{
		.resource_name   = CtrlNdestinationCallback,
		.resource_class  = CtrlCCallback,
		.resource_type   = CtrlRCallback,
		.resource_size   = sizeof(XtCallbackList),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.destination_callback),
		.default_type    = CtrlRCallback,
		.default_addr    = (XtPointer)NULL,
	},
	{
		.resource_name   = CtrlNfocusCallback,
		.resource_class  = CtrlCCallback,
		.resource_type   = CtrlRCallback,
		.resource_size   = sizeof(XtCallbackList),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.focus_callback),
		.default_type    = CtrlRCallback,
		.default_addr    = (XtPointer)NULL,
	},
	{
		.resource_name   = CtrlNlosingFocusCallback,
		.resource_class  = CtrlCCallback,
		.resource_type   = CtrlRCallback,
		.resource_size   = sizeof(XtCallbackList),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.losing_focus_callback),
		.default_type    = CtrlRCallback,
		.default_addr    = (XtPointer)NULL,
	},
	{
		.resource_name   = CtrlNgainPrimaryCallback,
		.resource_class  = CtrlCCallback,
		.resource_type   = CtrlRCallback,
		.resource_size   = sizeof(XtCallbackList),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.gain_primary_callback),
		.default_type    = CtrlRCallback,
		.default_addr    = (XtPointer)NULL,
	},
	{
		.resource_name   = CtrlNlosePrimaryCallback,
		.resource_class  = CtrlCCallback,
		.resource_type   = CtrlRCallback,
		.resource_size   = sizeof(XtCallbackList),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.lose_primary_callback),
		.default_type    = CtrlRCallback,
		.default_addr    = (XtPointer)NULL,
	},
	{
		.resource_name   = CtrlNgainClipboardCallback,
		.resource_class  = CtrlCCallback,
		.resource_type   = CtrlRCallback,
		.resource_size   = sizeof(XtCallbackList),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.gain_clipboard_callback),
		.default_type    = CtrlRCallback,
		.default_addr    = (XtPointer)NULL,
	},
	{
		.resource_name   = CtrlNloseClipboardCallback,
		.resource_class  = CtrlCCallback,
		.resource_type   = CtrlRCallback,
		.resource_size   = sizeof(XtCallbackList),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.lose_clipboard_callback),
		.default_type    = CtrlRCallback,
		.default_addr    = (XtPointer)NULL,
	},
	{
		.resource_name   = CtrlNmodifyVerifyCallback,
		.resource_class  = CtrlCCallback,
		.resource_type   = CtrlRCallback,
		.resource_size   = sizeof(XtCallbackList),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.modify_verify_callback),
		.default_type    = CtrlRCallback,
		.default_addr    = (XtPointer)NULL,
	},
	{
		.resource_name   = CtrlNmotionVerifyCallback,
		.resource_class  = CtrlCCallback,
		.resource_type   = CtrlRCallback,
		.resource_size   = sizeof(XtCallbackList),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.motion_verify_callback),
		.default_type    = CtrlRCallback,
		.default_addr    = (XtPointer)NULL,
	},
	{
		.resource_name   = CtrlNvalueChangedCallback,
		.resource_class  = CtrlCCallback,
		.resource_type   = CtrlRCallback,
		.resource_size   = sizeof(XtCallbackList),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.value_changed_callback),
		.default_type    = CtrlRCallback,
		.default_addr    = (XtPointer)NULL,
	},
	{
		.resource_name   = CtrlNvalue,
		.resource_class  = CtrlCValue,
		.resource_type   = CtrlRString,
		.resource_size   = sizeof(String),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.value),
		.default_type    = CtrlRString,
		.default_addr    = (XtPointer)"",
	},
	{
		.resource_name   = CtrlNmarginHeight,
		.resource_class  = CtrlCMarginHeight,
		.resource_type   = CtrlRDimension,
		.resource_size   = sizeof(Dimension),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.margin_height),
		.default_type    = CtrlRImmediate,
		.default_addr    = (XtPointer)DEF_TEXT_MARGIN,
	},
	{
		.resource_name   = CtrlNmarginWidth,
		.resource_class  = CtrlCMarginWidth,
		.resource_type   = CtrlRDimension,
		.resource_size   = sizeof(Dimension),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.margin_width),
		.default_type    = CtrlRImmediate,
		.default_addr    = (XtPointer)DEF_TEXT_MARGIN,
	},
	{
		.resource_name   = CtrlNblinkRate,
		.resource_class  = CtrlCBlinkRate,
		.resource_type   = CtrlRTime,
		.resource_size   = sizeof(Time),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.blink_rate),
		.default_type    = CtrlRImmediate,
		.default_addr    = (XtPointer)DEF_BLINK_RATE,
	},
	{
		.resource_name   = CtrlNselectThreshold,
		.resource_class  = CtrlCSelectThreshold,
		.resource_type   = CtrlRDimension,
		.resource_size   = sizeof(Dimension),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.select_threshold),
		.default_type    = CtrlRImmediate,
		.default_addr    = (XtPointer)DEF_SELECT_THRESHOLD,
	},
	{
		.resource_name   = CtrlNisTabGroup,
		.resource_class  = CtrlCIsTabGroup,
		.resource_type   = CtrlRBoolean,
		.resource_size   = sizeof(Boolean),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, primitive.is_tab_group),
		.default_type    = CtrlRImmediate,
		.default_addr    = (XtPointer)TRUE,
	},
	{
		.resource_name   = CtrlNcursor,
		.resource_class  = CtrlCCursor,
		.resource_type   = CtrlRCursor,
		.resource_size   = sizeof(Cursor),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, primitive.cursor),
		.default_type    = CtrlRString,
		.default_addr    = "pirate",
	},
};

CtrlTextFieldClassRec ctrlTextFieldClassRec = {
	.core_class = {
		.superclass             = (WidgetClass)&ctrlPrimitiveClassRec,
		.class_name             = "CtrlTextField",
		.widget_size            = sizeof(CtrlTextFieldRec),
		.class_initialize       = NULL,
		.class_part_initialize  = NULL,
		.class_inited           = FALSE,
		.initialize             = Initialize,
		.realize                = Realize,
		.actions                = actions,
		.num_actions            = XtNumber(actions),
		.resources              = resources,
		.num_resources          = XtNumber(resources),
		.xrm_class              = NULLQUARK,
		.compress_motion        = TRUE,
		.compress_exposure      = XtExposeCompressMaximal | XtExposeNoRegion,
		.compress_enterleave    = TRUE,
		.visible_interest       = FALSE,
		.destroy                = Destroy,
		.resize                 = Resize,
		.expose                 = XtInheritExpose,
		.set_values             = SetValues,
		.set_values_almost      = XtInheritSetValuesAlmost,
		.get_values_hook        = NULL,
		.accept_focus           = NULL,
		.version                = XtVersion,
		.callback_private       = NULL,
		.tm_table               = _CtrlTextTranslations,
		.query_geometry         = QueryGeometry,
		.display_accelerator    = NULL,
		.extension              = NULL,

		/* obsolete */
		.initialize_hook        = NULL,
		.set_values_hook        = NULL,
	},
	.primitive_class = {
		.highlight              = (XtWidgetProc)_XtInherit,
		.unhighlight            = (XtWidgetProc)_XtInherit,
		.press                  = (XtWidgetProc)_XtInherit,
		.unpress                = (XtWidgetProc)_XtInherit,
		.tooltip_post           = (XtWidgetProc)_XtInherit,
		.tooltip_unpost         = (XtWidgetProc)_XtInherit,
		.draw                   = Draw,
		.activate               = NULL,
		.translations           = NULL,
	},
	.text_class = {
		0 /* nothing */
	},
};

WidgetClass ctrlTextFieldWidgetClass = (WidgetClass)&ctrlTextFieldClassRec;

static void
Initialize(Widget rw, Widget nw, ArgList args, Cardinal *nargs)
{
	CtrlTextFieldWidget reqtf, newtf;
	XtAppContext app;
	String origvalue;
	Dimension thickness;

	(void)args;
	(void)nargs;
	app = XtWidgetToApplicationContext(nw);
	reqtf = (CtrlTextFieldWidget)rw;;
	newtf = (CtrlTextFieldWidget)nw;;
	origvalue = newtf->text.value;
	newtf->primitive.focusable = TRUE;
	newtf->primitive.pressed = TRUE;
	newtf->primitive.is3d = TRUE;
	newtf->text.blink_on = TRUE;
	newtf->text.has_focus = FALSE;
	newtf->text.overstrike = FALSE;
	newtf->text.under_preedit = FALSE;
	newtf->text.selection_move = FALSE;
	newtf->text.has_primary_selection = FALSE;
	newtf->text.has_clipboard_selection = FALSE;
	newtf->text.has_destination_selection = FALSE;
	newtf->text.selection_position = 0;
	newtf->text.preedit_position = 0;
	newtf->text.first_visible = 0;
	newtf->text.preedit_start = 0;
	newtf->text.preedit_end = 0;
	newtf->text.last_time = 0;
	newtf->text.h_offset = 0;
	newtf->text.timer_id = 0;
	newtf->text.text_length = strlen(origvalue);
	newtf->text.cursor_position = newtf->text.text_length;
	newtf->text.text_size = MAX(newtf->text.text_length, DEF_TEXT_SIZE);
	newtf->text.value = XtMalloc(newtf->text.text_size);
	_CtrlGetFontMetrics(
		app,
		newtf->text.font,
		&newtf->text.font_average_width,
		&newtf->text.font_ascent,
		&newtf->text.font_descent,
		&newtf->text.font_height
	);
	thickness = 2 * (newtf->primitive.highlight_thickness + newtf->primitive.shadow_thickness);
	if (reqtf->core.width == 0)
		newtf->core.width = thickness + 2 * newtf->text.margin_width + newtf->text.columns * newtf->text.font_average_width;
	if (reqtf->core.height == 0)
		newtf->core.height = thickness + 2 * newtf->text.margin_height + newtf->text.font_height;
	snprintf(newtf->text.value, newtf->text.text_size, "%s", origvalue);
}

static void 
Realize(Widget w, XtValueMask *valuemask, XSetWindowAttributes *attrs)
{
	XtRealizeProc realize;

	realize = ctrlTextFieldWidgetClass->core_class.superclass->core_class.realize;
	(*realize)(w, valuemask, attrs);
	(void)_CtrlGetInputContext(
		XtDisplay(w), w,
		PreeditStart,
		PreeditDone,
		PreeditDraw,
		PreeditCaret,
		PreeditDestroy
	);
}

static void
Destroy(Widget w)
{
	CtrlTextFieldWidget textw;

	textw = (CtrlTextFieldWidget)w;
	XtFree(textw->text.value);
}

static void
Resize(Widget w)
{
	XtWidgetProc resize;
	CtrlTextFieldWidget textw;
	Dimension new_width, text_width;
	Position offset, padding;

	resize = ctrlTextFieldWidgetClass->core_class.superclass->core_class.resize;
	(*resize)(w);
	textw = (CtrlTextFieldWidget)w;
	padding = textw->text.margin_width + textw->primitive.shadow_thickness + textw->primitive.shadow_thickness;
	text_width = _CtrlGetTextWidth(textw->text.font, textw->text.value, textw->text.text_length);
	new_width = textw->core.width - 2 * padding;
	offset = textw->text.h_offset - padding;
	if (text_width - new_width < -offset) {
		textw->text.h_offset = padding;
		if (text_width - new_width >= 0) {
			textw->text.h_offset += new_width - text_width;
		}
	}
	AdjustText(textw, textw->text.cursor_position);
	Draw(w);
}

static XtGeometryResult
QueryGeometry(Widget w, XtWidgetGeometry *intended, XtWidgetGeometry *desired)
{
	CtrlTextFieldWidget textw;
	Dimension thickness, width;

	textw = (CtrlTextFieldWidget)w;
	thickness = 2 * (textw->primitive.highlight_thickness + textw->primitive.shadow_thickness);
	width = thickness + 2 * textw->text.margin_width + textw->text.columns * textw->text.font_average_width;
	if (intended->request_mode & CWWidth)
		desired->width = MAX(width, intended->width);
	desired->height = thickness + 2 * textw->text.margin_height + textw->text.font_height;
	return _CtrlReplyToQueryGeometry(w, intended, desired);
}

static Boolean
SetValues(Widget cw, Widget rw, Widget nw, ArgList args, Cardinal *nargs)
{
	CtrlTextFieldWidget oldw, neww;
	Boolean redisplay;

	(void)rw;
	(void)args;
	(void)nargs;
	redisplay = FALSE;
	oldw = (CtrlTextFieldWidget)cw;
	neww = (CtrlTextFieldWidget)nw;

	if (neww->core.being_destroyed)
		return FALSE;
#warning TODO: write SetValues
	if (neww->core.width == 0)
		neww->core.width = oldw->core.width;
	if (neww->core.height == 0)
		neww->core.height = oldw->core.height;
	return redisplay;
}

static void
Draw(Widget w)
{
	XtWidgetProc draw;

	draw = ((CtrlPrimitiveWidgetClass)ctrlPrimitiveWidgetClass)->primitive_class.draw;
	(*draw)(w);
}

static void
InsertChar(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	(void)w;
	(void)ev;
	(void)params;
	(void)nparams;
	printf("ASDA\n");
}

static int
PreeditStart(XIC xic, XPointer client_data, XPointer call_data)
{
	(void)xic;
	(void)client_data;
	(void)call_data;
	return -1;
#warning TODO: write preedit callback functions
}

static int
PreeditDone(XIC xic, XPointer client_data, XPointer call_data)
{
	(void)xic;
	(void)client_data;
	(void)call_data;
	return -1;
#warning TODO: write preedit callback functions
}

static int
PreeditDraw(XIC xic, XPointer client_data, XPointer call_data)
{
	(void)xic;
	(void)client_data;
	(void)call_data;
	return -1;
#warning TODO: write preedit callback functions
}

static int
PreeditCaret(XIC xic, XPointer client_data, XPointer call_data)
{
	(void)xic;
	(void)client_data;
	(void)call_data;
	return -1;
#warning TODO: write preedit callback functions
}

static int
PreeditDestroy(XIC xic, XPointer client_data, XPointer call_data)
{
	(void)xic;
	(void)client_data;
	(void)call_data;
	return -1;
#warning TODO: write preedit callback functions
}

/* update textw->text.h_offset for character at cursor_position to be visible */
static void
AdjustText(CtrlTextFieldWidget textw, Cardinal cursor_position)
{
	Position left, margin, diff;

	margin = textw->text.margin_width + textw->primitive.shadow_thickness + textw->primitive.highlight_thickness;
	left = _CtrlGetTextWidth(textw->text.font, textw->text.value, cursor_position) + textw->text.h_offset;
	if ((diff = left - margin) < 0) {                               /* scroll text to the right */
		textw->text.h_offset -= diff;
	} else if ((diff = left - textw->core.width + margin) > 0) {    /* scroll text to the left */
		textw->text.h_offset -= diff;
	}
}
