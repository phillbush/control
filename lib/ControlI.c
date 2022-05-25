#include <control/ControlP.h>

#include "ControlI.h"

#define FREE(p) XtFree((char *)(p))
#define WARN(app, name, msg)  XtAppWarningMsg((app), (name), __func__, "ToolkitError", (msg), NULL, 0)
#define ERROR(app, name, msg) XtAppErrorMsg((app), (name), __func__, "ToolkitError", (msg), NULL, 0)
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

typedef struct _CtrlFontRec {
	Display        *dpy;
	XftFont        *xftfont;
} _CtrlFontRec, *_CtrlFont;

struct _CtrlInputMethodRec {
	XContext        context;
	XIM             xim;
	XIMStyles      *styles;
	XIMStyle        pref_style;
};

struct _CtrlInputContextRec {
	XContext        context;
	XIC             xic;
	Window          focus_window;
	unsigned long   event_mask;
};

static XContext ximcontext = 0;
static XContext xiccontext = 0;

static Boolean
CvtStringToFont(Display *dpy, XrmValue *args, Cardinal *nargs, XrmValue *from, XrmValue *to, XtPointer *data)
{
	_CtrlFont font;
	String fontName;
	XtAppContext app;
	XtPointer p;

	(void)args;
	(void)nargs;
	(void)data;
	font = NULL;
	app = XtDisplayToApplicationContext(dpy);
	XtAppLock(app);
	if (from->addr == NULL)
		goto error;
	fontName = (String)from->addr;
	font = (_CtrlFont)XtMalloc(sizeof(*font));
	font->dpy = dpy;
	if ((font->xftfont = XftFontOpenXlfd(dpy, DefaultScreen(dpy), fontName)) == NULL) {
		if ((font->xftfont = XftFontOpenName(dpy, DefaultScreen(dpy), fontName)) == NULL) {
			WARN(app, "unknownValue", "could not open font");
			goto error;
		}
	}
	XtAppUnlock(app);
	p = (XtPointer)font;
	CONVERTER_DONE(to, XtPointer, p)
	return TRUE;
error:
	XtAppUnlock(app);
	if (font->xftfont != NULL)
		XftFontClose(dpy, font->xftfont);
	FREE(font);
	return FALSE;
}

static void
CvtFontDestroy(XtAppContext app, XrmValue *to, XtPointer data, XrmValue *args, Cardinal *nargs)
{
	_CtrlFont font;

	(void)app;
	(void)data;
	(void)args;
	(void)nargs;
	font = *((_CtrlFont *)to->addr);
	XftFontClose(font->dpy, font->xftfont);
	FREE(font);
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
			CtrlRFont,
			CvtStringToFont,
			XtCacheByDisplay | XtCacheRefCount,
			CvtFontDestroy
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
_CtrlDrawRectangle(Display *dpy, Pixmap pix, Pixmap tile, Pixel color, Position x, Position y, Dimension w, Dimension h)
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
	XtAppUnlock(app);
	XFreeGC(dpy, gc);
}

void
_CtrlDrawHighlight(Display *dpy, Pixmap pix, Pixmap tile, Pixel color, Position x, Position y, Dimension w, Dimension h, Dimension b)
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
_CtrlDrawTopShadow(Display *dpy, Pixmap pix, Pixmap tile, Pixel color, Position x, Position y, Dimension w, Dimension h, Dimension bi, Dimension bo)
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
_CtrlDrawBottomShadow(Display *dpy, Pixmap pix, Pixmap tile, Pixel color, Position x, Position y, Dimension w, Dimension h, Dimension bi, Dimension bo)
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
_CtrlCommitPixmap(Display *dpy, Window win, Pixmap pix, Position x, Position y, Dimension w, Dimension h)
{
	XtAppContext app;
	GC gc;

	if (win == None || pix == None || w == 0 || h == 0)
		return;
	app = XtDisplayToApplicationContext(dpy);
	gc = XCreateGC(dpy, win, 0, NULL);
	XtAppLock(app);
	XCopyArea(dpy, pix, win, gc, 0, 0, w, h, x, y);
	XtAppUnlock(app);
	XFreeGC(dpy, gc);
}

