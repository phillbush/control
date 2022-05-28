#include <control_private.h>

#include "common.h"

/* core methods */
static void ClassInitialize(void);
static void ClassPartInitialize(WidgetClass);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Resize(Widget);
static void Realize(Widget, XtValueMask *, XSetWindowAttributes *);
static void Destroy(Widget);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);

/* primitive methods */
static void Highlight(Widget);
static void Unhighlight(Widget);
static void Press(Widget);
static void Unpress(Widget);
static void TooltipPost(Widget);
static void TooltipUnpost(Widget);
static void Activate(Widget, XEvent *, String *, Cardinal *);
static void Draw(Widget);

static XtActionsRec actions[] = {
#warning TODO: Fill in Primitive actions[] after implement widget traversal;
};

static XtResource resources[] = {
	{
		.resource_name   = CtrlNforeground,
		.resource_class  = CtrlCForeground,
		.resource_type   = CtrlRXftColor,
		.resource_size   = sizeof(XtPointer),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.foreground),
		.default_type    = CtrlRString,
		.default_addr    = (XtPointer)DEF_FOREGROUND,
	},
	{
		.resource_name   = CtrlNfont,
		.resource_class  = CtrlCFont,
		.resource_type   = CtrlRXftFont,
		.resource_size   = sizeof(XtPointer),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.font),
		.default_type    = CtrlRString,
		.default_addr    = (XtPointer)DEF_FONT,
	},
	{
		.resource_name   = CtrlNmarginHeight,
		.resource_class  = CtrlCMarginHeight,
		.resource_type   = CtrlRDimension,
		.resource_size   = sizeof(Dimension),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.margin_height),
		.default_type    = CtrlRImmediate,
		.default_addr    = (XtPointer)DEF_TEXT_MARGIN,
	},
	{
		.resource_name   = CtrlNmarginWidth,
		.resource_class  = CtrlCMarginWidth,
		.resource_type   = CtrlRDimension,
		.resource_size   = sizeof(Dimension),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.margin_width),
		.default_type    = CtrlRImmediate,
		.default_addr    = (XtPointer)DEF_TEXT_MARGIN,
	},
	{
		.resource_name   = CtrlNshadowThickness,
		.resource_class  = CtrlCShadowThickness,
		.resource_type   = CtrlRDimension,
		.resource_size   = sizeof(Dimension),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.shadow_thickness),
		.default_type    = CtrlRImmediate,
		.default_addr    = (XtPointer)DEF_THICKNESS,
	},
	{
		.resource_name   = CtrlNshadowLightColor,
		.resource_class  = CtrlCShadowLightColor,
		.resource_type   = CtrlRPixel,
		.resource_size   = sizeof(Pixel),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.shadow_light_pixel),
		.default_type    = CtrlRString,
		.default_addr    = (XtPointer)DEF_SHADOW_LIGHT_COLOR,
	},
	{
		.resource_name   = CtrlNshadowLightPixmap,
		.resource_class  = CtrlCShadowLightPixmap,
		.resource_type   = CtrlRPixmap,
		.resource_size   = sizeof(Pixmap),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.shadow_light_pixmap),
		.default_type    = CtrlRImmediate,
		.default_addr    = (XtPointer)None,
	},
	{
		.resource_name   = CtrlNshadowDarkColor,
		.resource_class  = CtrlCShadowDarkColor,
		.resource_type   = CtrlRPixel,
		.resource_size   = sizeof(Pixel),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.shadow_dark_pixel),
		.default_type    = CtrlRString,
		.default_addr    = (XtPointer)DEF_SHADOW_DARK_COLOR,
	},
	{
		.resource_name   = CtrlNshadowDarkPixmap,
		.resource_class  = CtrlCShadowDarkPixmap,
		.resource_type   = CtrlRPixmap,
		.resource_size   = sizeof(Pixmap),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.shadow_dark_pixmap),
		.default_type    = CtrlRImmediate,
		.default_addr    = (XtPointer)None,
	},
	{
		.resource_name   = CtrlNhighlightThickness,
		.resource_class  = CtrlCHighlightThickness,
		.resource_type   = CtrlRDimension,
		.resource_size   = sizeof(Dimension),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.highlight_thickness),
		.default_type    = CtrlRImmediate,
		.default_addr    = (XtPointer)DEF_THICKNESS,
	},
	{
		.resource_name   = CtrlNhighlightColor,
		.resource_class  = CtrlCHighlightColor,
		.resource_type   = CtrlRPixel,
		.resource_size   = sizeof(Pixel),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.highlight_pixel),
		.default_type    = CtrlRString,
		.default_addr    = (XtPointer)DEF_HIGHLIGHT_COLOR,
	},
	{
		.resource_name   = CtrlNhighlightPixmap,
		.resource_class  = CtrlCHighlightPixmap,
		.resource_type   = CtrlRPixmap,
		.resource_size   = sizeof(Pixmap),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.highlight_pixmap),
		.default_type    = CtrlRImmediate,
		.default_addr    = (XtPointer)None,
	},
	{
		.resource_name   = CtrlNisTabGroup,
		.resource_class  = CtrlCIsTabGroup,
		.resource_type   = CtrlRBoolean,
		.resource_size   = sizeof(Boolean),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.is_tab_group),
		.default_type    = CtrlRImmediate,
		.default_addr    = (XtPointer)FALSE,
	},
	{
		.resource_name   = CtrlNtraverseable,
		.resource_class  = CtrlCTraverseable,
		.resource_type   = CtrlRBoolean,
		.resource_size   = sizeof(Boolean),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.traverseable),
		.default_type    = CtrlRImmediate,
		.default_addr    = (XtPointer)TRUE,
	},
	{
		.resource_name   = CtrlNtooltip,
		.resource_class  = CtrlCTooltip,
		.resource_type   = CtrlRString,
		.resource_size   = sizeof(String),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.tooltip),
		.default_type    = CtrlRImmediate,
		.default_addr    = NULL,
	},
	{
		.resource_name   = CtrlNcursor,
		.resource_class  = CtrlCCursor,
		.resource_type   = CtrlRCursor,
		.resource_size   = sizeof(Cursor),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.cursor),
		.default_type    = CtrlRImmediate,
		.default_addr    = (XtPointer)None,
	},
};

