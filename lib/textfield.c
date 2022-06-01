#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <control_private.h>

#include "common.h"

#define HALFTHICKNESS(w) (((CtrlPrimitiveWidget)(w))->primitive.highlight_thickness + ((CtrlPrimitiveWidget)(w))->primitive.shadow_thickness)
#define THICKNESS(w)     (2 * HALFTHICKNESS(w))
#define INPUTSIZE        1024

enum {
	MODIFY_NONE,
	MODIFY_INSERTION,
	MODIFY_DELETION,
};

struct Undo {
	struct Undo *prev, *next;
	String text;
	int length;
};

static struct Undo dummy_undo = {
	/*
	 * The last entry of the undo_list is a dummy entry with .text
	 * set to NULL, we use it to know whether we are at the end of
	 * the undo list.
	 */
	.text = NULL,
	.prev = NULL,
	.next = NULL,
};

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
static void Activate(Widget, XEvent *, String *, Cardinal *);
static void SelectNothing(Widget, XEvent *, String *, Cardinal *);
static void SelectAll(Widget, XEvent *, String *, Cardinal *);
static void InsertChar(Widget, XEvent *, String *, Cardinal *);
static void BeginningOfLine(Widget, XEvent *, String *, Cardinal *);
static void EndOfLine(Widget, XEvent *, String *, Cardinal *);
static void Extend(Widget, XEvent *, String *, Cardinal *);
static void BackwardCharacter(Widget, XEvent *, String *, Cardinal *);
static void BackwardWord(Widget, XEvent *, String *, Cardinal *);
static void CopyClipboard(Widget, XEvent *, String *, Cardinal *);
static void ForwardCharacter(Widget, XEvent *, String *, Cardinal *);
static void ForwardWord(Widget, XEvent *, String *, Cardinal *);
static void DeletePrevChar(Widget, XEvent *, String *, Cardinal *);
static void DeleteNextChar(Widget, XEvent *, String *, Cardinal *);
static void DeleteToBeginning(Widget, XEvent *, String *, Cardinal *);
static void DeleteToEnd(Widget, XEvent *, String *, Cardinal *);
static void DeleteWordBackwards(Widget, XEvent *, String *, Cardinal *);
static void DeleteWordForwards(Widget, XEvent *, String *, Cardinal *);
static void PasteClipboard(Widget, XEvent *, String *, Cardinal *);
static void PastePrimary(Widget, XEvent *, String *, Cardinal *);
static void Redo(Widget, XEvent *, String *, Cardinal *);
static void SelectStart(Widget, XEvent *, String *, Cardinal *);
static void SelectAdjust(Widget, XEvent *, String *, Cardinal *);
static void SelectEnd(Widget, XEvent *, String *, Cardinal *);
static void Undo(Widget, XEvent *, String *, Cardinal *);

/* preedit callback functions */
static int PreeditStart(XIC, XPointer, XPointer);
static int PreeditDone(XIC, XPointer, XPointer);
static int PreeditDraw(XIC, XPointer, XPointer);
static int PreeditCaret(XIC, XPointer, XPointer);
static int PreeditDestroy(XIC, XPointer, XPointer);

/* helper internal functions */
static void InitiateSelection(Widget, Time, Atom, String *, int *);
static void ValueChanged(Widget, XEvent *);
static void AdjustText(Widget);
static void Insert(CtrlTextFieldWidget, String, int);
static void SetCursor(Widget, Time, int, Boolean, Boolean);
static void Redraw(Widget);
static Boolean DeleteSelection(Widget);
static Boolean Copy(Widget, Atom *, Atom *, Atom *, XtPointer *, unsigned long *, int *);
static void Paste(Widget, XtPointer, Atom *, Atom *, XtPointer, unsigned long *, int *);
static int GetCursorPosition(Widget, int);
static void AddUndo(Widget, Boolean);

