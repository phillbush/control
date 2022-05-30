#ifndef _CONTROL_H
#define _CONTROL_H

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Core.h>

/* resource name definitions */
#define CtrlNaccelerators               XtNaccelerators
#define CtrlNallowHoriz                 XtNallowHoriz
#define CtrlNallowVert                  XtNallowVert
#define CtrlNancestorSensitive          XtNancestorSensitive
#define CtrlNbackground                 XtNbackground
#define CtrlNbackgroundPixmap           XtNbackgroundPixmap
#define CtrlNbitmap                     XtNbitmap
#define CtrlNborder                     XtNborder
#define CtrlNborderColor                XtNborderColor
#define CtrlNborderPixmap               XtNborderPixmap
#define CtrlNborderWidth                XtNborderWidth
#define CtrlNcallback                   XtNcallback
#define CtrlNchangeHook                 XtNchangeHook
#define CtrlNchildren                   XtNchildren
#define CtrlNcolormap                   XtNcolormap
#define CtrlNconfigureHook              XtNconfigureHook
#define CtrlNcreateHook                 XtNcreateHook
#define CtrlNdepth                      XtNdepth
#define CtrlNdestroyCallback            XtNdestroyCallback
#define CtrlNdestroyHook                XtNdestroyHook
#define CtrlNeditType                   XtNeditType
#define CtrlNfile                       XtNfile
#define CtrlNfont                       XtNfont
#define CtrlNfontSet                    XtNfontSet
#define CtrlNforceBars                  XtNforceBars
#define CtrlNforeground                 XtNforeground
#define CtrlNfunction                   XtNfunction
#define CtrlNgeometryHook               XtNgeometryHook
#define CtrlNhSpace                     XtNhSpace
#define CtrlNheight                     XtNheight
#define CtrlNhighlight                  XtNhighlight
#define CtrlNindex                      XtNindex
#define CtrlNinitialResourcesPersistent XtNinitialResourcesPersistent
#define CtrlNinnerHeight                XtNinnerHeight
#define CtrlNinnerWidth                 XtNinnerWidth
#define CtrlNinnerWindow                XtNinnerWindow
#define CtrlNinsertPosition             XtNinsertPosition
#define CtrlNinternalHeight             XtNinternalHeight
#define CtrlNinternalWidth              XtNinternalWidth
#define CtrlNjumpProc                   XtNjumpProc
#define CtrlNjustify                    XtNjustify
#define CtrlNknobHeight                 XtNknobHeight
#define CtrlNknobIndent                 XtNknobIndent
#define CtrlNknobPixel                  XtNknobPixel
#define CtrlNknobWidth                  XtNknobWidth
#define CtrlNlabel                      XtNlabel
#define CtrlNlength                     XtNlength
#define CtrlNlowerRight                 XtNlowerRight
#define CtrlNmappedWhenManaged          XtNmappedWhenManaged
#define CtrlNmenuEntry                  XtNmenuEntry
#define CtrlNname                       XtNname
#define CtrlNnotify                     XtNnotify
#define CtrlNnumChildren                XtNnumChildren
#define CtrlNnumShells                  XtNnumShells
#define CtrlNorientation                XtNorientation
#define CtrlNparameter                  XtNparameter
#define CtrlNpixmap                     XtNpixmap
#define CtrlNpopdownCallback            XtNpopdownCallback
#define CtrlNpopupCallback              XtNpopupCallback
#define CtrlNresize                     XtNresize
#define CtrlNreverseVideo               XtNreverseVideo
#define CtrlNscreen                     XtNscreen
#define CtrlNscrollDCursor              XtNscrollDCursor
#define CtrlNscrollHCursor              XtNscrollHCursor
#define CtrlNscrollLCursor              XtNscrollLCursor
#define CtrlNscrollProc                 XtNscrollProc
#define CtrlNscrollRCursor              XtNscrollRCursor
#define CtrlNscrollUCursor              XtNscrollUCursor
#define CtrlNscrollVCursor              XtNscrollVCursor
#define CtrlNselection                  XtNselection
#define CtrlNselectionArray             XtNselectionArray
#define CtrlNsensitive                  XtNsensitive
#define CtrlNshells                     XtNshells
#define CtrlNshown                      XtNshown
#define CtrlNspace                      XtNspace
#define CtrlNstring                     XtNstring
#define CtrlNtextOptions                XtNtextOptions
#define CtrlNtextSink                   XtNtextSink
#define CtrlNtextSource                 XtNtextSource
#define CtrlNthickness                  XtNthickness
#define CtrlNthumb                      XtNthumb
#define CtrlNthumbProc                  XtNthumbProc
#define CtrlNtop                        XtNtop
#define CtrlNtranslations               XtNtranslations
#define CtrlNunrealizeCallback          XtNunrealizeCallback
#define CtrlNupdate                     XtNupdate
#define CtrlNuseBottom                  XtNuseBottom
#define CtrlNuseRight                   XtNuseRight
#define CtrlNvSpace                     XtNvSpace
#define CtrlNvalue                      XtNvalue
#define CtrlNwidth                      XtNwidth
#define CtrlNwindow                     XtNwindow
#define CtrlNx                          XtNx
#define CtrlNy                          XtNy
#define CtrlNcursor                     "cursor"
#define CtrlNtooltip                    "tooltip"
#define CtrlNshadowThickness            "shadowThickness"
#define CtrlNshadowLightColor           "shadowLightColor"
#define CtrlNshadowLightPixmap          "shadowLightPixmap"
#define CtrlNshadowDarkColor            "shadowDarkColor"
#define CtrlNshadowDarkPixmap           "shadowDarkPixmap"
#define CtrlNhighlightThickness         "highlightThickness"
#define CtrlNhighlightColor             "highlightColor"
#define CtrlNhighlightPixmap            "highlightPixmap"
#define CtrlNisTabGroup                 "isTabGroup"
#define CtrlNtraverseable               "traverseable"
#define CtrlNpreeditStartCallback       "preeditStartCallback"
#define CtrlNpreeditDoneCallback        "preeditDoneCallback"
#define CtrlNpreeditDrawCallback        "preeditDrawCallback"
#define CtrlNpreeditCaretCallback       "preeditCaretCallback"
#define CtrlNactivateCallback           "activateCallback"
#define CtrlNdestinationCallback        "destinationCallback"
#define CtrlNfocusCallback              "focusCallback"
#define CtrlNlosingFocusCallback        "losingFocusCallback"
#define CtrlNgainPrimaryCallback        "gainPrimaryCallback"
#define CtrlNlosePrimaryCallback        "losePrimaryCallback"
#define CtrlNgainClipboardCallback      "gainClipboardCallback"
#define CtrlNloseClipboardCallback      "loseClipboardCallback"
#define CtrlNvalueChangedCallback       "valueChangedCallback"
#define CtrlNmarginHeight               "marginHeight"
#define CtrlNmarginWidth                "marginWidth"
#define CtrlNcursorPosition             "cursorPosition"
#define CtrlNblinkRate                  "blinkRate"
#define CtrlNselectThreshold            "selectThreshold"
#define CtrlNcolumns                    "columns"
#define CtrlNselforeground              "selforeground"
#define CtrlNselbackground              "selbackground"

