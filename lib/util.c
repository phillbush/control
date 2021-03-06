#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include <control_private.h>

#include "common.h"

#define CONVERTER_DONE(to, type, value)                         \
{                                                               \
	if ((to)->addr != NULL) {                               \
		if ((to)->size < sizeof(type)) {                \
			(to)->size = sizeof(type);              \
			goto error;                             \
		} else {                                        \
			*((type *)((to)->addr)) = (value);      \
		}                                               \
	} else {                                                \
		(to)->addr = (XPointer)&(type){(value)};        \
	}                                                       \
	(to)->size = sizeof(type);                              \
}

typedef struct CtrlFontRec {
	Display        *dpy;
	XftFont        *xftfont;
} CtrlFontRec, *CtrlFont;

typedef struct CtrlColorRec {
	Display        *dpy;
	XftColor        xftcolor;
} CtrlColorRec, *CtrlColor;

static void
NullProc(Widget w, XtPointer p, XEvent *ev, Boolean *b)
{
	(void)w;
	(void)p;
	(void)ev;
	(void)b;
	/* This function does nothing.  It's here to allow the event
	 * mask required by the input method to be added to the client
	 * window.
	 */
}

static Widget GetShell(Widget w)
{
	while (w != NULL && !XtIsShell(w))
		w = XtParent(w);
	return w;
}

static Boolean
CvtStringToDimension(Display *dpy, XrmValue *args, Cardinal *nargs, XrmValue *from, XrmValue *to, XtPointer *data)
{
	XtAppContext app;
	String str, ep;
	Dimension val;

	(void)args;
	(void)nargs;
	(void)data;
	app = XtDisplayToApplicationContext(dpy);
	XtAppLock(app);
	if (from->addr == NULL)
		goto error;
	str = (String)from->addr;
	errno = 0;
	val = strtoul(str, &ep, 10);
	if (str[0] == '\0' || *ep != '\0' || errno == ERANGE) {
		WARN(app, "unknownValue", "value is not a number");
		goto error;
	}
	XtAppUnlock(app);
	CONVERTER_DONE(to, Dimension, val)
	return TRUE;
error:
	XtAppUnlock(app);
	return FALSE;
}

static Boolean
CvtStringToCardinal(Display *dpy, XrmValue *args, Cardinal *nargs, XrmValue *from, XrmValue *to, XtPointer *data)
{
	XtAppContext app;
	String str, ep;
	Cardinal val;

	(void)args;
	(void)nargs;
	(void)data;
	app = XtDisplayToApplicationContext(dpy);
	XtAppLock(app);
	if (from->addr == NULL)
		goto error;
	str = (String)from->addr;
	errno = 0;
	val = strtoul(str, &ep, 10);
	if (str[0] == '\0' || *ep != '\0' || errno == ERANGE) {
		WARN(app, "unknownValue", "value is not a number");
		goto error;
	}
	XtAppUnlock(app);
	CONVERTER_DONE(to, Cardinal, val)
	return TRUE;
error:
	XtAppUnlock(app);
	return FALSE;
}

static Boolean
CvtStringToFont(Display *dpy, XrmValue *args, Cardinal *nargs, XrmValue *from, XrmValue *to, XtPointer *data)
{
	CtrlFont font;
	String fontName;
	XtAppContext app;
	XtPointer p;
	Boolean allocd;

	(void)args;
	(void)nargs;
	(void)data;
	allocd = FALSE;
	font = NULL;
	app = XtDisplayToApplicationContext(dpy);
	XtAppLock(app);
	if (from->addr == NULL)
		goto error;
	fontName = (String)from->addr;
	font = (CtrlFont)XtMalloc(sizeof(*font));
	font->dpy = dpy;
	if ((font->xftfont = XftFontOpenXlfd(dpy, DefaultScreen(dpy), fontName)) == NULL) {
		if ((font->xftfont = XftFontOpenName(dpy, DefaultScreen(dpy), fontName)) == NULL) {
			WARN(app, "unknownValue", "could not open font");
			goto error;
		}
	}
	XtAppUnlock(app);
	p = (XtPointer)font;
	allocd = TRUE;
	CONVERTER_DONE(to, XtPointer, p)
	return TRUE;
error:
	XtAppUnlock(app);
	if (allocd)
		XftFontClose(dpy, font->xftfont);
	FREE(font);
	return FALSE;
}