char translations[] =
"<Key>Escape:                   select-nothing()\n"
"~s c <Key>A:                   select-all()\n"
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
"<Btn1Motion>:                  select-adjust()\n"
"<Btn1Up>:                      select-end()\n"
"<Btn3Up>:                      select-end()\n"
"<Btn2Down>:                    paste-primary()\n"
"<Btn3Down>:                    extend()\n"
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
	{"activate",                    Activate},
	{"backward-character",          BackwardCharacter},
	{"backward-kill-word",          DeleteWordBackwards},
	{"backward-word",               BackwardWord},
	{"beginning-of-line",           BeginningOfLine},
	{"copy-clipboard",              CopyClipboard},
	{"delete-next-character",       DeleteNextChar},
	{"delete-previous-character",   DeletePrevChar},
	{"end-of-line",                 EndOfLine},
	{"extend",                      Extend},
	{"forward-character",           ForwardCharacter},
	{"forward-kill-word",           DeleteWordForwards},
	{"forward-word",                ForwardWord},
	{"insert-char",                 InsertChar},
	{"kill-to-beginning-of-line",   DeleteToBeginning},
	{"kill-to-end-of-line",         DeleteToEnd},
	{"paste-clipboard",             PasteClipboard},
	{"paste-primary",               PastePrimary},
	{"redo",                        Redo},
	{"select-all",                  SelectAll},
	{"select-nothing",              SelectNothing},
	{"select-start",                SelectStart},
	{"select-adjust",               SelectAdjust},
	{"select-end",                  SelectEnd},
	{"undo",                        Undo},
};

static XtResource resources[] = {
	{
		.resource_name   = CtrlNcolumns,
		.resource_class  = CtrlCColumns,
		.resource_type   = CtrlRDimension,
		.resource_size   = sizeof(Dimension),
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
		.resource_name   = CtrlNisTabGroup,
		.resource_class  = CtrlCIsTabGroup,
		.resource_type   = CtrlRBoolean,
		.resource_size   = sizeof(Boolean),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.is_tab_group),
		.default_type    = CtrlRImmediate,
		.default_addr    = (XtPointer)TRUE,
	},
	{
		.resource_name   = CtrlNcursor,
		.resource_class  = CtrlCCursor,
		.resource_type   = CtrlRCursor,
		.resource_size   = sizeof(Cursor),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.cursor),
		.default_type    = CtrlRString,
		.default_addr    = "xterm",
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
	newtf->text.last_modify = MODIFY_NONE;
	newtf->text.overstrike = FALSE;
	newtf->text.select_word = FALSE;
	newtf->text.under_preedit = FALSE;
	newtf->text.caret_position = 0;
	newtf->text.last_time = 0;
	newtf->text.h_offset = 0;
	newtf->text.preedit_value = NULL;
	newtf->text.clipboard_value = NULL;
	newtf->text.clipboard_size = 0;
	newtf->text.primary_value = NULL;
	newtf->text.primary_size = 0;
	newtf->text.preedit_size = 0;
	newtf->text.preedit_length = 0;
	newtf->text.text_length = strlen(origvalue);
	newtf->text.selection_position = newtf->text.cursor_position = newtf->text.text_length;
	newtf->text.text_size = MAX(newtf->text.text_length, INPUTSIZE);
	newtf->text.value = XtMalloc(newtf->text.text_size);
	if (reqtf->core.width == 0)
		newtf->core.width = THICKNESS(nw) + 2 * newtf->primitive.margin_width + newtf->text.columns * newtf->primitive.font_average_width;
	if (reqtf->core.height == 0)
		newtf->core.height = THICKNESS(nw) + 2 * newtf->primitive.margin_height + newtf->primitive.font_height;
	snprintf(newtf->text.value, newtf->text.text_size, "%s", origvalue);
	newtf->text.undo_list = (XtPointer)&dummy_undo;
	newtf->text.undo_current = NULL;
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
	struct Undo *undo, *tmp;

	textw = (CtrlTextFieldWidget)w;
	FREE(textw->text.value);
	FREE(textw->text.preedit_value);
	FREE(textw->text.clipboard_value);
	FREE(textw->text.primary_value);
	undo = (struct Undo *)textw->text.undo_list;
	while (undo) {
		tmp = undo;
		undo = undo->next;
		if (tmp->text != NULL) {
			FREE(tmp->text);
			FREE(tmp);
		}
	}
}

