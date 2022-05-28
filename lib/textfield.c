#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <control_private.h>

#include "common.h"

#define HALFTHICKNESS(w) (((CtrlPrimitiveWidget)(w))->primitive.highlight_thickness + ((CtrlPrimitiveWidget)(w))->primitive.shadow_thickness)
#define THICKNESS(w)     (2 * HALFTHICKNESS(w))
#define INPUTSIZE        1024

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
static void BeginningOfLine(Widget, XEvent *, String *, Cardinal *);
static void EndOfLine(Widget, XEvent *, String *, Cardinal *);
static void BackwardCharacter(Widget, XEvent *, String *, Cardinal *);
static void ForwardCharacter(Widget, XEvent *, String *, Cardinal *);
static void DeletePrevChar(Widget, XEvent *, String *, Cardinal *);
static void DeleteNextChar(Widget, XEvent *, String *, Cardinal *);
static void DeleteToBeginning(Widget, XEvent *, String *, Cardinal *);
static void DeleteToEnd(Widget, XEvent *, String *, Cardinal *);

/* preedit callback functions */
static int PreeditStart(XIC, XPointer, XPointer);
static int PreeditDone(XIC, XPointer, XPointer);
static int PreeditDraw(XIC, XPointer, XPointer);
static int PreeditCaret(XIC, XPointer, XPointer);
static int PreeditDestroy(XIC, XPointer, XPointer);

/* helper widget-internal functions */
static void AdjustText(CtrlTextFieldWidget, Cardinal);
static void Insert(CtrlTextFieldWidget, String, int);
static void SetCursor(Widget, Position, Boolean);
static void Redraw(Widget);
static Boolean DeleteSelection(Widget);

char translations[] =
"~s c <Key>A:                   beginning-of-line()\n"
"~s ~c <Key>Home:               beginning-of-line()\n"
"~s ~c <Key>KP_Home:            beginning-of-line()\n"
"s c <Key>A:                    beginning-of-line(extend)\n"
"s ~c <Key>Home:                beginning-of-line(extend)\n"
"s ~c <Key>KP_Home:             beginning-of-line(extend)\n"
"~s c <Key>B:                   backward-character()\n"
"~s ~c <Key>Left:               backward-character()\n"
"~s ~c <Key>KP_Left:            backward-character()\n"
"s c <Key>B:                    backward-character(extend)\n"
"s ~c <Key>Left:                backward-character(extend)\n"
"s ~c <Key>KP_Left:             backward-character(extend)\n"
"c <Key>C:                      copy-clipboard()\n"
"c <Key>D:                      delete-next-character()\n"
"~c <Key>Delete:                delete-next-character()\n"
"~s c <Key>E:                   end-of-line()\n"
"~s ~c <Key>End:                end-of-line()\n"
"~s ~c <Key>KP_End:             end-of-line()\n"
"s c <Key>E:                    end-of-line(extend)\n"
"s ~c <Key>End:                 end-of-line(extend)\n"
"s ~c <Key>KP_End:              end-of-line(extend)\n"
"~s c <Key>F:                   forward-character()\n"
"~s ~c <Key>Right:              forward-character()\n"
"~s ~c <Key>KP_Right:           forward-character()\n"
"s c <Key>F:                    forward-character(extend)\n"
"s ~c <Key>Right:               forward-character(extend)\n"
"s ~c <Key>KP_Right:            forward-character(extend)\n"
"c <Key>H:                      delete-previous-character()\n"
"~c <Key>BackSpace:             delete-previous-character()\n"
"c <Key>K:                      kill-to-end-of-line()\n"
"c <Key>U:                      kill-to-beginning-of-line()\n"
"c <Key>V:                      paste-clipboard()\n"
"c <Key>W:                      backward-kill-word()\n"
"c <Key>BackSpace:              backward-kill-word()\n"
"c <Key>Delete:                 forward-kill-word()\n"
"~s c <Key>Z:                   undo()\n"
"s c <Key>Z:                    redo()\n"
"~s c <Key>Left:                backward-word()\n"
"~s c <Key>KP_Left:             backward-word()\n"
"s c <Key>Left:                 backward-word(extend)\n"
"s c <Key>KP_Left:              backward-word(extend)\n"
"~s c <Key>Right:               forward-word()\n"
"~s c <Key>KP_Right:            forward-word()\n"
"s c <Key>Right:                forward-word(extend)\n"
"s c <Key>KP_Right:             forward-word(extend)\n"
"<Key>Return:                   activate()\n"
"<Key>KP_Enter:                 activate()\n"
"<Btn1Down>:                    select-start()\n"
"<Btn1Motion>:                  extend-adjust()\n"
"<Btn1Up>:                      extend-end()\n"
"<Btn2Down>:                    paste-primary()\n"
"<Btn3Down>:                    extend-start()\n"
"<Btn3Up>:                      extend-start()\n"
"<Btn3Up>:                      extend-end()\n"
//"<Unmap>:                       unmap()\n"
//"<Enter>:                       enter-window()\n"
//"<Leave>:                       leave-window()\n"
//"<EnterWindow>:                 enter-window()\n"
//"<LeaveWindow>:                 leave-window()\n"
//"<FocusIn>:                     focus-in()\n"
//"<FocusOut>:                    focus-out()\n"
"<Key>:                         insert-char()\n"
;

