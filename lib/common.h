#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>

/* util macros */
#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))

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
void _CtrlRegisterConverters(void);
void _CtrlNewPixmap(Display *, Pixmap *, Window, Dimension, Dimension, Cardinal);
void _CtrlDrawRectangle(Display *, Pixmap, Pixmap, Pixel, Position, Position, Dimension, Dimension);
void _CtrlDrawXftRectangle(Display *, Pixmap, XtPointer, Position, Position, Dimension, Dimension);
void _CtrlDrawHighlight(Display *, Pixmap, Pixmap, Pixel, Position, Position, Dimension, Dimension, Dimension);
void _CtrlDrawTopShadow(Display *, Pixmap, Pixmap, Pixel, Position, Position, Dimension, Dimension, Dimension, Dimension);
void _CtrlDrawBottomShadow(Display *, Pixmap, Pixmap, Pixel, Position, Position, Dimension, Dimension, Dimension, Dimension);
void _CtrlDrawText(Display *, Pixmap, XtPointer, XtPointer, Position, Position, String, Cardinal);
void _CtrlCommitPixmap(Display *, Window, Pixmap, Position, Position, Dimension, Dimension);
void _CtrlGetFontMetrics(XtAppContext, XtPointer, Dimension *, Dimension *, Dimension *, Dimension *);
Dimension _CtrlGetTextWidth(XtPointer, String, Cardinal);
XIC _CtrlGetInputContext(Widget, XICProc, XICProc, XICProc, XICProc, XICProc);
XtGeometryResult _CtrlReplyToQueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
Status _CtrlLookupString(Display *, XIC, XEvent *, String, int, int *);
Position _CtrlNextRune(String text, Position position, int inc);