static void
CvtFontDestroy(XtAppContext app, XrmValue *to, XtPointer data, XrmValue *args, Cardinal *nargs)
{
	CtrlFont font;

	(void)app;
	(void)data;
	(void)args;
	(void)nargs;
	font = *((CtrlFont *)to->addr);
	XftFontClose(font->dpy, font->xftfont);
	FREE(font);
}

static Boolean
CvtStringToColor(Display *dpy, XrmValue *args, Cardinal *nargs, XrmValue *from, XrmValue *to, XtPointer *data)
{
	CtrlColor color;
	String colorName;
	XtAppContext app;
	XtPointer p;
	Boolean allocd;

	(void)args;
	(void)nargs;
	(void)data;
	allocd = FALSE;
	color = NULL;
	app = XtDisplayToApplicationContext(dpy);
	XtAppLock(app);
	if (from->addr == NULL)
		goto error;
	colorName = (String)from->addr;
	color = (CtrlColor)XtMalloc(sizeof(*color));
	color->dpy = dpy;
	if (!XftColorAllocName(dpy, DefaultVisual(dpy, DefaultScreen(dpy)), DefaultColormap(dpy, DefaultScreen(dpy)), colorName, &color->xftcolor)) {
		WARN(app, "unknownValue", "could not allocate color");
		goto error;
	}
	XtAppUnlock(app);
	p = (XtPointer)color;
	allocd = TRUE;
	CONVERTER_DONE(to, XtPointer, p);
	return TRUE;
error:
	XtAppUnlock(app);
	if (allocd)
		XftColorFree(dpy, DefaultVisual(dpy, DefaultScreen(dpy)), DefaultColormap(dpy, DefaultScreen(dpy)), &color->xftcolor);
	FREE(color);
	return FALSE;
}

static void
CvtColorDestroy(XtAppContext app, XrmValue *to, XtPointer data, XrmValue *args, Cardinal *nargs)
{
	CtrlColor color;

	(void)app;
	(void)data;
	(void)args;
	(void)nargs;
	color = *((CtrlColor *)to->addr);
	XftColorFree(
		color->dpy,
		DefaultVisual(color->dpy, DefaultScreen(color->dpy)),
		DefaultColormap(color->dpy, DefaultScreen(color->dpy)),
		&color->xftcolor
	);
	FREE(color);
}

static void
DisownCallback(Widget w, XtPointer call_data, XtPointer client_data)
{
	XtAppContext app;
	Time time;

	(void)call_data;
	app = XtWidgetToApplicationContext(w);
	XtAppLock(app);
	time = XtLastTimestampProcessed(XtDisplay(w));
	XtDisownSelection(w, (Atom)client_data, time);
	XtAppUnlock(app);
}

static void
LoseSelection(Widget w, Atom *sel)
{
	XtAppContext app;

	app = XtWidgetToApplicationContext(w);
	XtAppLock(app);
	XtRemoveCallback(w, CtrlNdestroyCallback, DisownCallback, (XtPointer)*sel);
	XtAppUnlock(app);
}

void
_CtrlOwnSelection(Widget w, XtConvertSelectionProc cvt, Atom sel, Time time)
{
	XtAppContext app;

	app = XtWidgetToApplicationContext(w);
	XtAppLock(app);
	XtOwnSelection(w, sel, time, cvt, LoseSelection, NULL);
	XtAddCallback(w, CtrlNdestroyCallback, DisownCallback, (XtPointer)sel);
	XtAppUnlock(app);
}

void
_CtrlGetSelection(Widget w, Atom sel, Atom target, XtSelectionCallbackProc proc, Time time)
{
	XtAppContext app;

	app = XtWidgetToApplicationContext(w);
	XtAppLock(app);
	XtGetSelectionValue(w, sel, target, proc, NULL, time);
	XtAppUnlock(app);
}

