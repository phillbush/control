#include <control/ControlP.h>

#include "ControlI.h"

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

void
_CtrlInitializeConverters(void)
{
	static Boolean initialized = FALSE;

	if (initialized)
		return;
	initialized = TRUE;
#warning TODO: initialize converters
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
	XtFree((char *)rects);
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
	XtFree((char *)rects);
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
	XtAppErrorMsg(
		app,
		"noInputMethod",
		"_CtrlGetInputMethod",
		"XtToolkitError",
		"Couldn't open input method",
		NULL,
		0
	);
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
	XFree(preedit);
done:
	XtAppUnlock(app);
	return ic;
error:
	XtAppUnlock(app);
	XtAppErrorMsg(
		app,
		"noInputContext",
		"_CtrlGetInputContext",
		"XtToolkitError",
		"Couldn't create input context",
		NULL,
		0
	);
	return NULL;
}