static XtActionsRec actions[] = {
	/* text replacing bindings */
	{"insert-char",                 InsertChar},
	{"beginning-of-line",           BeginningOfLine},
	{"end-of-line",                 EndOfLine},
	{"backward-character",          BackwardCharacter},
	{"forward-character",           ForwardCharacter},
	{"delete-previous-character",   DeletePrevChar},
	{"delete-next-character",       DeleteNextChar},
	{"kill-to-beginning-of-line",   DeleteToBeginning},
	{"kill-to-end-of-line",         DeleteToEnd},
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
		.resource_name   = CtrlNselbackground,
		.resource_class  = CtrlCSelbackground,
		.resource_type   = CtrlRPixel,
		.resource_size   = sizeof(Pixel),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.selbackground),
		.default_type    = CtrlRString,
		.default_addr    = (XtPointer)DEF_SELBACKGROUND,
	},
	{
		.resource_name   = CtrlNselforeground,
		.resource_class  = CtrlCSelforeground,
		.resource_type   = CtrlRXftColor,
		.resource_size   = sizeof(XtPointer),
		.resource_offset = XtOffsetOf(CtrlTextFieldRec, text.selforeground),
		.default_type    = CtrlRString,
		.default_addr    = (XtPointer)DEF_SELFOREGROUND,
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
		.tm_table               = translations,
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
	String origvalue;

	(void)args;
	(void)nargs;
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
	newtf->text.caret_position = 0;
	newtf->text.preedit_position = 0;
	newtf->text.preedit_start = 0;
	newtf->text.preedit_end = 0;
	newtf->text.last_time = 0;
	newtf->text.h_offset = 0;
	newtf->text.timer_id = 0;
	newtf->text.text_length = strlen(origvalue);
	newtf->text.selection_position = newtf->text.cursor_position = newtf->text.text_length;
	newtf->text.text_size = MAX(newtf->text.text_length, INPUTSIZE);
	newtf->text.value = XtMalloc(newtf->text.text_size);
	if (reqtf->core.width == 0)
		newtf->core.width = THICKNESS(nw) + 2 * newtf->primitive.margin_width + newtf->text.columns * newtf->primitive.font_average_width;
	if (reqtf->core.height == 0)
		newtf->core.height = THICKNESS(nw) + 2 * newtf->primitive.margin_height + newtf->primitive.font_height;
	snprintf(newtf->text.value, newtf->text.text_size, "%s", origvalue);
}