void
_CtrlRegisterConverters(void)
{
	static Boolean registered = FALSE;
	static struct {
		char            *from_type;
		char            *to_type;
		XtTypeConverter  converter;
		XtCacheType      cache_type;
		XtDestructor     destructor;
	} converters[] = {
		{
			CtrlRString,
			CtrlRXftColor,
			CvtStringToColor,
			XtCacheByDisplay | XtCacheRefCount,
			CvtColorDestroy,
		},
		{
			CtrlRString,
			CtrlRXftFont,
			CvtStringToFont,
			XtCacheByDisplay | XtCacheRefCount,
			CvtFontDestroy,
		},
		{
			CtrlRString,
			CtrlRCardinal,
			CvtStringToCardinal,
			XtCacheNone,
			NULL,
		},
		{
			CtrlRString,
			CtrlRDimension,
			CvtStringToDimension,
			XtCacheNone,
			NULL,
		},
	};
	Cardinal i;

	/*
	 * Converter functions and their respective destructor functions
	 * are registered here using the converters[] table.  This table
	 * contains the following fields:
	 *
	 * - A CtrlROrigin string, used to name the Origin type.  This should
	 *   be a macro to a string defined in include/control/Control.h
	 *
	 * - A CtrlRTarget string, used to name the Target type.  this should
	 *   be a macro to a string defined in include/control/Control.h
	 *
	 * - A CvtOriginToTarget function, used to convert between the origin
	 *   and Target types.  This should be a static function defined in
	 *   this source file.
	 *
	 * - A XtCacheSomething value specifying whether to reuse results from
	 *   previous conversions.  See X Toolkit Intrinsics documentation for
	 *   more information.
	 *
	 * - A CvtTargetDestroy function, used to destroy (or decrement a
	 *   reference counter) of an object of the Target type.  This should
	 *   be a static function defined in this source file.
	 */
	if (registered)
		return;
	registered = TRUE;
	for (i = 0; i < XtNumber(converters); i++) {
		XtSetTypeConverter(
			converters[i].from_type,
			converters[i].to_type,
			converters[i].converter,
			NULL,
			0,
			converters[i].cache_type,
			converters[i].destructor
		);
	}
}

void
_CtrlNewPixmap(Display *dpy, Pixmap *pix, Window win, Dimension w, Dimension h, Cardinal depth)
{
	XtAppContext app;

	if (win == None || w == 0 || h == 0)
		return;
	app = XtDisplayToApplicationContext(dpy);
	XtAppLock(app);
	if (*pix != None)
		XFreePixmap(dpy, *pix);
	*pix = XCreatePixmap(dpy, win, w, h, depth);
	XtAppUnlock(app);
}

void
_CtrlDelPixmap(Display *dpy, Pixmap *pix)
{
	XtAppContext app;

	if (*pix == None)
		return;
	app = XtDisplayToApplicationContext(dpy);
	XtAppLock(app);
	XFreePixmap(dpy, *pix);
	XtAppUnlock(app);
}

void
_CtrlDrawRectangle(Display *dpy, Pixmap pix, Pixmap tile, Pixel color, int x, int y, Dimension w, Dimension h)
{
	XGCValues gcvalues;
	XtAppContext app;
	GC gc;
	unsigned long valuemask;

	if (pix == None || w == 0 || h == 0)
		return;
	app = XtDisplayToApplicationContext(dpy);
	if (tile != None && tile != XtUnspecifiedPixmap) {
		gcvalues.fill_style = FillTiled;
		gcvalues.tile = tile;
		valuemask = GCFillStyle | GCTile;
	} else {
		gcvalues.fill_style = FillSolid;
		gcvalues.foreground = color;
		valuemask = GCFillStyle | GCForeground;
	}
	gc = XCreateGC(dpy, pix, valuemask, &gcvalues);
	XtAppLock(app);
	XFillRectangle(dpy, pix, gc, x, y, w, h);
	XFreeGC(dpy, gc);
	XtAppUnlock(app);
}

void
_CtrlDrawXftRectangle(Display *dpy, Pixmap pix, XtPointer color, int x, int y, Dimension w, Dimension h)
{
	XGCValues gcvalues;
	XtAppContext app;
	GC gc;
	unsigned long valuemask;

	if (pix == None || w == 0 || h == 0)
		return;
	app = XtDisplayToApplicationContext(dpy);
	gcvalues.fill_style = FillSolid;
	gcvalues.foreground = ((CtrlColor)color)->xftcolor.pixel;
	valuemask = GCFillStyle | GCForeground;
	gc = XCreateGC(dpy, pix, valuemask, &gcvalues);
	XtAppLock(app);
	XFillRectangle(dpy, pix, gc, x, y, w, h);
	XFreeGC(dpy, gc);
	XtAppUnlock(app);
}