/* resource class definitions */
#define CtrlCAccelerators               XtCAccelerators
#define CtrlCBackground                 XtCBackground
#define CtrlCBitmap                     XtCBitmap
#define CtrlCBoolean                    XtCBoolean
#define CtrlCBorderColor                XtCBorderColor
#define CtrlCBorderWidth                XtCBorderWidth
#define CtrlCCallback                   XtCCallback
#define CtrlCColor                      XtCColor
#define CtrlCColormap                   XtCColormap
#define CtrlCCursor                     XtCCursor
#define CtrlCDepth                      XtCDepth
#define CtrlCEditType                   XtCEditType
#define CtrlCEventBindings              XtCEventBindings
#define CtrlCFile                       XtCFile
#define CtrlCFont                       XtCFont
#define CtrlCFontSet                    XtCFontSet
#define CtrlCForeground                 XtCForeground
#define CtrlCFraction                   XtCFraction
#define CtrlCFunction                   XtCFunction
#define CtrlCHSpace                     XtCHSpace
#define CtrlCHeight                     XtCHeight
#define CtrlCIndex                      XtCIndex
#define CtrlCInitialResourcesPersistent XtCInitialResourcesPersistent
#define CtrlCInsertPosition             XtCInsertPosition
#define CtrlCInterval                   XtCInterval
#define CtrlCJustify                    XtCJustify
#define CtrlCKnobIndent                 XtCKnobIndent
#define CtrlCKnobPixel                  XtCKnobPixel
#define CtrlCLabel                      XtCLabel
#define CtrlCLength                     XtCLength
#define CtrlCMappedWhenManaged          XtCMappedWhenManaged
#define CtrlCMargin                     XtCMargin
#define CtrlCMenuEntry                  XtCMenuEntry
#define CtrlCNotify                     XtCNotify
#define CtrlCOrientation                XtCOrientation
#define CtrlCParameter                  XtCParameter
#define CtrlCPixmap                     XtCPixmap
#define CtrlCPosition                   XtCPosition
#define CtrlCReadOnly                   XtCReadOnly
#define CtrlCResize                     XtCResize
#define CtrlCReverseVideo               XtCReverseVideo
#define CtrlCScreen                     XtCScreen
#define CtrlCScrollDCursor              XtCScrollDCursor
#define CtrlCScrollHCursor              XtCScrollHCursor
#define CtrlCScrollLCursor              XtCScrollLCursor
#define CtrlCScrollProc                 XtCScrollProc
#define CtrlCScrollRCursor              XtCScrollRCursor
#define CtrlCScrollUCursor              XtCScrollUCursor
#define CtrlCScrollVCursor              XtCScrollVCursor
#define CtrlCSelection                  XtCSelection
#define CtrlCSelectionArray             XtCSelectionArray
#define CtrlCSensitive                  XtCSensitive
#define CtrlCSpace                      XtCSpace
#define CtrlCString                     XtCString
#define CtrlCTextOptions                XtCTextOptions
#define CtrlCTextPosition               XtCTextPosition
#define CtrlCTextSink                   XtCTextSink
#define CtrlCTextSource                 XtCTextSource
#define CtrlCThickness                  XtCThickness
#define CtrlCThumb                      XtCThumb
#define CtrlCTranslations               XtCTranslations
#define CtrlCVSpace                     XtCVSpace
#define CtrlCValue                      XtCValue
#define CtrlCWidth                      XtCWidth
#define CtrlCWindow                     XtCWindow
#define CtrlCX                          XtCX
#define CtrlCY                          XtCY
#define CtrlCTooltip                    "Tooltip"
#define CtrlCShadowThickness            "ShadowThickness"
#define CtrlCShadowLightColor           "ShadowLightColor"
#define CtrlCShadowLightPixmap          "ShadowLightPixmap"
#define CtrlCShadowDarkColor            "ShadowDarkColor"
#define CtrlCShadowDarkPixmap           "ShadowDarkPixmap"
#define CtrlCHighlightThickness         "HighlightThickness"
#define CtrlCHighlightColor             "HighlightColor"
#define CtrlCHighlightPixmap            "HighlightPixmap"
#define CtrlCIsTabGroup                 "IsTabGroup"
#define CtrlCTraverseable               "Traverseable"
#define CtrlCMarginWidth                "MarginWidth"
#define CtrlCMarginHeight               "MarginHeight"
#define CtrlCCursorPosition             "CursorPosition"
#define CtrlCBlinkRate                  "BlinkRate"
#define CtrlCSelectThreshold            "SelectThreshold"
#define CtrlCColumns                    "Columns"
#define CtrlCSelforeground              "Selforeground"
#define CtrlCSelbackground              "Selbackground"

