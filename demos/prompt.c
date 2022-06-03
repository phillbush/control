#include <stdio.h>
#include <stdlib.h>

#include <X11/Shell.h>
#include <control.h>

static String fallbackresources[] = {
	"*CtrlTextField.value:          Hello World",
	"*CtrlTextField.columns:        50",
	NULL
};

static void
printandexit(Widget w, XtPointer client_data, XtPointer call_data)
{
	String str;

	(void)client_data;
	(void)call_data;
	XtVaGetValues(w, CtrlNvalue, &str, NULL);
	printf("%s\n", str);
	XtDestroyApplicationContext(XtWidgetToApplicationContext(w));
	exit(0);
}

int
main(int argc, char *argv[])
{
	XtAppContext app;
	Widget shell;

	XtSetLanguageProc(NULL, NULL, NULL);
	shell = XtOpenApplication(
		&app,
		"Demo",
		NULL, 0,
		&argc, argv,
		fallbackresources,
		ctrlShellWidgetClass,
		NULL, 0
	);
	(void)XtVaCreateManagedWidget(
		"textfield",
		ctrlTextFieldWidgetClass,
		shell,
		CtrlNactivateCallback, (XtCallbackRec[]){
			{ .callback = printandexit, .closure = NULL },
			{ .callback = NULL,         .closure = NULL },
		},
		NULL
	);
	XtRealizeWidget(shell);
	XtAppMainLoop(app);
}