CtrlPrimitiveClassRec ctrlPrimitiveClassRec = {
	.core_class = {
		.superclass             = (WidgetClass)&widgetClassRec,
		.class_name             = "CtrlPrimitive",
		.widget_size            = sizeof(CtrlPrimitiveRec),
		.class_initialize       = ClassInitialize,
		.class_part_initialize  = ClassPartInitialize,
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
		.expose                 = Redisplay,
		.set_values             = SetValues,
		.set_values_almost      = XtInheritSetValuesAlmost,
		.get_values_hook        = NULL,
		.accept_focus           = NULL,
		.version                = XtVersion,
		.callback_private       = NULL,
		.tm_table               = NULL,
		.query_geometry         = NULL,
		.display_accelerator    = NULL,
		.extension              = NULL,

		/* obsolete */
		.initialize_hook        = NULL,
		.set_values_hook        = NULL,
	},
	.primitive_class = {
		.highlight              = Highlight,
		.unhighlight            = Unhighlight,
		.press                  = Press,
		.unpress                = Unpress,
		.tooltip_post           = TooltipPost,
		.tooltip_unpost         = TooltipUnpost,
		.activate               = Activate,
		.draw                   = Draw,
	},
};

WidgetClass ctrlPrimitiveWidgetClass = (WidgetClass)&ctrlPrimitiveClassRec;

static void
ClassInitialize(void)
{
	_CtrlRegisterConverters();
}

static void
ClassPartInitialize(WidgetClass wc)
{
	CtrlPrimitiveWidgetClass c, s;      /* class, super */

	c = (CtrlPrimitiveWidgetClass)wc;
	s = (CtrlPrimitiveWidgetClass)c->core_class.superclass;
	if (c->primitive_class.highlight == (XtWidgetProc)_XtInherit)
		c->primitive_class.highlight = s->primitive_class.highlight;
	if (c->primitive_class.unhighlight == (XtWidgetProc)_XtInherit)
		c->primitive_class.unhighlight = s->primitive_class.unhighlight;
	if (c->primitive_class.press == (XtWidgetProc)_XtInherit)
		c->primitive_class.press = s->primitive_class.press;
	if (c->primitive_class.unpress == (XtWidgetProc)_XtInherit)
		c->primitive_class.unpress = s->primitive_class.unpress;
	if (c->primitive_class.tooltip_post == (XtWidgetProc)_XtInherit)
		c->primitive_class.tooltip_post = s->primitive_class.tooltip_post;
	if (c->primitive_class.tooltip_unpost == (XtWidgetProc)_XtInherit)
		c->primitive_class.tooltip_unpost = s->primitive_class.tooltip_unpost;
	if (c->primitive_class.activate == (XtActionProc)_XtInherit)
		c->primitive_class.activate = s->primitive_class.activate;
}

