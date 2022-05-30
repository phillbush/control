#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>

/* sizes */
#define ATOM_SIZE               32
#define UTF8_SIZE               8

/* selections */
#define CLIPBOARD               "CLIPBOARD"
#define UTF8_STRING             "UTF8_STRING"
#define CLIPBOARD               "CLIPBOARD"
#define TARGETS                 "TARGETS"
#define WM_DELETE_WINDOW        "WM_DELETE_WINDOW"

/* util macros */
#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))
#define FREE(p) XtFree((char *)(p))
#define WARN(app, name, msg)  XtAppWarningMsg((app), (name), __func__, "ToolkitError", (msg), NULL, 0)
#define ERROR(app, name, msg) XtAppErrorMsg((app), (name), __func__, "ToolkitError", (msg), NULL, 0)

/* resource default values */
#define DEF_THICKNESS           2
#define DEF_TEXT_COLUMNS        20
#define DEF_TEXT_MARGIN         5
#define DEF_BLINK_RATE          500
#define DEF_SELECT_THRESHOLD    5
#define DEF_FONT                "monospace:size=9"
#define DEF_FOREGROUND          "#FFFFFF"
#define DEF_SELFOREGROUND       "#FFFFFF"
#define DEF_SELBACKGROUND       "#3465A4"
#define DEF_SHADOW_LIGHT_COLOR  "#737373"
#define DEF_SHADOW_DARK_COLOR   "#101010"
#define DEF_HIGHLIGHT_COLOR     "#C5C8C6"

/* routines from util.c */
void _CtrlOwnSelection(Widget, XtConvertSelectionProc, Atom, Time);
void _CtrlRegisterConverters(void);
void _CtrlNewPixmap(Display *, Pixmap *, Window, Dimension, Dimension, Cardinal);
void _CtrlDrawRectangle(Display *, Pixmap, Pixmap, Pixel, int, int, Dimension, Dimension);
void _CtrlDrawXftRectangle(Display *, Pixmap, XtPointer, int, int, Dimension, Dimension);
void _CtrlDrawHighlight(Display *, Pixmap, Pixmap, Pixel, int, int, Dimension, Dimension, Dimension);
void _CtrlDrawTopShadow(Display *, Pixmap, Pixmap, Pixel, int, int, Dimension, Dimension, Dimension, Dimension);
void _CtrlDrawBottomShadow(Display *, Pixmap, Pixmap, Pixel, int, int, Dimension, Dimension, Dimension, Dimension);
void _CtrlDrawText(Display *, Pixmap, XtPointer, XtPointer, int, int, String, Cardinal);
void _CtrlCommitPixmap(Display *, Window, Pixmap, int, int, Dimension, Dimension);
void _CtrlGetFontMetrics(XtAppContext, XtPointer, Dimension *, Dimension *, Dimension *, Dimension *);
Dimension _CtrlGetTextWidth(XtPointer, String, Cardinal);
XIC _CtrlGetInputContext(Widget, XICProc, XICProc, XICProc, XICProc, XICProc);
XtGeometryResult _CtrlReplyToQueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
Status _CtrlLookupString(Display *, XIC, XEvent *, String, int, int *);
int _CtrlNextRune(String, int, int);
int _CtrlRuneBytes(String, int);
int _CtrlRuneChars(String, int);
int _CtrlMoveWordEdge(String, int, int);
Atom _CtrlInternAtom(Display *, String);