static void
Resize(Widget w)
{
	CtrlTextFieldWidget textw;
	Dimension new_width, text_width;
	int offset, padding;

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
	AdjustText(w);
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
	CtrlTextFieldWidget oldtf, newtf;
	Boolean redraw;
	Dimension width, height;

	(void)rw;
	(void)args;
	(void)nargs;
	redraw = FALSE;
	oldtf = (CtrlTextFieldWidget)cw;
	newtf = (CtrlTextFieldWidget)nw;

	if (newtf->core.being_destroyed)
		return FALSE;
	if (newtf->text.value != oldtf->text.value) {
		/* We select everything and then insert the new value */
		newtf->text.selection_position = 0;
		newtf->text.cursor_position = newtf->text.text_length;
		Insert(newtf, oldtf->text.value, strlen(oldtf->text.value));
		FREE(oldtf->text.value);
		ValueChanged(nw, NULL);
		redraw = TRUE;
	}
	if (newtf->text.selforeground != oldtf->text.selforeground ||
	    newtf->text.selbackground != oldtf->text.selbackground) {
		redraw = TRUE;
	}
	if (newtf->text.columns != oldtf->text.columns) {
		newtf->text.columns = oldtf->text.columns;
	}
	width = THICKNESS(nw) + 2 * newtf->primitive.margin_width + newtf->text.columns * newtf->primitive.font_average_width;
	height = THICKNESS(nw) + 2 * newtf->primitive.margin_height + newtf->primitive.font_height;
	if (newtf->core.width != width) {
		newtf->core.width = width;
		redraw = TRUE;
	}
	if (newtf->core.height != height) {
		newtf->core.height = height;
		redraw = TRUE;
	}
	if (redraw) {
		AdjustText(nw);
		Draw(nw);
	}
	return redraw;
}

static void
Draw(Widget w)
{
	CtrlTextFieldWidget textw;
	XtWidgetProc draw;
	int minpos, maxpos, x, y;
	Dimension width, widthpre;

	textw = (CtrlTextFieldWidget)w;
	minpos = MIN(textw->text.cursor_position, textw->text.selection_position);
	maxpos = MAX(textw->text.cursor_position, textw->text.selection_position);
	x = textw->text.h_offset;
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
		width = _CtrlGetTextWidth(textw->primitive.font, textw->text.preedit_value, textw->text.preedit_length);
		_CtrlDrawXftRectangle(
			XtDisplay(w),
			textw->primitive.pixsave,
			textw->primitive.foreground,
			x,
			y + textw->primitive.font_height,
			width,
			1
		);
		_CtrlDrawText(
			XtDisplay(w),
			textw->primitive.pixsave,
			textw->primitive.font,
			textw->primitive.foreground,
			x, y,
			textw->text.preedit_value,
			textw->text.preedit_length
		);
		x += width;
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
			width = _CtrlGetTextWidth(textw->primitive.font, textw->text.preedit_value, textw->text.caret_position);
		} else {
			width = 0;
		}
		_CtrlDrawXftRectangle(
			XtDisplay(w),
			textw->primitive.pixsave,
			textw->text.selforeground,
			textw->text.h_offset + widthpre + width,
			y, 1,
			textw->primitive.font_height
		);
	}

	draw = ((CtrlPrimitiveWidgetClass)ctrlPrimitiveWidgetClass)->primitive_class.draw;
	(*draw)(w);
}

static void
Activate(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlGenericCallData cd;
	CtrlTextFieldWidget textw;

	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	cd = (CtrlGenericCallData){
		.reason = CTRL_ACTIVATE,
		.event = ev,
	};
	XtCallCallbackList(w, textw->text.activate_callback, (XtPointer)&cd);
#warning TODO: if the parent is a manager, pass the event to the parent
}

static void
SelectNothing(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)ev;
	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	textw->text.selection_position = textw->text.cursor_position;
	Redraw(w);
}

static void
SelectAll(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	textw->text.selection_position = 0;
	SetCursor(w, ev->xkey.time, textw->text.text_length, TRUE, TRUE);
}

static void
InsertChar(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;
	Status status;
	char buf[INPUTSIZE];
	int len;

	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	if (textw->text.last_modify != MODIFY_INSERTION) {
		AddUndo(w, TRUE);
		textw->text.last_modify = MODIFY_INSERTION;
	}
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
	DeleteSelection(w);
	Insert(textw, buf, len);
	ValueChanged(w, ev);
	Redraw(w);
}

static void
BeginningOfLine(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	(void)params;
	SetCursor(w, ev->xkey.time, 0, *nparams > 0, TRUE);
}

static void
EndOfLine(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)params;
	textw = (CtrlTextFieldWidget)w;
	SetCursor(w, ev->xkey.time, textw->text.text_length, *nparams > 0, TRUE);
}

static void
Extend(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;
	int minpos, maxpos, pos;

	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	minpos = MIN(textw->text.cursor_position, textw->text.selection_position);
	maxpos = MAX(textw->text.cursor_position, textw->text.selection_position);
	pos = GetCursorPosition(w, ev->xbutton.x);
	if (pos < minpos)
		textw->text.selection_position = maxpos;
	if (pos > maxpos)
		textw->text.selection_position = minpos;
	SetCursor(w, ev->xbutton.time, pos, TRUE, TRUE);
}

