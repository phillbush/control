CONTROL(3) - Library Functions Manual

# NAME

**control** - X11 Widget Toolkit

# SYNOPSIS

**#include &lt;control.h>**

*typedef void*  
**(\*XtCallbackProc)**(*Widget widget*,
*XtPointer closure*,
*XtPointer call\_data*);

*WidgetClass ctrlPrimitiveWidgetClass*  
*WidgetClass ctrlTextFieldWidgetClass*

# DESCRIPTION

**control**
is a widget set based on the X Toolkit Intrinsics (Xt) Library.
The Xt library provides an object-oriented framework for creating reusable user-interface components called widgets.
**control**
provides widgets for common user-interface elements such as buttons and text input fields.

Each widget is configurable using X resources.
Some resources are hardcoded by the application programmer,
others may be set by the user.
For example, the background color of most widget can be set by a resource named
'`background`'
which belongs to the
'`Background`'
class of resources.
Note that resource names are in
*camelCase*
(beginning in lower case);
while resource classes are in
*PascalCase*
(beginning in upper case).
The application programmer should avoid referring to resources by their string literals.
The
**control**
header file defines constants for all the known resource names and classes.
Using the constants is preferrable because the compiler will fail when using a non-existant resource name or class
(by using string literals, the application programmer is not warned, and the compilation will not fail).
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
A callback resource registers a list of functions, called
*callback functions*,
to be invoked when the user performs the action associated with the callback resource on the widget of that resource.
A callback function can be registerd along with a pointer to an object to be passed to the function when it be called.
For example, a button widget provides the
'`CtrlNactivateCallback`'
for when the user pushes the button.
A callback function must be typed as
'`XtCallbackProc`'
(defined in the
*SYNOPSIS*).
A callback function is called with the widget that called it as the first argument,
a pointer to the object supplied while registering the callback function as second argument,
and a pointer to an object called
*call data*
as third argument.
The actual type of the call data depends on the callback resource and are described below in the
*CALLBACKS*
section.
Call data objects are always a structure,
with the first field being an
'`int`'
describing the reason for the callback function to be called,
and the second field being a pointer to the
'`XEvent`'
object that caused the callback invocation.

Not every interaction event with a widget results in a callback to an application function.
Widgets may handle events themselves, by calling internal functions called
*actions*.
For example, a text input field widget has the action
'`delete-previous-character`'
to backspace an inputted character;
the application does not have to provide such function.
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

TODO.

# RESOURCES

TODO.

# CALLBACKS

TODO.

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
