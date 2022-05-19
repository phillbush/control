#include <string.h>

#include <control/PrimitiveP.h>

#include "ControlI.h"

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

/* helper functions */
static void Draw(Widget);

static XtActionsRec actions[] = {
#warning TODO: Fill in Primitive actions[] after implement widget traversal;
};

static XtResource resources[] = {
	{
		.resource_name   = CtrlNforeground,
		.resource_class  = CtrlCForeground,
		.resource_type   = CtrlRPixel,
		.resource_size   = sizeof(Pixel),
		.resource_offset = XtOffsetOf(CtrlPrimitiveRec, primitive.foreground),
		.default_type    = CtrlRString,
		.default_addr    = (XtPointer)DEF_FOREGROUND,
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
		.translations           = NULL,
#warning TODO: implement translations
	},
};

WidgetClass ctrlPrimitiveWidgetClass = (WidgetClass)&ctrlPrimitiveClassRec;

static void
ClassInitialize(void)
{
	InitializeConverters();
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
	if (c->primitive_class.translations == XtInheritTranslations)
		c->primitive_class.translations = s->primitive_class.translations;
	if (c->primitive_class.activate == (XtActionProc)_XtInherit)
		c->primitive_class.activate = s->primitive_class.activate;
}

static void
Initialize(Widget rw, Widget nw, ArgList args, Cardinal *nargs)
{
	CtrlPrimitiveWidget request, new;

	(void)args;
	(void)nargs;
	request = (CtrlPrimitiveWidget)rw;;
	new = (CtrlPrimitiveWidget)nw;;

	/* initialize the widget to its blank state */
	new->primitive.have_traversal = FALSE;
	new->primitive.pressed = FALSE;
	new->primitive.highlighted = FALSE;
	new->primitive.pixsave = None;

	/* check geometry; we should not create 0-sized widgets */
	if (request->core.width == 0)
		new->core.width += new->primitive.highlight_thickness * 2 + new->primitive.shadow_thickness * 2;
	if (request->core.height == 0)
		new->core.height += new->primitive.highlight_thickness * 2 + new->primitive.shadow_thickness * 2;

	/* alloc tooltip string */
	if (new->primitive.tooltip != NULL)
		new->primitive.tooltip = XtNewString(new->primitive.tooltip);
}

static void
Resize(Widget w)
{
	CtrlPrimitiveWidget widget;

	widget = (CtrlPrimitiveWidget)w;
	NewPixmap(
		XtDisplay(w),
		&widget->primitive.pixsave,
		XtWindow(w),
		widget->core.width,
		widget->core.height,
		widget->core.depth
	);
	Draw(w);
	CommitPixmap(
		XtDisplay(w),
		XtWindow(w),
		widget->primitive.pixsave,
		widget->core.x,
		widget->core.y,
		widget->core.width,
		widget->core.height
	);
}

static void 
Realize(Widget w, XtValueMask *valuemask, XSetWindowAttributes *attrs)
{
	*valuemask |= CWDontPropagate;
	attrs->do_not_propagate_mask = ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | PointerMotionMask;
	if ((attrs->cursor = ((CtrlPrimitiveWidget)w)->primitive.cursor) != None)
		*valuemask |= CWCursor;
	XtCreateWindow(w, InputOutput, CopyFromParent, *valuemask, attrs);
	Resize(w);
}

static void
Destroy(Widget w)
{
	CtrlPrimitiveWidget widget;

#warning TODO: Delete widget traversal information
	widget = (CtrlPrimitiveWidget)w;
	if (widget->primitive.tooltip != NULL) {
		XtFree(widget->primitive.tooltip);
	}
}

