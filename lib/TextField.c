#include <control/TextFieldP.h>

#include "ControlI.h"

/* core methods */
static void Realize(Widget, XtValueMask *, XSetWindowAttributes *);

/* help methods */
static int PreeditStart(XIC, XPointer, XPointer);
static int PreeditDone(XIC, XPointer, XPointer);
static int PreeditDraw(XIC, XPointer, XPointer);
static int PreeditCaret(XIC, XPointer, XPointer);
static int PreeditDestroy(XIC, XPointer, XPointer);

CtrlTextFieldClassRec ctrlTextFieldClassRec = {
	.core_class = {
		.superclass             = (WidgetClass)&ctrlPrimitiveClassRec,
		.class_name             = "CtrlTextField",
		.widget_size            = sizeof(CtrlTextFieldRec),
		.class_initialize       = NULL,
		.class_part_initialize  = NULL,
		.class_inited           = FALSE,
		.realize                = Realize,
	},
	.text_class = {
	},
};

WidgetClass ctrlTextFieldWidgetClass = (WidgetClass)&ctrlTextFieldClassRec;

static void 
Realize(Widget w, XtValueMask *valuemask, XSetWindowAttributes *attrs)
{
	XtRealizeProc realize;

	realize = ctrlTextFieldWidgetClass->core_class.superclass->core_class.realize;
	(*realize)(w, valuemask, attrs);
	(void)_CtrlGetInputContext(
		XtDisplay(w), w,
		PreeditStart,
		PreeditDone,
		PreeditDraw,
		PreeditCaret,
		PreeditDestroy
	);
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