static void
BackwardCharacter(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)params;
	textw = (CtrlTextFieldWidget)w;
	SetCursor(w, ev->xkey.time, _CtrlNextRune(textw->text.value, textw->text.cursor_position, -1), *nparams > 0, TRUE);
}

static void
BackwardWord(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)params;
	textw = (CtrlTextFieldWidget)w;
	SetCursor(w, ev->xkey.time, _CtrlMoveWordEdge(textw->text.value, textw->text.cursor_position, -1), *nparams > 0, TRUE);
}

static void
CopyClipboard(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	InitiateSelection(
		w,
		ev->xkey.time,
		_CtrlInternAtom(XtDisplay(w), CLIPBOARD),
		&textw->text.clipboard_value,
		&textw->text.clipboard_size
	);
}

static void
ForwardCharacter(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)params;
	textw = (CtrlTextFieldWidget)w;
	SetCursor(w, ev->xkey.time, _CtrlNextRune(textw->text.value, textw->text.cursor_position, +1), *nparams > 0, TRUE);
}

static void
ForwardWord(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)params;
	textw = (CtrlTextFieldWidget)w;
	SetCursor(w, ev->xkey.time, _CtrlMoveWordEdge(textw->text.value, textw->text.cursor_position, +1), *nparams > 0, TRUE);
}

static void
DeletePrevChar(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	if (textw->text.last_modify != MODIFY_DELETION) {
		AddUndo(w, TRUE);
		textw->text.last_modify = MODIFY_DELETION;
	}
	if (DeleteSelection(w))
		goto done;
	if (textw->text.cursor_position > 0)
		Insert(textw, NULL, _CtrlNextRune(textw->text.value, textw->text.cursor_position, -1) - textw->text.cursor_position);
done:
	Redraw(w);
	ValueChanged(w, ev);
}

static void
DeleteNextChar(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	if (textw->text.last_modify != MODIFY_DELETION) {
		AddUndo(w, TRUE);
		textw->text.last_modify = MODIFY_DELETION;
	}
	if (DeleteSelection(w))
		goto done;
	if (textw->text.value[textw->text.cursor_position] != '\0')
		textw->text.cursor_position = _CtrlNextRune(textw->text.value, textw->text.cursor_position, +1);
	if (textw->text.cursor_position > 0)
		Insert(textw, NULL, _CtrlNextRune(textw->text.value, textw->text.cursor_position, -1) - textw->text.cursor_position);
done:
	Redraw(w);
	ValueChanged(w, ev);
}

static void
DeleteToBeginning(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	if (textw->text.last_modify != MODIFY_DELETION) {
		AddUndo(w, TRUE);
		textw->text.last_modify = MODIFY_DELETION;
	}
	if (DeleteSelection(w))
		goto done;
	Insert(textw, NULL, 0 - textw->text.cursor_position);
done:
	Redraw(w);
	ValueChanged(w, ev);
}

static void
DeleteToEnd(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	if (textw->text.last_modify != MODIFY_DELETION) {
		AddUndo(w, TRUE);
		textw->text.last_modify = MODIFY_DELETION;
	}
	if (DeleteSelection(w))
		goto done;
	textw->text.value[textw->text.cursor_position] = '\0';
done:
	Redraw(w);
	ValueChanged(w, ev);
}

static void
DeleteWordBackwards(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;
	int pos;

	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	if (textw->text.last_modify != MODIFY_DELETION) {
		AddUndo(w, TRUE);
		textw->text.last_modify = MODIFY_DELETION;
	}
	if (DeleteSelection(w))
		goto done;
	while (textw->text.cursor_position > 0 && isspace((unsigned char)textw->text.value[(pos = _CtrlNextRune(textw->text.value, textw->text.cursor_position, -1))]))
		Insert(textw, NULL, pos - textw->text.cursor_position);
	while (textw->text.cursor_position > 0 && !isspace((unsigned char)textw->text.value[(pos = _CtrlNextRune(textw->text.value, textw->text.cursor_position, -1))]))
		Insert(textw, NULL, pos - textw->text.cursor_position);
done:
	Redraw(w);
	ValueChanged(w, ev);
}

