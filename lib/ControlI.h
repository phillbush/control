#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>

/* resource default values */
#define DEF_THICKNESS           2
#define DEF_FOREGROUND          "#FFFFFF"
#define DEF_SHADOW_LIGHT_COLOR  "#737373"
#define DEF_SHADOW_DARK_COLOR   "#101010"
#define DEF_HIGHLIGHT_COLOR     "#C5C8C6"

void _CtrlInitializeConverters(void);
void _CtrlNewPixmap(Display *, Pixmap *, Window, Dimension, Dimension, Cardinal);
void _CtrlDrawRectangle(Display *, Pixmap, Pixmap, Pixel, Position, Position, Dimension, Dimension);
void _CtrlDrawHighlight(Display *, Pixmap, Pixmap, Pixel, Position, Position, Dimension, Dimension, Dimension);
void _CtrlDrawTopShadow(Display *, Pixmap, Pixmap, Pixel, Position, Position, Dimension, Dimension, Dimension, Dimension);
void _CtrlDrawBottomShadow(Display *, Pixmap, Pixmap, Pixel, Position, Position, Dimension, Dimension, Dimension, Dimension);
void _CtrlCommitPixmap(Display *, Window, Pixmap, Position, Position, Dimension, Dimension);
struct _CtrlInputMethodRec *_CtrlGetInputMethod(Display *dpy);
struct _CtrlInputContextRec *_CtrlGetInputContext(Display *dpy, Widget w, XICProc start, XICProc done, XICProc draw, XICProc caret, XICProc destroy);