void
_CtrlDrawHighlight(Display *dpy, Pixmap pix, Pixmap tile, Pixel color, int x, int y, Dimension w, Dimension h, Dimension b)
{
	XRectangle rects[4];
	XGCValues gcvalues;
	XtAppContext app;
	GC gc;
	unsigned long valuemask;

	if (pix == None || w == 0 || h == 0 || b == 0)
		return;
	app = XtDisplayToApplicationContext(dpy);
	if (tile != None && tile != XtUnspecifiedPixmap) {
		gcvalues.fill_style = FillTiled;
		gcvalues.tile = tile;
		valuemask = GCFillStyle | GCTile;
	} else {
		gcvalues.fill_style = FillSolid;
		gcvalues.foreground = color;
		valuemask = GCFillStyle | GCForeground;
	}
	gc = XCreateGC(dpy, pix, valuemask, &gcvalues);
	rects[0].x = rects[1].x = rects[2].x = x ;
	rects[3].x = x + w - b ;
	rects[0].y = rects[2].y = rects[3].y = y ;
	rects[1].y = y + h - b ;
	rects[0].width = rects[1].width = w ;
	rects[2].width = rects[3].width = b ;
	rects[0].height = rects[1].height = b ;
	rects[2].height = rects[3].height = h ;
	XtAppLock(app);
	XFillRectangles (dpy, pix, gc, rects, 4);
	XtAppUnlock(app);
	XFreeGC(dpy, gc);
}

void
_CtrlDrawTopShadow(Display *dpy, Pixmap pix, Pixmap tile, Pixel color, int x, int y, Dimension w, Dimension h, Dimension bi, Dimension bo)
{
	XRectangle *rects;
	XGCValues gcvalues;
	XtAppContext app;
	GC gc;
	Dimension i;
	unsigned long valuemask;

	if (pix == None || w == 0 || h == 0 || bi == 0)
		return;
	if (bo > 0) {
		x += bo;
		y += bo;
		w -= bo * 2;
		h -= bo * 2;
	}
	app = XtDisplayToApplicationContext(dpy);
	rects = (XRectangle *)XtCalloc(bi * 2, sizeof(*rects));
	if (tile != None && tile != XtUnspecifiedPixmap) {
		gcvalues.fill_style = FillTiled;
		gcvalues.tile = tile;
		valuemask = GCFillStyle | GCTile;
	} else {
		gcvalues.fill_style = FillSolid;
		gcvalues.foreground = color;
		valuemask = GCFillStyle | GCForeground;
	}
	for(i = 0; i < bi; i++) {
		rects[i * 2]     = (XRectangle){.x = x + i, .y = y + i, .width = 1, .height = h - (i * 2 + 1)};
		rects[i * 2 + 1] = (XRectangle){.x = x + i, .y = y + i, .width = w - (i * 2 + 1), .height = 1};
	}
	gc = XCreateGC(dpy, pix, valuemask, &gcvalues);
	XtAppLock(app);
	XFillRectangles (dpy, pix, gc, rects, bi * 2);
	XtAppUnlock(app);
	XFreeGC(dpy, gc);
	FREE(rects);
}

void
_CtrlDrawBottomShadow(Display *dpy, Pixmap pix, Pixmap tile, Pixel color, int x, int y, Dimension w, Dimension h, Dimension bi, Dimension bo)
{
	XRectangle *rects;
	XGCValues gcvalues;
	XtAppContext app;
	GC gc;
	Dimension i;
	unsigned long valuemask;

	if (pix == None || w == 0 || h == 0 || bi == 0)
		return;
	if (bo > 0) {
		x += bo;
		y += bo;
		w -= bo * 2;
		h -= bo * 2;
	}
	app = XtDisplayToApplicationContext(dpy);
	rects = (XRectangle *)XtCalloc(bi * 2, sizeof(*rects));
	if (tile != None && tile != XtUnspecifiedPixmap) {
		gcvalues.fill_style = FillTiled;
		gcvalues.tile = tile;
		valuemask = GCFillStyle | GCTile;
	} else {
		gcvalues.fill_style = FillSolid;
		gcvalues.foreground = color;
		valuemask = GCFillStyle | GCForeground;
	}
	for(i = 0; i < bi; i++) {
		rects[i * 2]     = (XRectangle){.x = x + w - 1 - i, .y = y + i,         .width = 1,     .height = h - i * 2};
		rects[i * 2 + 1] = (XRectangle){.x = x + i,         .y = y + h - 1 - i, .width = w - i * 2, .height = 1};
	}
	gc = XCreateGC(dpy, pix, valuemask, &gcvalues);
	XtAppLock(app);
	XFillRectangles (dpy, pix, gc, rects, bi * 2);
	XtAppUnlock(app);
	XFreeGC(dpy, gc);
	FREE(rects);
}