static void
DeleteWordForwards(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;
	int pos, tmp;

	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	if (textw->text.last_modify != MODIFY_DELETION) {
		AddUndo(w, TRUE);
		textw->text.last_modify = MODIFY_DELETION;
	}
	if (DeleteSelection(w))
		goto done;
	while (textw->text.value[textw->text.cursor_position] != '\0' && !isspace((unsigned char)textw->text.value[(pos = _CtrlNextRune(textw->text.value, textw->text.cursor_position, +1))])) {
		tmp = pos;
		pos = textw->text.cursor_position;
		textw->text.cursor_position = tmp;
		Insert(textw, NULL, pos - textw->text.cursor_position);
	}
	while (textw->text.value[textw->text.cursor_position] != '\0' && isspace((unsigned char)textw->text.value[(pos = _CtrlNextRune(textw->text.value, textw->text.cursor_position, +1))])) {
		tmp = pos;
		pos = textw->text.cursor_position;
		textw->text.cursor_position = tmp;
		Insert(textw, NULL, pos - textw->text.cursor_position);
	}
done:
	Redraw(w);
	ValueChanged(w, ev);
}

static void
PasteClipboard(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	(void)params;
	(void)nparams;
	_CtrlGetSelection(
		w,
		_CtrlInternAtom(XtDisplay(w), CLIPBOARD),
		_CtrlInternAtom(XtDisplay(w), UTF8_STRING),
		Paste,
		ev->xkey.time
	);
}

static void
PastePrimary(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	(void)params;
	(void)nparams;
	_CtrlGetSelection(
		w,
		XA_PRIMARY,
		_CtrlInternAtom(XtDisplay(w), UTF8_STRING),
		Paste,
		ev->xkey.time
	);
}

static void
Redo(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;
	struct Undo *curr;

	(void)ev;
	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	//if (textw->text.last_modify != MODIFY_NONE) {
	//	AddUndo(w, FALSE);
	//	textw->text.last_modify = MODIFY_NONE;
	//}
	curr = (struct Undo *)textw->text.undo_current;
	if (curr != NULL && curr->prev != NULL)
		textw->text.undo_current = (XtPointer)curr->prev;
	curr = (struct Undo *)textw->text.undo_current;
	if (curr != NULL) {
		memcpy(textw->text.value, curr->text, curr->length);
		textw->text.text_length = curr->length;
		textw->text.value[curr->length] = '\0';
		textw->text.cursor_position = textw->text.text_length;
		textw->text.selection_position = textw->text.cursor_position;
		Redraw(w);
	}
}

static void
SelectStart(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;
	int pos;

	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	if (textw->text.under_preedit)  /* we ignore mouse events when compositing */
		return;
	if (ev->xbutton.time > textw->text.last_time &&
	    ev->xbutton.time - textw->text.last_time < (unsigned long)XtGetMultiClickTime(XtDisplay(w))) {
		if (textw->text.select_word) {
			textw->text.selection_position = 0;
			SetCursor(w, ev->xkey.time, textw->text.text_length, TRUE, TRUE);
			textw->text.select_word = FALSE;
		} else {
			pos = textw->text.cursor_position;
			SetCursor(
				w,
				ev->xkey.time,
				_CtrlMoveWordEdge(textw->text.value, pos, -1),
				FALSE,
				FALSE
			);
			SetCursor(
				w,
				ev->xkey.time,
				_CtrlMoveWordEdge(textw->text.value, pos, +1),
				TRUE,
				TRUE
			);
			textw->text.select_word = TRUE;
		}
	} else {
		SetCursor(
			w,
			ev->xbutton.time,
			GetCursorPosition(w, ev->xbutton.x),
			FALSE,
			TRUE
		);
		textw->text.select_word = FALSE;
	}
	textw->text.last_time = ev->xbutton.time;
}

static void
SelectAdjust(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;
	int prevcursor;

	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	prevcursor = textw->text.cursor_position;
	if (textw->text.under_preedit)          /* we ignore mouse events when compositing */
		return;
	textw->text.cursor_position = GetCursorPosition(w, ev->xmotion.x);
	if (textw->text.cursor_position == prevcursor)       /* no change, do not redraw */
		return;
	Redraw(w);
}

static void
SelectEnd(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;

	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	if (textw->text.cursor_position != textw->text.selection_position) {
		InitiateSelection(w, ev->xbutton.time, XA_PRIMARY, &textw->text.primary_value, &textw->text.primary_size);
	}
}

static void
Undo(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlTextFieldWidget textw;
	struct Undo *curr;

	(void)ev;
	(void)params;
	(void)nparams;
	textw = (CtrlTextFieldWidget)w;
	//if (textw->text.last_modify != MODIFY_NONE) {
	//	AddUndo(w, FALSE);
	//	textw->text.last_modify = MODIFY_NONE;
	//}
	curr = (struct Undo *)textw->text.undo_current;
	if (curr != NULL) {
		if (curr->text == NULL) {
			return;
		}
		textw->text.undo_current = (XtPointer)curr->next;
	}
	curr = (struct Undo *)textw->text.undo_current;
	if (curr != NULL) {
		memcpy(textw->text.value, curr->text, curr->length);
		textw->text.text_length = curr->length;
		textw->text.value[curr->length] = '\0';
		textw->text.cursor_position = textw->text.text_length;
		textw->text.selection_position = textw->text.cursor_position;
		Redraw(w);
	}
}

