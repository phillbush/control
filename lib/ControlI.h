/* resource names */

/* resource default values */
#define DEF_THICKNESS           2
#define DEF_FOREGROUND          "#FFFFFF"
#define DEF_SHADOW_LIGHT_COLOR  "#737373"
#define DEF_SHADOW_DARK_COLOR   "#101010"
#define DEF_HIGHLIGHT_COLOR     "#C5C8C6"

void InitializeConverters(void);
void NewPixmap(Display *, Pixmap *, Window, Dimension, Dimension, Cardinal);
void DrawRectangle(Display *, Pixmap, Pixmap, Pixel, Position, Position, Dimension, Dimension);
void DrawHighlight(Display *, Pixmap, Pixmap, Pixel, Position, Position, Dimension, Dimension, Dimension);
void DrawTopShadow(Display *, Pixmap, Pixmap, Pixel, Position, Position, Dimension, Dimension, Dimension, Dimension);
void DrawBottomShadow(Display *, Pixmap, Pixmap, Pixel, Position, Position, Dimension, Dimension, Dimension, Dimension);
void CommitPixmap(Display *, Window, Pixmap, Position, Position, Dimension, Dimension);