void
_CtrlDrawText(Display *dpy, Pixmap pix, XtPointer font, XtPointer color, int x, int y, String text, Cardinal len)
{
	XftDraw *draw;

	draw = XftDrawCreate(dpy, pix, DefaultVisual(dpy, DefaultScreen(dpy)), DefaultColormap(dpy, DefaultScreen(dpy)));
	XftDrawStringUtf8(draw, &((CtrlColor)color)->xftcolor, ((CtrlFont)font)->xftfont, x, y + ((CtrlFont)font)->xftfont->ascent, text, len);
	XftDrawDestroy(draw);
}

void
_CtrlCommitPixmap(Display *dpy, Window win, Pixmap pix, int x, int y, Dimension w, Dimension h)
{
	XtAppContext app;
	GC gc;

	if (win == None || pix == None || w == 0 || h == 0)
		return;
	app = XtDisplayToApplicationContext(dpy);
	XtAppLock(app);
	gc = XCreateGC(dpy, win, 0, NULL);
	XCopyArea(dpy, pix, win, gc, 0, 0, w, h, x, y);
	XFreeGC(dpy, gc);
	XtAppUnlock(app);
}

void
_CtrlGetFontMetrics(XtAppContext app, XtPointer font, Dimension *average_width, Dimension *ascent, Dimension *descent, Dimension *height)
{
	if (font == NULL)
		ERROR(app, "noFont", "could not get font metrics");
	if (average_width != NULL)
		*average_width = ((CtrlFont)font)->xftfont->max_advance_width;
	if (ascent != NULL)
		*ascent        = ((CtrlFont)font)->xftfont->ascent;
	if (descent != NULL)
		*descent       = ((CtrlFont)font)->xftfont->descent;
	if (height != NULL)
		*height        = ((CtrlFont)font)->xftfont->height;
}

Dimension
_CtrlGetTextWidth(XtPointer font, String text, Cardinal len)
{
	XGlyphInfo box;

	XftTextExtentsUtf8(((CtrlFont)font)->dpy, ((CtrlFont)font)->xftfont, text, len, &box);
	return box.xOff;
}

XIC
_CtrlGetInputContext(Widget w, XICProc startp, XICProc donep, XICProc drawp, XICProc caretp, XICProc destroyp)
{
	CtrlShellWidget shell;
	Display *dpy;
	XtAppContext app;
	XVaNestedList preedit;
	Window win;
	XIC xic;
	XtValueMask mask;

	/* get input context for window (create one if non existant) */
	xic = NULL;
	dpy = XtDisplay(w);
	preedit = NULL;
	app = XtDisplayToApplicationContext(dpy);
	win = XtWindow(w);
	XtAppLock(app);
	shell = (CtrlShellWidget)GetShell(w);
	preedit = XVaCreateNestedList(
		0,
		XNPreeditStartCallback, &(XICCallback){
			.client_data    = (XPointer)w,
			.callback       = startp,
		},
		XNPreeditDoneCallback,  &(XICCallback){
			.client_data    = (XPointer)w,
			.callback       = donep,
		},
		XNPreeditDrawCallback,  &(XICCallback){
			.client_data    = (XPointer)w,
			.callback       = drawp,
		},
		XNPreeditCaretCallback, &(XICCallback){
			.client_data    = (XPointer)w,
			.callback       = caretp,
		},
		NULL
	);
	if (preedit == NULL)
		goto error;
	xic = XCreateIC(
		shell->ctrl.xim,
		XNInputStyle,           shell->ctrl.pref_style,
		XNPreeditAttributes,    preedit,
		XNDestroyCallback,      &(XICCallback){
			.client_data    = (XPointer)w,
			.callback       = destroyp,
		},
		XNClientWindow,         win,
		/* we need not set XNFocusWindow for it defaults to XNClientWindow */
		NULL
	);
	if (xic == NULL)
		goto error;
	if (XGetICValues(xic, XNFilterEvents, &mask, NULL) != NULL)
		goto error;
	if (mask)
		XtAddEventHandler(w, mask, False, NullProc, NULL);
	XFree(preedit);
	XtAppUnlock(app);
	return xic;
error:
	if (xic != NULL)
		XDestroyIC(xic);
	XFree(preedit);
	XtAppUnlock(app);
	ERROR(app, "noInputContext", "could not create input context");
	return NULL;
}

