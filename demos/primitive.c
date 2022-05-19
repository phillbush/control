#include <X11/Shell.h>
#include <control/Primitive.h>

static String fallbackresources[] = {
	"*CtrlPrimitive.cursor:             pirate",
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
		"primitive",
		ctrlPrimitiveWidgetClass,
		shell,
		CtrlNfocusable, TRUE,
		CtrlNpressable, TRUE,
		CtrlNwidth,     100,
		CtrlNheight,    100,
		NULL
	);
	XtRealizeWidget(shell);
	XtAppMainLoop(app);
}