static void
Initialize(Widget rw, Widget nw, ArgList args, Cardinal *nargs)
{
	CtrlPrimitiveWidget request, new;
	Dimension incr;

	(void)args;
	(void)nargs;
	request = (CtrlPrimitiveWidget)rw;;
	new = (CtrlPrimitiveWidget)nw;;

	/* initialize the widget to its blank state */
	new->primitive.have_traversal = FALSE;
	new->primitive.highlighted = FALSE;
	new->primitive.focusable = FALSE;
	new->primitive.pressed = FALSE;
	new->primitive.is3d = FALSE;
	new->primitive.pixsave = None;

	/* check geometry; we should not create 0-sized widgets */
	incr = new->primitive.highlight_thickness * 2 + new->primitive.shadow_thickness * 2;
	if (request->core.width == 0)
		new->core.width += incr;
	if (request->core.height == 0)
		new->core.height += incr;

	/* alloc tooltip string */
	if (new->primitive.tooltip != NULL)
		new->primitive.tooltip = XtNewString(new->primitive.tooltip);

	_CtrlGetFontMetrics(
		XtWidgetToApplicationContext(nw),
		new->primitive.font,
		&new->primitive.font_average_width,
		&new->primitive.font_ascent,
		&new->primitive.font_descent,
		&new->primitive.font_height
	);
}

static void
Resize(Widget w)
{
	CtrlPrimitiveWidget primitivew;

	primitivew = (CtrlPrimitiveWidget)w;
	_CtrlNewPixmap(
		XtDisplay(w),
		&primitivew->primitive.pixsave,
		XtWindow(w),
		primitivew->core.width,
		primitivew->core.height,
		primitivew->core.depth
	);
	_CtrlDrawRectangle(
		XtDisplay(w),
		primitivew->primitive.pixsave,
		primitivew->core.background_pixmap,
		primitivew->core.background_pixel,
		0, 0,
		primitivew->core.width,
		primitivew->core.height
	);
	(*(((CtrlPrimitiveWidgetClass)(XtClass(w)))->primitive_class.draw))(w);
	_CtrlCommitPixmap(
		XtDisplay(w),
		XtWindow(w),
		primitivew->primitive.pixsave,
		primitivew->core.x,
		primitivew->core.y,
		primitivew->core.width,
		primitivew->core.height
	);
}

static void 
Realize(Widget w, XtValueMask *valuemask, XSetWindowAttributes *attrs)
{
	CtrlPrimitiveWidget primitivew;

	primitivew = (CtrlPrimitiveWidget)w;
	*valuemask |= CWDontPropagate;
	attrs->do_not_propagate_mask = ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | PointerMotionMask;
	if ((attrs->cursor = primitivew->primitive.cursor) != None)
		*valuemask |= CWCursor;
	(*ctrlPrimitiveWidgetClass->core_class.superclass->core_class.realize)(w, valuemask, attrs);
	(*(((CtrlPrimitiveWidgetClass)(XtClass(w)))->core_class.resize))(w);
}

static void
Destroy(Widget w)
{
	CtrlPrimitiveWidget primitivew;

#warning TODO: Delete widget traversal information
	primitivew = (CtrlPrimitiveWidget)w;
	if (primitivew->primitive.tooltip != NULL) {
		XtFree(primitivew->primitive.tooltip);
	}
}

static void
Redisplay(Widget w, XEvent *ev, Region r)
{
	CtrlPrimitiveWidget primitivew;

	(void)ev;
	(void)r;
	primitivew = (CtrlPrimitiveWidget)w;
	_CtrlCommitPixmap(
		XtDisplay(w),
		XtWindow(w),
		primitivew->primitive.pixsave,
		primitivew->core.x,
		primitivew->core.y,
		primitivew->core.width,
		primitivew->core.height
	);
}

