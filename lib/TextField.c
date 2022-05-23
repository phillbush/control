#include <control/TextFieldP.h>

#include "ControlI.h"

/* core methods */
static void Realize(Widget, XtValueMask *, XSetWindowAttributes *);

/* help methods */
static void preeditstart(XIM, XPointer, XPointer);
static void preeditdone(XIM, XPointer, XPointer);
static void preeditdraw(XIM, XPointer, XPointer);
static void preeditcaret(XIM, XPointer, XPointer);

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
	(*ctrlTextFieldClassRec.core_class.superclass->core_class.realize)(w, valuemask, attrs);
	_CtrlInitializeInput(w);
	XmImSetValues(
		w,
		CtrlNpreeditStartCallback, &(XIMCallback) {
			.client_data = (XPointer)w,
			.callback = (XIMProc)preeditstart,
		},
		CtrlNpreeditDoneCallback, &(XIMCallback) {
			.client_data = (XPointer)w,
			.callback = (XIMProc)preeditdone,
		},
		CtrlNpreeditDrawCallback, &(XIMCallback) {
			.client_data = (XPointer)w,
			.callback = (XIMProc)preeditdraw,
		},
		CtrlNpreeditCaretCallback, &(XIMCallback) {
			.client_data = (XPointer)w,
			.callback = (XIMProc)preeditcaret,
		},
		NULL
	);
}
