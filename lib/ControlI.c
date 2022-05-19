#include <control/ControlP.h>

#include "ControlI.h"

void
InitializeConverters(void)
{
	static Boolean initialized = FALSE;

	if (initialized)
		return;
#warning TODO: initialize converters
}

void
NewPixmap(Display *dpy, Pixmap *pix, Window win, Dimension w, Dimension h, Cardinal depth)
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
DrawRectangle(Display *dpy, Pixmap pix, Pixmap tile, Pixel color, Position x, Position y, Dimension w, Dimension h)
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
DrawHighlight(Display *dpy, Pixmap pix, Pixmap tile, Pixel color, Position x, Position y, Dimension w, Dimension h, Dimension b)
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
DrawTopShadow(Display *dpy, Pixmap pix, Pixmap tile, Pixel color, Position x, Position y, Dimension w, Dimension h, Dimension bi, Dimension bo)
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
DrawBottomShadow(Display *dpy, Pixmap pix, Pixmap tile, Pixel color, Position x, Position y, Dimension w, Dimension h, Dimension bi, Dimension bo)
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
CommitPixmap(Display *dpy, Window win, Pixmap pix, Position x, Position y, Dimension w, Dimension h)
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
