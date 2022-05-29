CONTROL(3) - Library Functions Manual

# NAME

**control** - X11 Widget Toolkit

# SYNOPSIS

**#include &lt;control.h>**

*WidgetClass ctrlPrimitiveWidgetClass*  
*WidgetClass ctrlTextFieldWidgetClass*

# DESCRIPTION

**control**
is a widget set based on the X Toolkit Intrinsics (Xt) Library.
The Xt library provides an object-oriented framework for creating reusable user-interface components called widgets.
**control**
provides widget classes for common user-interface elements such as buttons and text input fields.

Each widget is configurable using X resources.
Some resources are hardcoded by the application programmer,
others may be set by the user.
For example, the background color of most widget can be set by a resource named
'`background`'
which belongs to the
'`Background`'
class of resources.
Note that resource names are in
*camelCase*;
while resource classes are in
*PascalCase*.
The application programmer should avoid referring to resources by their string literals.
The
**control**
header file defines constants for all known resource names and classes
(so the compiler can warn if a unknown resource name or class is used).
For example, the
'`CtrlNbackground`'
constant evaluates to the background name string at compiling time; while the
'`CtrlCBackground`'
constant evaluates to the background class string.
Note that resource name constants begin with
'`CtrlN`'
followed by the camelCase name string;
and resource class constants begin with
'`CtrlC`'
followed by the PascalCase name string.

A special class of resources, the
'`CtrlCCallback`'
class,
is particuarly important for the application programmer.
A callback resource of a widget registers a list of functions, called
*callback functions*,
to be invoked when the user performs the action associated with the callback resource on that widget.
A callback function can be registerd along with a pointer to an object to be passed to the function when it be called.
For example, a button widget provides the
'`CtrlNactivateCallback`'
for when the user pushes the button.

Not every interaction event with a widget results in a callback to an application function.
Widgets may handle events themselves, by calling internal functions called
*actions*.
For example, a text input field widget has the action
'`delete-previous-character`'
to backspace an inputted character;
the application does not have to provide such function, for it is already implemented by
**control**.
Actions are mapped to events in a
*translation table*.
This table can be changed by the application code or by the user.
For example, the
'`delete-previous-character`'
action is mapped to the
'`<Ctrl-H>`'
and
'`<Backspace>`'
key presses.

In the Xt framework, widgets are organized in classes,
in a inheritable, object-oriented fashion.
**control**
defines a set of widget classes that the application programmer can instantiate.
Widget classes are organized hierarchically in such a way that
subclasses inherits resources and behavior from their superclasses.

A widget can be instantiated using the
XtCreateWidget(3),
XtVaCreateWidget(3),
XtCreateManagedWidget(3),
and
XtVaCreateManagedWidget(3)
functions from the Xt library.
The headers for the Xt library need not be included by the application programmer
(they are already included in the
**control**
header).
Refer to the Xt documentation for more information on the Xt framework used by
**control**.

Only global variables for class objects were listed in the
*SYNOPSIS*
section.
In particular,
the names of resources,
the constant values for the reason of a callback invocation,
and the data types for the call data of callback functions
are introduced through the next sections.
**control**
does not provide any library function.

# CLASSES

This section describes the widget classes provided by
**control**.
This section also describes the classes provided by the Xt framework
(see the section
*Xt Classes*
below).

For each widget class there is an object exported by the
**control**
header file
(used by the application programmer to instantiate widgets),
and a resource class recognized by the application
(used by the user to customize the widget).
In
**control**,
objects are named
'`Ctrl*WidgetClass`'
and resource class are
'`Ctrl*`'
(where the asterisk is replaced by the name of the class).
For example,
'`ctrlPrimitiveWidgetClass`'
and
'`ctrlTextFieldWidgetClass`'
are the objects for the Primitive and TextField widget classes
and
'`CtrlPrimitive`'
and
'`CtrlTextField`'
are the resource classes for these widget classes

## Primitive

Object pointer: CtrlPrimitiveWidgetClass  
Resource class: CtrlPrimitive

The Primitive widget class is the generic superclass for windowed widget classes provided by
**control**.
It is subclassed from Xt's Core widget class.
It handles the basic drawing of the 3d shadow and highlighting;
and it provides the routine that redraws the widget.
Although a primitive widget itself does not draw any text,
primitive holds resources such as text font and text color.
Since it is just a superclass for other widget classes,
a Primitive widget is almost never instantiated.

The Primitive widget class provides the following resources
in addition to those inherited from its superclass:

