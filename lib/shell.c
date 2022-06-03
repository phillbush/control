#include <stdlib.h>

#include <control_private.h>

#include "common.h"

#define TRANSLATIONS \
	"<Message>WM_PROTOCOLS:protocols()\n"

/* core methods */
static void ClassInitialize(void);
static void ClassPartInitialize(WidgetClass);
static void Destroy(Widget);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Realize(Widget, XtValueMask *, XSetWindowAttributes *);
static void Resize(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);

/* composite methods */
static XtGeometryResult GeometryManager(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
static void ChangeManaged(Widget);

/* shell methods */
static XtGeometryResult RootGeometryManager(Widget, XtWidgetGeometry *, XtWidgetGeometry *);

/* internal functions */
static void Focus(Widget, XtPointer, XEvent *, Boolean *);

/* actions */
static void Protocols(Widget, XEvent *, String *, Cardinal *);

/* shell callback functions */
static void PopupCallback(Widget, XtPointer, XtPointer);
static void PopdownCallback(Widget, XtPointer, XtPointer);

static XtActionsRec actions[] = {
	{"protocols",                   Protocols},
};

static XtResource resources[] = {
	{
		.resource_name          = CtrlNfocusFollowPointer,
		.resource_class         = CtrlCFocusFollowPointer,
		.resource_type          = CtrlRBoolean,
		.resource_size          = sizeof(Boolean),
		.resource_offset        = XtOffsetOf(CtrlShellRec, ctrl.follow_focus),
		.default_type           = CtrlRImmediate,
		.default_addr           = (XtPointer)FALSE,
	},
	{
		.resource_name          = CtrlNcloseCallback,
		.resource_class         = CtrlCCallback,
		.resource_type          = CtrlRCallback,
		.resource_size          = sizeof(XtCallbackList),
		.resource_offset        = XtOffsetOf(CtrlShellRec, ctrl.close_callback),
		.default_type           = CtrlRCallback,
		.default_addr           = (XtPointer)NULL,
	},
};

static CompositeClassExtensionRec compositeExt = {
	.next_extension                 = NULL,
	.record_type                    = NULLQUARK,
	.version                        = XtCompositeExtensionVersion,
	.record_size                    = sizeof(CompositeClassExtensionRec),
	.accepts_objects                = TRUE,
	.allows_change_managed_set      = FALSE,
};

static ShellClassExtensionRec shellExt = {
	.next_extension                 = NULL,
	.record_type                    = NULLQUARK,
	.version                        = XtShellExtensionVersion,
	.record_size                    = sizeof(ShellClassExtensionRec),
	.root_geometry_manager          = RootGeometryManager,
};

CtrlShellClassRec ctrlShellClassRec = {
	.core_class = {
		.superclass             = (WidgetClass)&wmShellClassRec,
		.class_name             = "CtrlShell",
		.widget_size            = sizeof(CtrlShellRec),
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
		.compress_motion        = FALSE,
		.compress_exposure      = XtExposeCompressSeries,
		.compress_enterleave    = FALSE,
		.visible_interest       = FALSE,
		.destroy                = Destroy,
		.resize                 = Resize,
		.expose                 = NULL,
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
	.composite_class = {
		.geometry_manager       = GeometryManager,
		.change_managed         = ChangeManaged,
		.insert_child           = XtInheritInsertChild,
		.delete_child           = XtInheritDeleteChild,
		.extension              = &compositeExt,
	},
	.shell_class = {
		.extension              = &shellExt,
	},
	.wm_shell_class = {
		.extension              = NULL,
	},
	.vendor_shell_class = {
		.extension              = NULL,
	},
	.top_level_shell_class = {
		.extension              = NULL,
	},
	.application_shell_class = {
		.extension              = NULL,
	},
	.session_shell_class = {
		.extension              = NULL,
	},
	.ctrl_shell_class = {
		0,
	},
};

WidgetClass ctrlShellWidgetClass = (WidgetClass)&ctrlShellClassRec;

static void
ClassInitialize(void)
{
	_CtrlRegisterConverters();
}

static void
ClassPartInitialize(WidgetClass wc)
{
	CtrlShellWidgetClass shellclass;
	CompositeClassExtension ext_composite;
	ShellClassExtension ext_shell;

	shellclass = (CtrlShellWidgetClass)wc;
	ext_composite = (CompositeClassExtension)XtGetClassExtension(
		wc,
		XtOffsetOf(CompositeClassRec, composite_class.extension),
		NULLQUARK,
		1L,
		0
	);
	ext_shell = (ShellClassExtension)XtGetClassExtension(
		wc,
		XtOffsetOf(ShellClassRec, shell_class.extension),
		NULLQUARK,
		1L,
		0
	);
	if (ext_composite == NULL) {
		shellclass->composite_class.extension = &compositeExt;
	}
	if (ext_shell == NULL) {
		shellclass->shell_class.extension = &shellExt;
	}
}

static void
Destroy(Widget w)
{
	CtrlShellWidget shell;
	XtAppContext app;

	shell = (CtrlShellWidget)w;
	app = XtWidgetToApplicationContext(w);
	XtAppLock(app);
	XCloseIM(shell->ctrl.xim);
	XtAppUnlock(app);
}

static void
Initialize(Widget rw, Widget nw, ArgList args, Cardinal *nargs)
{
	CtrlShellWidget shell;
	XtAppContext app;
	XIMStyle preeditstyle;
	XIMStyle statusstyle;
	XIMStyles *styles;
	unsigned int i;

	(void)rw;
	(void)args;
	(void)nargs;
	shell = (CtrlShellWidget)nw;
	app = XtWidgetToApplicationContext(nw);
	XtAppLock(app);
	XtInsertEventHandler(
		nw,
		FocusChangeMask | EnterWindowMask | LeaveWindowMask,
		FALSE,
		Focus,
		(XtPointer)nw,
		XtListHead
	);
	XtAddCallback(nw, CtrlNpopupCallback, PopupCallback, (XtPointer)nw);
	XtAddCallback(nw, CtrlNpopdownCallback, PopdownCallback, (XtPointer)nw);
	preeditstyle = XIMPreeditNothing;
	statusstyle = XIMStatusNothing;                 /* we do not do status area (should we?) */
	if ((shell->ctrl.xim = XOpenIM(XtDisplay(nw), NULL, NULL, NULL)) == NULL)
		goto error;
	if (XGetIMValues(shell->ctrl.xim, XNQueryInputStyle, &styles, NULL) != NULL)
		goto error;
	for (i = 0; i < styles->count_styles; i++) {
		if (styles->supported_styles[i] & XIMPreeditCallbacks) {
			preeditstyle = XIMPreeditCallbacks;
			break;
		}
	}
	shell->ctrl.pref_style = preeditstyle | statusstyle;
	XtAppUnlock(app);
	return;
error:
	if (shell->ctrl.xim != NULL)
		XCloseIM(shell->ctrl.xim);
	XtAppUnlock(app);
	ERROR(app, "noInputMethod", "could not open input method");
	return;
}

static void
Realize(Widget w, XtValueMask *valuemask, XSetWindowAttributes *attrs)
{
	static XtTranslations translations = NULL;
	CtrlShellWidget shell;
	XtAppContext app;
	Atom wm_delete_window;

	shell = (CtrlShellWidget)w;
	app = XtWidgetToApplicationContext(w);
	if (shell->core.width == 0)
		shell->core.width = 10;
	if (shell->core.height == 0)
		shell->core.height = 10;
	(*wmShellWidgetClass->core_class.realize)(w, valuemask, attrs);
	XtAppLock(app);
	wm_delete_window = XInternAtom(XtDisplay(w), WM_DELETE_WINDOW, False);
	if (translations == NULL)
		translations = XtParseTranslationTable(TRANSLATIONS);
	XtOverrideTranslations(w, translations);
	(void)XSetWMProtocols(XtDisplay(w), XtWindow(w), &wm_delete_window, 1);
	XtAppUnlock(app);
}

static void
Resize(Widget w)
{
	CtrlShellWidget shell;
	Cardinal i;

	shell = (CtrlShellWidget)w;
	for (i = 0; i < shell->composite.num_children; i++) {
		if (XtIsManaged(shell->composite.children[i])) {
			XtResizeWidget(
				shell->composite.children[i],
				shell->core.width,
				shell->core.height,
				BORDER_WIDTH
			);
		}
	}
}

static Boolean
SetValues(Widget cw, Widget rw, Widget nw, ArgList args, Cardinal *nargs)
{
	CtrlShellWidget oldsw, newsw;

	(void)rw;
	(void)args;
	(void)nargs;
	oldsw = (CtrlShellWidget)cw;
	newsw = (CtrlShellWidget)nw;
	if (newsw->core.being_destroyed)
		goto done;
	if (newsw->ctrl.follow_focus != oldsw->ctrl.follow_focus) {
#warning TODO: handle focus policy change
	}
done:
	return FALSE;
}

static XtGeometryResult
GeometryManager(Widget w, XtWidgetGeometry *request, XtWidgetGeometry *reply)
{
	(void)w;
	(void)request;
	(void)reply;
	return XtGeometryNo;
}

static void
ChangeManaged(Widget w)
{
	CtrlShellWidget shell;
	Cardinal i;

	shell = (CtrlShellWidget)w;
	(*wmShellClassRec.composite_class.change_managed)(w);
	for (i = 0; i < shell->composite.num_children; i++) {
		if (XtIsManaged(shell->composite.children[i])) {
			XtSetKeyboardFocus(w, shell->composite.children[i]);
			break;
		}
	}
#warning TODO: see how motif does with XmeNavigChangeManaged
}

static XtGeometryResult
RootGeometryManager(Widget w, XtWidgetGeometry *request, XtWidgetGeometry *reply)
{
	(void)w;
	(void)request;
	(void)reply;
	return XtGeometryNo;
}

static void
Focus(Widget w, XtPointer clientdata, XEvent *ev, Boolean *dontSwallow)
{
	(void)w;
	(void)clientdata;
	(void)ev;
	(void)dontSwallow;
#warning TODO: Implement Focus
}

static void
Protocols(Widget w, XEvent *ev, String *params, Cardinal *nparams)
{
	CtrlShellWidget shell;
	CtrlGenericCallData cd;
	Atom wm_delete_window;
	Atom wm_protocols;

	(void)params;
	(void)nparams;
	shell = (CtrlShellWidget)w;
	wm_delete_window = XInternAtom(XtDisplay(w), WM_DELETE_WINDOW, False);
	wm_protocols = XInternAtom(XtDisplay(w), WM_PROTOCOLS, False);
	if (ev->type != ClientMessage ||
	    ev->xclient.message_type != wm_protocols ||
	    (Atom)ev->xclient.data.l[0] != wm_delete_window)
		return;
	if (shell->ctrl.close_callback != NULL) {
		cd = (CtrlGenericCallData){
			.reason = CTRL_CLOSE,
			.event = ev,
		};
		XtCallCallbackList(w, shell->ctrl.close_callback, (XtPointer)&cd);
	} else {
		XtDestroyApplicationContext(XtWidgetToApplicationContext(w));
		exit(0);
	}
}

static void
PopupCallback(Widget w, XtPointer closure, XtPointer calldata)
{
	(void)w;
	(void)closure;
	(void)calldata;
#warning TODO: implement PopupCallback
}

static void
PopdownCallback(Widget w, XtPointer closure, XtPointer calldata)
{
	(void)w;
	(void)closure;
	(void)calldata;
#warning TODO: implement PopdownCallback
}