/* resource representation type definitions */
#define CtrlRAcceleratorTable           XtRAcceleratorTable
#define CtrlRAtom                       XtRAtom
#define CtrlRBitmap                     XtRBitmap
#define CtrlRBool                       XtRBool
#define CtrlRBoolean                    XtRBoolean
#define CtrlRCallProc                   XtRCallProc
#define CtrlRCallback                   XtRCallback
#define CtrlRCardinal                   XtRCardinal
#define CtrlRColor                      XtRColor
#define CtrlRColormap                   XtRColormap
#define CtrlRCommandArgArray            XtRCommandArgArray
#define CtrlRCursor                     XtRCursor
#define CtrlRDimension                  XtRDimension
#define CtrlRDirectoryString            XtRDirectoryString
#define CtrlRDisplay                    XtRDisplay
#define CtrlREditMode                   XtREditMode
#define CtrlREnum                       XtREnum
#define CtrlREnvironmentArray           XtREnvironmentArray
#define CtrlRFile                       XtRFile
#define CtrlRFloat                      XtRFloat
#define CtrlRFont                       XtRFont
#define CtrlRFontSet                    XtRFontSet
#define CtrlRFontStruct                 XtRFontStruct
#define CtrlRFunction                   XtRFunction
#define CtrlRGeometry                   XtRGeometry
#define CtrlRGravity                    XtRGravity
#define CtrlRImmediate                  XtRImmediate
#define CtrlRInitialState               XtRInitialState
#define CtrlRInt                        XtRInt
#define CtrlRJustify                    XtRJustify
#define CtrlRLongBoolean                XtRLongBoolean
#define CtrlRObject                     XtRObject
#define CtrlROrientation                XtROrientation
#define CtrlRPixel                      XtRPixel
#define CtrlRPixmap                     XtRPixmap
#define CtrlRPointer                    XtRPointer
#define CtrlRPosition                   XtRPosition
#define CtrlRRestartStyle               XtRRestartStyle
#define CtrlRScreen                     XtRScreen
#define CtrlRShort                      XtRShort
#define CtrlRSmcConn                    XtRSmcConn
#define CtrlRString                     XtRString
#define CtrlRStringArray                XtRStringArray
#define CtrlRStringTable                XtRStringTable
#define CtrlRTranslationTable           XtRTranslationTable
#define CtrlRUnsignedChar               XtRUnsignedChar
#define CtrlRVisual                     XtRVisual
#define CtrlRWidget                     XtRWidget
#define CtrlRWidgetClass                XtRWidgetClass
#define CtrlRWidgetList                 XtRWidgetList
#define CtrlRWindow                     XtRWindow
#define CtrlRTextPosition               XtCTextPosition
#define CtrlRTime                       "Time"
#define CtrlRXftFont                    "XftFont"
#define CtrlRXftColor                   "XftColor"

