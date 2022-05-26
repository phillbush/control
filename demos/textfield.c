#include <X11/Shell.h>
#include <control.h>

static String fallbackresources[] = {
	"*CtrlTextField.value:          \"Hello World\"",
	NULL
};

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
		sessionShellWidgetClass,
		NULL, 0
	);
	(void)XtVaCreateManagedWidget(
		"textfield",
		ctrlTextFieldWidgetClass,
		shell,
		NULL
	);
	XtRealizeWidget(shell);
	XtAppMainLoop(app);
}
