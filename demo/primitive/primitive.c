#include <X11/Shell.h>

#include <control/Primitive.h>

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
		NULL,
		sessionShellWidgetClass,
		NULL, 0
	);
	(void)XtVaCreateManagedWidget(
		"primitive",
		ctrlPrimitiveWidgetClass,
		shell,
		XtNwidth, 10,
		XtNheight, 10,
		NULL
	);
	XtRealizeWidget(shell);
	XtAppMainLoop(app);
}