/* resource enum definitions */
#define CtrlEfalse                      XtEfalse
#define CtrlEhorizontal                 XtEhorizontal
#define CtrlEno                         XtEno
#define CtrlEoff                        XtEoff
#define CtrlEon                         XtEon
#define CtrlEtextAppend                 XtEtextAppend
#define CtrlEtextEdit                   XtEtextEdit
#define CtrlEtextRead                   XtEtextRead
#define CtrlEtrue                       XtEtrue
#define CtrlEvertical                   XtEvertical
#define CtrlExtdefaultbackground        XtExtdefaultbackground
#define CtrlExtdefaultfont              XtExtdefaultfont
#define CtrlExtdefaultforeground        XtExtdefaultforeground
#define CtrlEyes                        XtEyes

/* Callback Reasons */
enum {
	CTRL_NONE,
	CTRL_VALUE_CHANGED,
};

/* Callback Structures */
typedef struct {
	int             reason;
	XEvent         *event;
} CtrlGenericCallData;

/* widget class types */
typedef struct _CtrlPrimitiveClassRec  *CtrlPrimitiveWidgetClass;
typedef struct _CtrlTextFieldClassRec  *CtrlTextFieldWidgetClass;

/* widget types */
typedef struct _CtrlPrimitiveRec       *CtrlPrimitiveWidget;
typedef struct _CtrlTextFieldRec       *CtrlTextFieldWidget;

/* widget classes */
extern WidgetClass ctrlPrimitiveWidgetClass;
extern WidgetClass ctrlTextFieldWidgetClass;

#endif /* _CONTROL_H */