static Boolean
SetValues(Widget cw, Widget rw, Widget nw, ArgList args, Cardinal *nargs)
{
	CtrlPrimitiveWidget old, new;

	(void)rw;
	(void)args;
	(void)nargs;
	old = (CtrlPrimitiveWidget)cw;
	new = (CtrlPrimitiveWidget)nw;

	if (old->primitive.tooltip != new->primitive.tooltip) {
		if (old->primitive.tooltip != NULL) {
			XtFree(old->primitive.tooltip);
		}
		if (new->primitive.tooltip != NULL) {
			new->primitive.tooltip = XtNewString(new->primitive.tooltip);
		}
	}
	if (XtIsSensitive(cw) && !XtIsSensitive(nw))
		(*(((CtrlPrimitiveWidgetClass)XtClass(nw))->primitive_class.tooltip_unpost))(nw);
	else if (old->primitive.tooltip != NULL && new->primitive.tooltip == NULL)
		(*(((CtrlPrimitiveWidgetClass)XtClass(nw))->primitive_class.tooltip_unpost))(nw);
	else if (old->primitive.tooltip == NULL && new->primitive.tooltip != NULL)
		(*(((CtrlPrimitiveWidgetClass)XtClass(nw))->primitive_class.tooltip_post))(nw);
	if ((old->primitive.cursor != new->primitive.cursor) && XtIsRealized(nw)) {
		if (new->primitive.cursor != None) {
			XDefineCursor(XtDisplay(nw), XtWindow(nw), new->primitive.cursor);
		} else {
			XUndefineCursor(XtDisplay(nw), XtWindow(nw));
		}
	}
#warning TODO: update keyboard traversals
	if (old->primitive.shadow_thickness != new->primitive.shadow_thickness ||
	    old->primitive.shadow_light_pixel != new->primitive.shadow_light_pixel ||
	    old->primitive.shadow_light_pixmap != new->primitive.shadow_light_pixmap ||
	    old->primitive.shadow_dark_pixel != new->primitive.shadow_dark_pixel ||
	    old->primitive.shadow_dark_pixmap != new->primitive.shadow_dark_pixmap ||
	    old->primitive.highlight_thickness != new->primitive.highlight_thickness ||
	    old->primitive.highlight_pixel != new->primitive.highlight_pixel ||
	    old->primitive.highlight_pixmap != new->primitive.highlight_pixmap) {
#warning TODO: redraw widget without damage drawings that subclasses may have done
		return TRUE;
	} else {
		return FALSE;
	}
}

static void
Draw(Widget w)
{
	CtrlPrimitiveWidget primitivew;

	primitivew = (CtrlPrimitiveWidget)w;
	if (primitivew->primitive.focusable) {
		_CtrlDrawHighlight(
			XtDisplay(w),
			primitivew->primitive.pixsave,
			(primitivew->primitive.highlighted)
			? primitivew->primitive.highlight_pixmap
			: primitivew->core.background_pixmap,
			(primitivew->primitive.highlighted)
			? primitivew->primitive.highlight_pixel
			: primitivew->core.background_pixel,
			0, 0,
			primitivew->core.width,
			primitivew->core.height,
			primitivew->primitive.highlight_thickness
		);
	}
	if (primitivew->primitive.is3d) {
		_CtrlDrawTopShadow(
			XtDisplay(w),
			primitivew->primitive.pixsave,
			(primitivew->primitive.pressed)
			? primitivew->primitive.shadow_dark_pixmap
			: primitivew->primitive.shadow_light_pixmap,
			(primitivew->primitive.pressed)
			? primitivew->primitive.shadow_dark_pixel
			: primitivew->primitive.shadow_light_pixel,
			0, 0,
			primitivew->core.width,
			primitivew->core.height,
			primitivew->primitive.shadow_thickness,
			primitivew->primitive.highlight_thickness
		);
		_CtrlDrawBottomShadow(
			XtDisplay(w),
			primitivew->primitive.pixsave,
			(primitivew->primitive.pressed)
			? primitivew->primitive.shadow_light_pixmap
			: primitivew->primitive.shadow_dark_pixmap,
			(primitivew->primitive.pressed)
			? primitivew->primitive.shadow_light_pixel
			: primitivew->primitive.shadow_dark_pixel,
			0, 0,
			primitivew->core.width,
			primitivew->core.height,
			primitivew->primitive.shadow_thickness,
			primitivew->primitive.highlight_thickness
		);
	}
}