*	CtrlNcursor
*	CtrlNhighlightColor
*	CtrlNhighlightPixmap
*	CtrlNhighlightThickness
*	CtrlNfont
*	CtrlNfontAscent
*	CtrlNfontAverageWidth
*	CtrlNfontDescent
*	CtrlNfontHeight
*	CtrlNforeground
*	CtrlNisTabGroup
*	CtrlNmarginHeight
*	CtrlNmarginWidth
*	CtrlNshadowDarkColor
*	CtrlNshadowDarkPixmap
*	CtrlNshadowLightColor
*	CtrlNshadowLightPixmap
*	CtrlNshadowThickness
*	CtrlNtraverseable
*	CtrlNtooltip

## TextField

Object pointer: CtrlTextFieldWidgetClass  
Resource class: CtrlTextClass

The TextField widget class provides a single line text input field for the user to type text in.
It is subclassed from the Primitive widget class.

The TextField widget class provides the following resources
in addition to those inherited from its superclass:

*	CtrlNactivateCallback
*	CtrlNblinkRate
*	CtrlNcolumns
*	CtrlNdestinationCallback
*	CtrlNfocusCallback
*	CtrlNgainClipboardCallback
*	CtrlNgainPrimaryCallback
*	CtrlNloseClipboardCallback
*	CtrlNlosePrimaryCallback
*	CtrlNlosingFocusCallback
*	CtrlNmodifyVerifyCallback
*	CtrlNmotionVerifyCallback
*	CtrlNselbackground
*	CtrlNselectionThreshold
*	CtrlNselforeground
*	CtrlNvalueChangedCallback
*	CtrlNvalue

# RESOURCES

## CtrlNactivateCallback

## CtrlNblinkRate

## CtrlNcolumns

## CtrlNcursor

## CtrlNdestinationCallback

## CtrlNfocusCallback

## CtrlNfont

## CtrlNfontAscent

## CtrlNfontAverageWidth

## CtrlNfontDescent

## CtrlNfontHeight

## CtrlNforeground

## CtrlNgainClipboardCallback

## CtrlNgainPrimaryCallback

## CtrlNhighlightColor

## CtrlNhighlightPixmap

## CtrlNhighlightThickness

## CtrlNisTabGroup

## CtrlNloseClipboardCallback

## CtrlNlosePrimaryCallback

## CtrlNlosingFocusCallback

## CtrlNmarginHeight

## CtrlNmarginWidth

## CtrlNmodifyVerifyCallback

## CtrlNmotionVerifyCallback

## CtrlNselbackground

## CtrlNselectionThreshold

## CtrlNselforeground

## CtrlNshadowDarkColor

## CtrlNshadowDarkPixmap

## CtrlNshadowLightColor

## CtrlNshadowLightPixmap

## CtrlNshadowThickness

## CtrlNtooltip

## CtrlNtraverseable

## CtrlNvalue

## CtrlNvalueChangedCallback

# CALLBACKS

The callback functions registered in a widget's callback resource is invoked by
**control**
when the event related to that resource occurs.
A callback function must be typed as
'`XtCallbackProc`'.

	typedef void
	(*XtCallbackProc)(Widget widget, XtPointer closure, XtPointer call_data);

A callback function is called with the widget that called it as the first argument;
a pointer to the object supplied while registering the callback function as second argument;
and a pointer to an object called
*call data*
as third argument.
The actual type of the call data depends on the callback resource.
Possible data types are described below.
Call data objects are always a structure, with the first field being an
'`int`'
describing the reason for the callback function to be called,
and the second field being a pointer to the
'`XEvent`'
object that caused the callback invocation.

## Generic CallData

The Generic CallData datatype is the generic type for most callback functions.
It simply contains a
'`reason`'
integer,
and a pointer to a event structure.
Since all other CallData datatypes have these same fields,
they will only be described here;
the subsections that follow will not describe them.

	typedef struct {
		int             reason;
		XEvent         *event;
	} CtrlGenericCallData;

*int reason*

	Indicates why the callback was invoked.
	See the description of each callback resource for information on the possible values.

*XEvent *event*

	Points to the XEvent that triggered the callback.
	It can be
	'`NULL`'
	if there's no X Event to be passed
	(for example, if the action that triggered the callback was generated programmatically).

# TRANSLATIONS

TODO.

# ACTIONS

TODO.

# EXAMPLES

	/* TODO */

# SEE ALSO

X(1)

*Xlib - C Language X Interface*.

*X Toolkit Intrinsics - C Language Interface*.

# AUTHORS

The
**control**
X11 Widget Toolkit was written by
Lucas de Sena <[lucas@seninha.org](mailto:lucas@seninha.org)>.

OpenBSD 7.1 - May 28, 2022