static void
Redisplay(Widget w, XEvent *ev, Region r)
{
	CtrlPrimitiveWidget widget;

	(void)ev;
	(void)r;
	widget = (CtrlPrimitiveWidget)w;
	CommitPixmap(
		XtDisplay(w),
		XtWindow(w),
		widget->primitive.pixsave,
		widget->core.x,
		widget->core.y,
		widget->core.width,
		widget->core.height
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
	return old->primitive.shadow_thickness != new->primitive.shadow_thickness ||
	       old->primitive.shadow_light_pixel != new->primitive.shadow_light_pixel ||
	       old->primitive.shadow_light_pixmap != new->primitive.shadow_light_pixmap ||
	       old->primitive.shadow_dark_pixel != new->primitive.shadow_dark_pixel ||
	       old->primitive.shadow_dark_pixmap != new->primitive.shadow_dark_pixmap ||
	       old->primitive.highlight_thickness != new->primitive.highlight_thickness ||
	       old->primitive.highlight_pixel != new->primitive.highlight_pixel ||
	       old->primitive.highlight_pixmap != new->primitive.highlight_pixmap;
}

static void
Draw(Widget w)
{
	CtrlPrimitiveWidget widget;

	widget = (CtrlPrimitiveWidget)w;
	DrawRectangle(
		XtDisplay(w),
		widget->primitive.pixsave,
		widget->core.background_pixmap,
		widget->core.background_pixel,
		0, 0,
		widget->core.width,
		widget->core.height
	);
	DrawHighlight(
		XtDisplay(w),
		widget->primitive.pixsave,
		(widget->primitive.highlighted)
		? widget->primitive.highlight_pixmap
		: widget->core.background_pixmap,
		(widget->primitive.highlighted)
		? widget->primitive.highlight_pixel
		: widget->core.background_pixel,
		0, 0,
		widget->core.width,
		widget->core.height,
		widget->primitive.highlight_thickness
	);
	DrawTopShadow(
		XtDisplay(w),
		widget->primitive.pixsave,
		(widget->primitive.pressed)
		? widget->primitive.shadow_dark_pixmap
		: widget->primitive.shadow_light_pixmap,
		(widget->primitive.pressed)
		? widget->primitive.shadow_dark_pixel
		: widget->primitive.shadow_light_pixel,
		0, 0,
		widget->core.width,
		widget->core.height,
		widget->primitive.shadow_thickness,
		widget->primitive.highlight_thickness
	);
	DrawBottomShadow(
		XtDisplay(w),
		widget->primitive.pixsave,
		(widget->primitive.pressed)
		? widget->primitive.shadow_light_pixmap
		: widget->primitive.shadow_dark_pixmap,
		(widget->primitive.pressed)
		? widget->primitive.shadow_light_pixel
		: widget->primitive.shadow_dark_pixel,
		0, 0,
		widget->core.width,
		widget->core.height,
		widget->primitive.shadow_thickness,
		widget->primitive.highlight_thickness
	);
}

static void
Highlight(Widget w)
{
	CtrlPrimitiveWidget widget;

	widget = (CtrlPrimitiveWidget)w;
	widget->primitive.highlighted = TRUE;
	DrawHighlight(
		XtDisplay(w),
		widget->primitive.pixsave,
		widget->primitive.highlight_pixmap,
		widget->primitive.highlight_pixel,
		0, 0,
		widget->core.width,
		widget->core.height,
		widget->primitive.highlight_thickness
	);
	CommitPixmap(
		XtDisplay(w),
		widget->primitive.pixsave,
		widget->primitive.pixsave,
		widget->core.x,
		widget->core.y,
		widget->core.width,
		widget->core.height
	);
}

static void
Unhighlight(Widget w)
{
	CtrlPrimitiveWidget widget;

	widget = (CtrlPrimitiveWidget)w;
	widget->primitive.highlighted = FALSE;
	DrawHighlight(
		XtDisplay(w),
		widget->primitive.pixsave,
		widget->core.background_pixmap,
		widget->core.background_pixel,
		0, 0,
		widget->core.width,
		widget->core.height,
		widget->primitive.highlight_thickness
	);
	CommitPixmap(
		XtDisplay(w),
		widget->primitive.pixsave,
		widget->primitive.pixsave,
		widget->core.x,
		widget->core.y,
		widget->core.width,
		widget->core.height
	);
}

static void
Press(Widget w)
{
	CtrlPrimitiveWidget widget;

	widget = (CtrlPrimitiveWidget)w;
	widget->primitive.pressed = TRUE;
	DrawTopShadow(
		XtDisplay(w),
		widget->primitive.pixsave,
		widget->primitive.shadow_dark_pixmap,
		widget->primitive.shadow_dark_pixel,
		0, 0,
		widget->core.width,
		widget->core.height,
		widget->primitive.shadow_thickness,
		widget->primitive.highlight_thickness
	);
	DrawBottomShadow(
		XtDisplay(w),
		widget->primitive.pixsave,
		widget->primitive.shadow_light_pixmap,
		widget->primitive.shadow_light_pixel,
		0, 0,
		widget->core.width,
		widget->core.height,
		widget->primitive.shadow_thickness,
		widget->primitive.highlight_thickness
	);
}

static void
Unpress(Widget w)
{
	CtrlPrimitiveWidget widget;

	widget = (CtrlPrimitiveWidget)w;
	widget->primitive.pressed = FALSE;
	DrawTopShadow(
		XtDisplay(w),
		widget->primitive.pixsave,
		widget->primitive.shadow_light_pixmap,
		widget->primitive.shadow_light_pixel,
		0, 0,
		widget->core.width,
		widget->core.height,
		widget->primitive.shadow_thickness,
		widget->primitive.highlight_thickness
	);
	DrawBottomShadow(
		XtDisplay(w),
		widget->primitive.pixsave,
		widget->primitive.shadow_dark_pixmap,
		widget->primitive.shadow_dark_pixel,
		0, 0,
		widget->core.width,
		widget->core.height,
		widget->primitive.shadow_thickness,
		widget->primitive.highlight_thickness
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