static void
Highlight(Widget w)
{
	CtrlPrimitiveWidget primitivew;

	primitivew = (CtrlPrimitiveWidget)w;
	if (!primitivew->primitive.focusable)
		return;
	primitivew->primitive.highlighted = TRUE;
	_CtrlDrawHighlight(
		XtDisplay(w),
		primitivew->primitive.pixsave,
		primitivew->primitive.highlight_pixmap,
		primitivew->primitive.highlight_pixel,
		0, 0,
		primitivew->core.width,
		primitivew->core.height,
		primitivew->primitive.highlight_thickness
	);
	_CtrlCommitPixmap(
		XtDisplay(w),
		primitivew->primitive.pixsave,
		primitivew->primitive.pixsave,
		primitivew->core.x,
		primitivew->core.y,
		primitivew->core.width,
		primitivew->core.height
	);
}

static void
Unhighlight(Widget w)
{
	CtrlPrimitiveWidget primitivew;

	primitivew = (CtrlPrimitiveWidget)w;
	if (!primitivew->primitive.focusable)
		return;
	primitivew->primitive.highlighted = FALSE;
	_CtrlDrawHighlight(
		XtDisplay(w),
		primitivew->primitive.pixsave,
		primitivew->core.background_pixmap,
		primitivew->core.background_pixel,
		0, 0,
		primitivew->core.width,
		primitivew->core.height,
		primitivew->primitive.highlight_thickness
	);
	_CtrlCommitPixmap(
		XtDisplay(w),
		primitivew->primitive.pixsave,
		primitivew->primitive.pixsave,
		primitivew->core.x,
		primitivew->core.y,
		primitivew->core.width,
		primitivew->core.height
	);
}

static void
Press(Widget w)
{
	CtrlPrimitiveWidget primitivew;

	primitivew = (CtrlPrimitiveWidget)w;
	if (!primitivew->primitive.is3d)
		return;
	primitivew->primitive.pressed = TRUE;
	_CtrlDrawTopShadow(
		XtDisplay(w),
		primitivew->primitive.pixsave,
		primitivew->primitive.shadow_dark_pixmap,
		primitivew->primitive.shadow_dark_pixel,
		0, 0,
		primitivew->core.width,
		primitivew->core.height,
		primitivew->primitive.shadow_thickness,
		primitivew->primitive.highlight_thickness
	);
	_CtrlDrawBottomShadow(
		XtDisplay(w),
		primitivew->primitive.pixsave,
		primitivew->primitive.shadow_light_pixmap,
		primitivew->primitive.shadow_light_pixel,
		0, 0,
		primitivew->core.width,
		primitivew->core.height,
		primitivew->primitive.shadow_thickness,
		primitivew->primitive.highlight_thickness
	);
}

static void
Unpress(Widget w)
{
	CtrlPrimitiveWidget primitivew;

	primitivew = (CtrlPrimitiveWidget)w;
	if (!primitivew->primitive.is3d)
		return;
	primitivew->primitive.pressed = FALSE;
	_CtrlDrawTopShadow(
		XtDisplay(w),
		primitivew->primitive.pixsave,
		primitivew->primitive.shadow_light_pixmap,
		primitivew->primitive.shadow_light_pixel,
		0, 0,
		primitivew->core.width,
		primitivew->core.height,
		primitivew->primitive.shadow_thickness,
		primitivew->primitive.highlight_thickness
	);
	_CtrlDrawBottomShadow(
		XtDisplay(w),
		primitivew->primitive.pixsave,
		primitivew->primitive.shadow_dark_pixmap,
		primitivew->primitive.shadow_dark_pixel,
		0, 0,
		primitivew->core.width,
		primitivew->core.height,
		primitivew->primitive.shadow_thickness,
		primitivew->primitive.highlight_thickness
	);
}

static void
TooltipPost(Widget w)
{
#warning TODO: implement primitive_class.tooltip_post
	(void)w;
}

static void
TooltipUnpost(Widget w)
{
#warning TODO: implement primitive_class.tooltip_unpost
	(void)w;
}

static void
Activate(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
#warning TODO: implement primitive_class.activate
	(void)w;
	(void)ev;
	(void)params;
	(void)nparams;
}