static int
PreeditStart(XIC xic, XPointer client_data, XPointer call_data)
{
	CtrlTextFieldWidget textw;

	(void)xic;
	(void)call_data;
	textw = (CtrlTextFieldWidget)client_data;
	textw->text.under_preedit = TRUE;
	textw->text.preedit_size = INPUTSIZE;
	textw->text.preedit_value = XtMalloc(textw->text.preedit_size);
	textw->text.preedit_value[0] = '\0';
	textw->text.preedit_length = 0;
	return INPUTSIZE;
}

static int
PreeditDone(XIC xic, XPointer client_data, XPointer call_data)
{
	CtrlTextFieldWidget textw;

	(void)xic;
	(void)call_data;
	textw = (CtrlTextFieldWidget)client_data;
	textw->text.under_preedit = FALSE;
	FREE(textw->text.preedit_value);
	textw->text.preedit_value = NULL;
	textw->text.preedit_size = 0;
	textw->text.preedit_length = 0;
	return 0;
}

static int
PreeditDraw(XIC xic, XPointer client_data, XPointer call_data)
{
	XIMPreeditDrawCallbackStruct *pdraw;
	CtrlTextFieldWidget textw;
	Widget w;
	int beg, dellen, inslen, endlen;

	(void)xic;
	w = (Widget)client_data;
	textw = (CtrlTextFieldWidget)w;
	pdraw = (XIMPreeditDrawCallbackStruct *)call_data;
	if (pdraw == NULL)
		return 0;

	/* we do not support wide characters */
	if (pdraw->text && pdraw->text->encoding_is_wchar == True) {
		WARN(XtWidgetToApplicationContext(w), "unsupportedEncoding", "Control only supports utf8");
		return 0;
	}

	beg = _CtrlRuneBytes(textw->text.preedit_value, pdraw->chg_first);
	dellen = _CtrlRuneBytes(textw->text.preedit_value + beg, pdraw->chg_length);
	inslen = pdraw->text ? _CtrlRuneBytes(pdraw->text->string.multi_byte, pdraw->text->length) : 0;
	endlen = (beg + dellen < textw->text.preedit_length) ? strlen(textw->text.preedit_value + beg + dellen) : 0;

	/* we cannot change text past the end of our pre-edit string */
	if (beg + dellen >= textw->text.preedit_size || beg + inslen >= textw->text.preedit_size)
		return 0;

	/* get space for text to be copied, and copy it */
	memmove(textw->text.preedit_value + beg + inslen, textw->text.preedit_value + beg + dellen, endlen + 1);
	if (pdraw->text && pdraw->text->length)
		memcpy(textw->text.preedit_value + beg, pdraw->text->string.multi_byte, inslen);
	textw->text.preedit_length = beg + inslen + endlen;
	textw->text.preedit_value[textw->text.preedit_length] = '\0';

	/* get caret position */
	textw->text.caret_position = _CtrlRuneBytes(textw->text.preedit_value, pdraw->caret);

	/* draw text field */
	Redraw(w);
	return 0;
}

static int
PreeditCaret(XIC xic, XPointer client_data, XPointer call_data)
{
	XIMPreeditCaretCallbackStruct *pcaret;
	CtrlTextFieldWidget textw;
	Widget w;

	(void)xic;
	w = (Widget)client_data;
	textw = (CtrlTextFieldWidget)w;
	pcaret = (XIMPreeditCaretCallbackStruct *)call_data;
	if (pcaret == NULL)
		return 0;
	switch (pcaret->direction) {
	case XIMForwardChar:
		textw->text.caret_position = _CtrlNextRune(textw->text.preedit_value, textw->text.caret_position, +1);
		break;
	case XIMBackwardChar:
		textw->text.caret_position = _CtrlNextRune(textw->text.preedit_value, textw->text.caret_position, -1);
		break;
	case XIMForwardWord:
		textw->text.caret_position = _CtrlMoveWordEdge(textw->text.preedit_value, textw->text.caret_position, +1);
		break;
	case XIMBackwardWord:
		textw->text.caret_position = _CtrlMoveWordEdge(textw->text.preedit_value, textw->text.caret_position, -1);
		break;
	case XIMLineStart:
		textw->text.caret_position = 0;
		break;
	case XIMLineEnd:
		textw->text.caret_position = textw->text.preedit_length;
		break;
	case XIMAbsolutePosition:
		textw->text.caret_position = _CtrlRuneBytes(textw->text.preedit_value, pcaret->position);
		break;
	case XIMDontChange:
		/* do nothing */
		break;
	case XIMCaretUp:
	case XIMCaretDown:
	case XIMNextLine:
	case XIMPreviousLine:
		/* not implemented */
		break;
	}
	pcaret->position = _CtrlRuneChars(textw->text.preedit_value, textw->text.caret_position);
	Redraw(w);
	return 0;
}