void
_CtrlGetFontMetrics(XtAppContext app, XtPointer font, Dimension *average_width, Dimension *ascent, Dimension *descent, Dimension *height)
{
	if (font == NULL)
		ERROR(app, "noFont", "could not get font metrics");
	if (average_width != NULL)
		*average_width = ((_CtrlFont)font)->xftfont->max_advance_width;
	if (ascent != NULL)
		*ascent        = ((_CtrlFont)font)->xftfont->ascent;
	if (descent != NULL)
		*descent       = ((_CtrlFont)font)->xftfont->descent;
	if (height != NULL)
		*height        = ((_CtrlFont)font)->xftfont->height;
}

int
_CtrlGetTextWidth(XtPointer font, String text, Cardinal len)
{
	XGlyphInfo box;

	XftTextExtentsUtf8(((_CtrlFont)font)->dpy, ((_CtrlFont)font)->xftfont, text, len, &box);
	return box.xOff;
}

struct _CtrlInputMethodRec *
_CtrlGetInputMethod(Display *dpy)
{
	struct _CtrlInputMethodRec *im;
	XtAppContext app;
	XIMStyle preeditstyle;
	XIMStyle statusstyle;
	unsigned int i;

	/* get input method for display (create one if non existant) */
	preeditstyle = XIMPreeditNothing;
	statusstyle = XIMStatusNothing;                 /* we do not do status area (should we?) */
	app = XtDisplayToApplicationContext(dpy);
	XtAppLock(app);
	if (ximcontext == 0)
		ximcontext = XUniqueContext();
	if (XFindContext(dpy, None, ximcontext, (XPointer *)&im) == 0)
		goto done;
	im = (struct _CtrlInputMethodRec *)XtMalloc(sizeof(*im));
	im->context = ximcontext;
	if ((im->xim = XOpenIM(dpy, NULL, NULL, NULL)) == NULL)
		goto error;
	if (XGetIMValues(im->xim, XNQueryInputStyle, &im->styles, NULL) != NULL)
		goto error;
	for (i = 0; i < im->styles->count_styles; i++) {
		if (im->styles->supported_styles[i] & XIMPreeditCallbacks) {
			preeditstyle = XIMPreeditCallbacks;
			break;
		}
	}
	im->pref_style = preeditstyle | statusstyle;
done:
	XtAppUnlock(app);
	return im;
error:
	XtAppUnlock(app);
	ERROR(app, "noInputMethod", "could not open input method");
	return NULL;
}

struct _CtrlInputContextRec *
_CtrlGetInputContext(Display *dpy, Widget w, XICProc startp, XICProc donep, XICProc drawp, XICProc caretp, XICProc destroyp)
{
	struct _CtrlInputMethodRec *im;
	struct _CtrlInputContextRec *ic;
	XtAppContext app;
	XVaNestedList preedit;
	Window win;

	/* get input context for window (create one if non existant) */
	preedit = NULL;
	app = XtDisplayToApplicationContext(dpy);
	win = XtWindow(w);
	im = _CtrlGetInputMethod(dpy);
	XtAppLock(app);
	if (xiccontext == 0)
		xiccontext = XUniqueContext();
	if (XFindContext(dpy, win, xiccontext, (XPointer *)&ic) == 0)
		goto done;
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
	ic = (struct _CtrlInputContextRec *)XtMalloc(sizeof(*ic));
	ic->context = xiccontext;
	ic->focus_window = win;
	ic->xic = XCreateIC(
		im->xim,
		XNInputStyle,           im->pref_style,
		XNPreeditAttributes,    preedit,
		XNDestroyCallback,      &(XICCallback){
			.client_data    = (XPointer)w,
			.callback       = destroyp,
		},
		XNClientWindow,         win,
		/* we need not set XNFocusWindow for it defaults to XNClientWindow */
		NULL
	);
	if (ic->xic == NULL)
		goto error;
	if (XGetICValues(ic->xic, XNFilterEvents, &ic->event_mask, NULL) != NULL)
		goto error;
done:
	XFree(preedit);
	XtAppUnlock(app);
	return ic;
error:
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