static void 
Realize(Widget w, XtValueMask *valuemask, XSetWindowAttributes *attrs)
{
	CtrlTextFieldWidget textw;

	textw = (CtrlTextFieldWidget)w;
	(*ctrlPrimitiveWidgetClass->core_class.realize)(w, valuemask, attrs);
	textw->text.xic = _CtrlGetInputContext(
		w,
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
	CtrlTextFieldWidget textw;
	Dimension new_width, text_width;
	Position offset, padding;

	textw = (CtrlTextFieldWidget)w;
	padding = textw->primitive.margin_width + textw->primitive.shadow_thickness + textw->primitive.shadow_thickness;
	text_width = _CtrlGetTextWidth(textw->primitive.font, textw->text.value, textw->text.text_length);
	new_width = textw->core.width - 2 * padding;
	offset = textw->text.h_offset - padding;
	if (text_width - new_width < -offset) {
		textw->text.h_offset = padding;
		if (text_width - new_width >= 0) {
			textw->text.h_offset += new_width - text_width;
		}
	}
	AdjustText(textw, textw->text.cursor_position);
	(*ctrlPrimitiveWidgetClass->core_class.resize)(w);
}

static XtGeometryResult
QueryGeometry(Widget w, XtWidgetGeometry *intended, XtWidgetGeometry *desired)
{
	CtrlTextFieldWidget textw;
	Dimension width;

	textw = (CtrlTextFieldWidget)w;
	width = THICKNESS(w) + 2 * textw->primitive.margin_width + textw->text.columns * textw->primitive.font_average_width;
	if (intended->request_mode & CWWidth)
		desired->width = MAX(width, intended->width);
	desired->height = THICKNESS(w) + 2 * textw->primitive.margin_height + textw->primitive.font_height;
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
	CtrlTextFieldWidget textw;
	XtWidgetProc draw;
	Position minpos, maxpos, x, y;
	Dimension width, widthpre;

	textw = (CtrlTextFieldWidget)w;
	minpos = MIN(textw->text.cursor_position, textw->text.selection_position);
	maxpos = MAX(textw->text.cursor_position, textw->text.selection_position);
	x = HALFTHICKNESS(w) + textw->primitive.margin_width + textw->text.h_offset;
	y = HALFTHICKNESS(w) + textw->primitive.margin_width;
	widthpre = 0;

	/* draw background */
	_CtrlDrawRectangle(
		XtDisplay(w),
		textw->primitive.pixsave,
		textw->core.background_pixmap,
		textw->core.background_pixel,
		0, 0,
		textw->core.width,
		textw->core.height
	);

	/* draw text before selection */
	if (minpos > 0) {
		width = _CtrlGetTextWidth(textw->primitive.font, textw->text.value, minpos);
		_CtrlDrawText(
			XtDisplay(w),
			textw->primitive.pixsave,
			textw->primitive.font,
			textw->primitive.foreground,
			x, y,
			textw->text.value,
			minpos
		);
		x += width;
		widthpre = width;
	}

	/* draw selected text or pre-edited text */
	if (textw->text.under_preedit) {
#warning TODO: draww preediting text
	} else if (maxpos > minpos) {
		width = _CtrlGetTextWidth(textw->primitive.font, textw->text.value + minpos, maxpos - minpos);
		_CtrlDrawRectangle(
			XtDisplay(w),
			textw->primitive.pixsave,
			None,
			textw->text.selbackground,
			x, y,
			width,
			textw->primitive.font_height
		);
		_CtrlDrawText(
			XtDisplay(w),
			textw->primitive.pixsave,
			textw->primitive.font,
			textw->primitive.foreground,
			x, y,
			textw->text.value + minpos,
			maxpos - minpos
		);
		x += width;
	}

	/* draw text after selection */
	_CtrlDrawText(
		XtDisplay(w),
		textw->primitive.pixsave,
		textw->primitive.font,
		textw->primitive.foreground,
		x, y,
		textw->text.value + maxpos,
		strlen(textw->text.value + maxpos)
	);

	/* draw I-beam (cursor line) */
	if (textw->text.cursor_position == textw->text.selection_position) {
		if (textw->text.under_preedit && textw->text.caret_position > 0) {
#warning TODO: get width of preedit text until caret
		} else {
			width = 0;
		}
		_CtrlDrawXftRectangle(
			XtDisplay(w),
			textw->primitive.pixsave,
			textw->text.selforeground,
			HALFTHICKNESS(w) + textw->primitive.margin_width + textw->text.h_offset + widthpre + width,
			y, 1,
			textw->primitive.font_height
		);
	}

	draw = ((CtrlPrimitiveWidgetClass)ctrlPrimitiveWidgetClass)->primitive_class.draw;
	(*draw)(w);
}

static void
InsertChar(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;
	CtrlGenericCallData cd;
	Status status;
	char buf[INPUTSIZE];
	int len;

	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	status = _CtrlLookupString(XtDisplay(w), textw->text.xic, ev, buf, sizeof(buf) - 1, &len);
	if (status != XLookupChars && status != XLookupBoth) {
		/*
		 * We got XLookupNone, XBufferOverflow, or XLookupKeySym.
		 * In any case, we could not compose input characters.
		 */
		return;
	}
	if (iscntrl(*buf) || *buf == '\0') {
		return;
	}
#warning TODO: Call modify_verify_callback
	DeleteSelection(w);
	Insert(textw, buf, len);
	cd = (CtrlGenericCallData){
		.reason = CTRL_VALUE_CHANGED,
		.event = ev,
	};
	AdjustText(textw, textw->text.cursor_position);
	XtCallCallbackList(w, textw->text.value_changed_callback, (XtPointer)&cd);
	Redraw(w);
}

static void
BeginningOfLine(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)ev;
	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	SetCursor(w, 0, *nparams > 0);
	AdjustText(textw, textw->text.cursor_position);
	Redraw(w);
}

static void
EndOfLine(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)ev;
	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	if (textw->text.value[textw->text.cursor_position] != '\0')
		SetCursor(w, textw->text.text_length, *nparams > 0);
	AdjustText(textw, textw->text.cursor_position);
	Redraw(w);
}

static void
BackwardCharacter(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)ev;
	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	if (textw->text.cursor_position > 0)
		SetCursor(w, _CtrlNextRune(textw->text.value, textw->text.cursor_position, -1), *nparams > 0);
	AdjustText(textw, textw->text.cursor_position);
	Redraw(w);
}