static int
PreeditDestroy(XIC xic, XPointer client_data, XPointer call_data)
{
	CtrlTextFieldWidget textw;

	(void)xic;
	(void)call_data;
	textw = (CtrlTextFieldWidget)client_data;
	textw->text.xic = NULL;
	return 0;
}

static void
InitiateSelection(Widget w, Time time, Atom atom, String *str, int *siz)
{
	CtrlTextFieldWidget textw;
	int minpos, maxpos;

	textw = (CtrlTextFieldWidget)w;
	FREE(*str);
	minpos = MIN(textw->text.cursor_position, textw->text.selection_position);
	maxpos = MAX(textw->text.cursor_position, textw->text.selection_position);
	*siz = maxpos - minpos;
	if (*siz <= 0)
		return;
	*str = XtMalloc(*siz);
	memcpy(*str, textw->text.value + minpos, *siz);
	_CtrlOwnSelection(w, Copy, atom, time);
}

static void
ValueChanged(Widget w, XEvent *ev)
{
	CtrlGenericCallData cd;
	CtrlTextFieldWidget textw;

	textw = (CtrlTextFieldWidget)w;
	cd = (CtrlGenericCallData){
		.reason = CTRL_VALUE_CHANGED,
		.event = ev,
	};
	XtCallCallbackList(w, textw->text.value_changed_callback, (XtPointer)&cd);
}