XtGeometryResult
_CtrlReplyToQueryGeometry(Widget w, XtWidgetGeometry *intended, XtWidgetGeometry *desired)
{
	XtAppContext app;
	XtGeometryResult ret;

	app = XtWidgetToApplicationContext(w);
	desired->request_mode = CWWidth | CWHeight;
	if ((intended->request_mode & CWWidth) &&
	    (intended->request_mode & CWHeight) &&
	    intended->width == desired->width &&
	    intended->height == desired->height) {
		return XtGeometryYes;
	}
	XtAppLock(app);
	ret = (desired->width == w->core.width && desired->height == w->core.height)
	    ? XtGeometryNo
	    : XtGeometryAlmost;
	XtAppUnlock(app);
	return ret;
}

Status
_CtrlLookupString(Display *dpy, XIC xic, XEvent *ev, String buf, int size, int *len)
{
	XtAppContext app;
	Status ret;
	;

	app = XtDisplayToApplicationContext(dpy);
	XtAppLock(app);
	*len = XmbLookupString(xic, (XKeyEvent *)ev, buf, size, NULL, &ret);
	XtAppUnlock(app);
	return ret;
}

/* return location of next utf8 rune in the given direction (+1 or -1) */
int
_CtrlNextRune(String text, int position, int inc)
{
	int n;

	for (n = position + inc; n + inc >= 0 && (text[n] & 0xc0) == 0x80; n += inc)
		;
	return n;
}

/* return bytes from beginning of text to nth utf8 rune to the right */
int
_CtrlRuneBytes(String text, int n)
{
	int ret;

	ret = 0;
	while (n-- > 0)
		ret += _CtrlNextRune(text + ret, 0, 1);
	return ret;
}

/* return number of characters from beginning of text to nth byte to the right */
int
_CtrlRuneChars(String text, int n)
{
	int ret, i;

	ret = i = 0;
	while (i < n) {
		i += _CtrlNextRune(text + i, 0, 1);
		ret++;
	}
	return ret;
}

/* return position to start (dir = -1) or end (dir = +1) of the word */
int
_CtrlMoveWordEdge(String text, int pos, int dir)
{
	if (dir < 0) {
		while (pos > 0 && isspace((unsigned char)text[_CtrlNextRune(text, pos, -1)]))
			pos = _CtrlNextRune(text, pos, -1);
		while (pos > 0 && !isspace((unsigned char)text[_CtrlNextRune(text, pos, -1)]))
			pos = _CtrlNextRune(text, pos, -1);
	} else {
		while (text[pos] && isspace((unsigned char)text[pos]))
			pos = _CtrlNextRune(text, pos, +1);
		while (text[pos] && !isspace((unsigned char)text[pos]))
			pos = _CtrlNextRune(text, pos, +1);
	}
	return pos;
}

Atom
_CtrlInternAtom(Display *dpy, String name)
{
	XtAppContext app;
	Atom sel;

	app = XtDisplayToApplicationContext(dpy);
	XtAppLock(app);
	sel = XInternAtom(dpy, name, FALSE);
	XtAppUnlock(app);
	return sel;
}

XftFont *
_CtrlGetFont(XtPointer font)
{
	return ((CtrlFont)font)->xftfont;
}

XftColor *
_CtrlGetColor(XtPointer color)
{
	return &((CtrlColor)color)->xftcolor;
}
