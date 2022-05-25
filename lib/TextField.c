#include <stdio.h>
#include <string.h>

#include <control/TextFieldP.h>

#include "ControlI.h"

/* core methods */
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Realize(Widget, XtValueMask *, XSetWindowAttributes *);
static void Destroy(Widget);

/* preedit callback functions */
static int PreeditStart(XIC, XPointer, XPointer);
static int PreeditDone(XIC, XPointer, XPointer);
static int PreeditDraw(XIC, XPointer, XPointer);
static int PreeditCaret(XIC, XPointer, XPointer);
static int PreeditDestroy(XIC, XPointer, XPointer);

static XtActionsRec actions[] = {
#warning TODO: Fill in TextField actions[]
};

static XtResource resources[] = {
	{
		.resource_name   = CtrlNfont,
		.resource_class  = CtrlCFont,
		.resource_type   = CtrlRXftFont,
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

		.set_values_almost      = XtInheritSetValuesAlmost,
		.get_values_hook        = NULL,
		.accept_focus           = NULL,
		.version                = XtVersion,
		.callback_private       = NULL,
		.tm_table               = NULL,
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
		.activate               = NULL,
		.translations           = NULL,
	},
	.text_class = {
	},
};

WidgetClass ctrlTextFieldWidgetClass = (WidgetClass)&ctrlTextFieldClassRec;

static void
Initialize(Widget rw, Widget nw, ArgList args, Cardinal *nargs)
{
	CtrlTextFieldWidget reqtf, newtf;
	Dimension width, height;
	XtAppContext app;
	String origvalue;

	(void)args;
	(void)nargs;
	app = XtWidgetToApplicationContext(nw);
	reqtf = (CtrlTextFieldWidget)rw;;
	newtf = (CtrlTextFieldWidget)nw;;
	origvalue = newtf->text.value;
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
	newtf->text.timer_id = 0;
	newtf->text.text_length = strlen(origvalue);
	newtf->text.cursor_position = newtf->text.text_length;
	newtf->text.text_size = MAX(newtf->text.text_length, DEF_TEXT_SIZE);
	newtf->text.value = XtMalloc(newtf->text.text_size);
	snprintf(newtf->text.value, newtf->text.text_size, "%s", origvalue);
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
Realize(Widget w, XtValueMask *valuemask, XSetWindowAttributes *attrs)
{
	XtRealizeProc realize;

	realize = ctrlTextFieldWidgetClass->core_class.superclass->core_class.realize;
	(*realize)(w, valuemask, attrs);
}

static void
Destroy(Widget w)
{
	CtrlTextFieldWidget textw;

	textw = (CtrlTextFieldWidget)w;
#warning TODO: implement TextField destroy function
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