static void
AdjustText(Widget w)
{
	CtrlTextFieldWidget textw;
	int left, margin, diff;

	/* update textw->text.h_offset for character at position to be visible */
	textw = (CtrlTextFieldWidget)w;
	margin = textw->primitive.margin_width + HALFTHICKNESS(textw);
	left = textw->text.h_offset;
	left += _CtrlGetTextWidth(textw->primitive.font, textw->text.value, textw->text.cursor_position);
	left += _CtrlGetTextWidth(textw->primitive.font, textw->text.preedit_value, textw->text.caret_position);
	if ((diff = left - margin) < 0) {
		/* scroll text to the right */
		textw->text.h_offset -= diff;
	} else if ((diff = left - textw->core.width + margin) > 0) {
		/* scroll text to the left */
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
SetCursor(Widget w, Time time, int pos, Boolean select, Boolean redraw)
{
	CtrlTextFieldWidget textw;

	textw = (CtrlTextFieldWidget)w;
	if (pos < 0 || pos > textw->text.text_length)
		return;
	textw->text.cursor_position = pos;
	if (select) {
		if (textw->text.cursor_position != textw->text.selection_position) {
			InitiateSelection(w, time, XA_PRIMARY, &textw->text.primary_value, &textw->text.primary_size);
		}
	} else {
		textw->text.selection_position = textw->text.cursor_position;
	}
	if (redraw) {
		Redraw(w);
	}
}

static void
Redraw(Widget w)
{
	CtrlTextFieldWidget textw;

	textw = (CtrlTextFieldWidget)w;
	AdjustText(w);
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
	int minpos, maxpos, len;

	textw = (CtrlTextFieldWidget)w;
	if (textw->text.selection_position == textw->text.cursor_position)
		return FALSE;
	minpos = MIN(textw->text.cursor_position, textw->text.selection_position);
	maxpos = MAX(textw->text.cursor_position, textw->text.selection_position);
	len = maxpos - minpos;
	if (len <= 0)
		return FALSE;
	textw->text.cursor_position = maxpos;
	textw->text.selection_position = minpos;
	Insert(textw, NULL, 0 - len);
	textw->text.cursor_position = minpos;
	return TRUE;
}

static Boolean
Copy(Widget w, Atom *sel, Atom *target, Atom *type, XtPointer *val, unsigned long *len, int *fmt)
{
	CtrlTextFieldWidget textw;
	char *p;

	/*
	 * We allocate memory into the *val argument.
	 * We need not free it, for the Xt framework automatically frees it
	 * when the selection conversion is done (because _CtrlOwnSelection
	 * in util.c does not inform a XtSelectionDoneProc process).
	 */
	if (*target == _CtrlInternAtom(XtDisplay(w), TARGETS)) {
		/*
		 * We have been asked for supported target formats.
		 * Answer with the only one we know (UTF8_STRING).
		 */
		*type = XA_ATOM;
		*len = 1;
		*fmt = ATOM_SIZE;
		*val = XtMalloc(sizeof(Atom));
		(*((Atom *)*val)) = _CtrlInternAtom(XtDisplay(w), UTF8_STRING);
		return TRUE;
	} else if (*target == _CtrlInternAtom(XtDisplay(w), UTF8_STRING) || *target == XA_STRING) {
		/*
		 * The last comment is false.
		 * We also support the old, generic XA_STRING target.
		 * But we'll send a UTF8 string in either case.
		 */
		textw = (CtrlTextFieldWidget)w;
		*fmt = UTF8_SIZE;
		*type = *target;
		if (*sel == XA_PRIMARY) {
			*len = textw->text.primary_size;
			p = textw->text.primary_value;
		} else if (*sel == _CtrlInternAtom(XtDisplay(w), CLIPBOARD)) {
			*len = textw->text.clipboard_size;
			p = textw->text.clipboard_value;
		} else {
			return FALSE;
		}
		*val = XtMalloc(*len);
		memcpy(*val, p, *len);
		return TRUE;
	}
	/*
	 * If we reach here, we don't know what to do (we have been
	 * asked to convert our selection to a format we do not know).
	 */
	return FALSE;
}

static void
Paste(Widget w, XtPointer client_data, Atom *sel, Atom *type, XtPointer val, unsigned long *len, int *fmt)
{
	unsigned long i, l;
	String s;

	(void)client_data;
	(void)sel;
	(void)fmt;
	if (*type == XT_CONVERT_FAIL || val == NULL || *len == 0)
		return;
	l = *len;
	s = (String)val;
	for (i = l; i > 0; i--) {
		if (s[i - 1] != '\0')
			break;
	}
	if (i == 0)
		return;
	AddUndo(w, TRUE);
	DeleteSelection(w);
	Insert((CtrlTextFieldWidget)w, s, i);
	Redraw(w);
}

static int
GetCursorPosition(Widget w, int xpos)
{
	CtrlTextFieldWidget textw;
	int x, i, j, textwidth;

	textw = (CtrlTextFieldWidget)w;
	xpos -= textw->primitive.font_average_width / 2;
	x = textw->text.h_offset;
	for (i = j = 0; textw->text.value[i] != '\0'; i = _CtrlNextRune(textw->text.value, i, +1)) {
		if (xpos < x)
			break;
		textwidth = _CtrlGetTextWidth(textw->primitive.font, textw->text.value, i);
		x = textw->text.h_offset + textwidth;
		j = i;
	}
	if (textw->text.value[i] == '\0')
		return i;
	return j;
}

static void
AddUndo(Widget w, Boolean editing)
{
	CtrlTextFieldWidget textw;
	struct Undo *list, *curr, *undo, *tmp;

	textw = (CtrlTextFieldWidget)w;

	/*
	 * When adding a new entry to the undo list, delete the entries
	 * after the new one.
	 */
	list = (struct Undo *)textw->text.undo_list;
	curr = (struct Undo *)textw->text.undo_current;
	if (curr != NULL && curr->prev != NULL) {
		undo = curr->prev;
		while (undo) {
			tmp = undo;
			undo = undo->prev;
			free(tmp->text);
			free(tmp);
		}
		curr->prev = NULL;
		textw->text.undo_list = (XtPointer)curr;
	}

	/*
	 * Add a new entry only if it differs from the one at the top of
	 * the list.
	 */
	list = (struct Undo *)textw->text.undo_list;
	if (list->text == NULL || strcmp(list->text, textw->text.value) != 0) {
		undo = (struct Undo *)XtMalloc(sizeof(*undo));
		undo->text = XtMalloc(textw->text.text_length + 1);
		undo->length = textw->text.text_length;
		memcpy(undo->text, textw->text.value, undo->length);
		undo->text[undo->length] = '\0';
		undo->next = list;
		undo->prev = NULL;
		if (list != NULL)
			list->prev = undo;
		textw->text.undo_list = (XtPointer)undo;

		/* If we are editing text, the current entry is the top one.  */
		if (editing) {
			textw->text.undo_current = (XtPointer)undo;
		}
	}
}