static void
ForwardCharacter(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)ev;
	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	if (textw->text.value[textw->text.cursor_position] != '\0')
		SetCursor(w, _CtrlNextRune(textw->text.value, textw->text.cursor_position, +1), *nparams > 0);
	AdjustText(textw, textw->text.cursor_position);
	Redraw(w);
}

static void
DeletePrevChar(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)ev;
	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
#warning TODO: Call modify_verify_callback
	if (!DeleteSelection(w)) {
		if (textw->text.cursor_position > 0) {
			Insert(textw, NULL, _CtrlNextRune(textw->text.value, textw->text.cursor_position, -1) - textw->text.cursor_position);
		}
	}
	AdjustText(textw, textw->text.cursor_position);
	Redraw(w);
}

static void
DeleteNextChar(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)ev;
	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
#warning TODO: Call modify_verify_callback
	if (!DeleteSelection(w)) {
		if (textw->text.value[textw->text.cursor_position] != '\0') {
			textw->text.cursor_position = _CtrlNextRune(textw->text.value, textw->text.cursor_position, +1);
		}
		if (textw->text.cursor_position > 0) {
			Insert(textw, NULL, _CtrlNextRune(textw->text.value, textw->text.cursor_position, -1) - textw->text.cursor_position);
		}
	}
	AdjustText(textw, textw->text.cursor_position);
	Redraw(w);
}

static void
DeleteToBeginning(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)ev;
	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
#warning TODO: Call modify_verify_callback
	if (!DeleteSelection(w))
		Insert(textw, NULL, 0 - textw->text.cursor_position);
	AdjustText(textw, textw->text.cursor_position);
	Redraw(w);
}

static void
DeleteToEnd(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)ev;
	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
#warning TODO: Call modify_verify_callback
	if (!DeleteSelection(w))
		textw->text.value[textw->text.cursor_position] = '\0';
	AdjustText(textw, textw->text.cursor_position);
	Redraw(w);
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

	margin = textw->primitive.margin_width + textw->primitive.shadow_thickness + textw->primitive.highlight_thickness;
	left = _CtrlGetTextWidth(textw->primitive.font, textw->text.value, cursor_position) + textw->text.h_offset + textw->primitive.font_average_width;
	if ((diff = left - margin) < 0) {                               /* scroll text to the right */
		textw->text.h_offset -= diff;
	} else if ((diff = left - textw->core.width + margin) > 0) {    /* scroll text to the left */
		textw->text.h_offset -= diff;
	}
}

static void
Insert(CtrlTextFieldWidget textw, String buf, int len)
{
	/*
	 * We move existing text out of the way, insert new text,
	 * and update the cursor position and text length.  We may
	 * reallocate textw's value.
	 */
	if (textw->text.text_length + len > textw->text.text_size - 1) {
		textw->text.text_size += MAX(len, INPUTSIZE);
		textw->text.value = XtRealloc(textw->text.value, textw->text.text_size);
	}
	memmove(
		&textw->text.value[textw->text.cursor_position + len],
		&textw->text.value[textw->text.cursor_position],
	        textw->text.text_length - textw->text.cursor_position + 1
	);
	if (len > 0) {
		memcpy(&textw->text.value[textw->text.cursor_position], buf, len);
	}
	textw->text.text_length += len;
	textw->text.cursor_position += len;
	textw->text.selection_position = textw->text.cursor_position;
}

static void
SetCursor(Widget w, Position pos, Boolean select)
{
	CtrlTextFieldWidget textw;

	textw = (CtrlTextFieldWidget)w;
#warning TODO: Call motion_verify_callback
	textw->text.cursor_position = pos;
	if (select) {
#warning TODO: Primary selection
	} else {
		textw->text.selection_position = textw->text.cursor_position;
	}
}

static void
Redraw(Widget w)
{
	CtrlTextFieldWidget textw;

	textw = (CtrlTextFieldWidget)w;
	Draw(w);
	_CtrlCommitPixmap(
		XtDisplay(w),
		XtWindow(w),
		textw->primitive.pixsave,
		textw->core.x,
		textw->core.y,
		textw->core.width,
		textw->core.height
	);
}

static Boolean
DeleteSelection(Widget w)
{
	CtrlTextFieldWidget textw;
	Position minpos, maxpos;

	textw = (CtrlTextFieldWidget)w;
	if (textw->text.selection_position == textw->text.cursor_position)
		return FALSE;
	minpos = MIN(textw->text.cursor_position, textw->text.selection_position);
	maxpos = MAX(textw->text.cursor_position, textw->text.selection_position);
	memmove(textw->text.value + minpos, textw->text.value + maxpos, textw->text.text_length - maxpos + 1);
	textw->text.cursor_position = textw->text.selection_position = minpos;
	return TRUE;
}
