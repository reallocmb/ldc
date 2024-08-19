#ifdef __unix__

#include"ldc_internal.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include"itypes.h"

static XImage *ximage = NULL;
Display *display;
Window window;

#if 0
void show_utf8_prop(Display *dpy, Window w, Atom p, u8 *buffer, u32 *buffer_count)
{
    Atom da, incr, type;
    int di;
    unsigned long size, dul;
    unsigned char *prop_ret = NULL;

    /* Dummy call to get type and size. */
    XGetWindowProperty(dpy, w, p, 0, 0, False, AnyPropertyType,
                       &type, &di, &dul, &size, &prop_ret);
    XFree(prop_ret);

    incr = XInternAtom(dpy, "INCR", False);
    if (type == incr)
    {
        printf("Data too large and INCR mechanism not implemented\n");
        return;
    }

    /* Read the data in one go. */
    printf("Property size: %lu\n", size);

    XGetWindowProperty(dpy, w, p, 0, size, False, AnyPropertyType,
                       &da, &di, &dul, &dul, &prop_ret);
    memcpy(buffer, prop_ret, size);
    buffer[size] = 0;
    *buffer_count = size;

    fflush(stdout);
    XFree(prop_ret);

    /* Signal the selection owner that we have successfully read the
     * data. */
    XDeleteProperty(dpy, w, p);
}

void clipboard_paste(u8 *buffer, u32 *buffer_count)
{
    Display *dpy;
    Window owner, target_window, root;
    int screen;
    Atom sel, target_property, utf8;
    XEvent ev;
    XSelectionEvent *sev;

    dpy = XOpenDisplay(NULL);
    if (!dpy)
    {
        fprintf(stderr, "Could not open X display\n");
        return;
    }

    screen = DefaultScreen(dpy);
    root = RootWindow(dpy, screen);

    sel = XInternAtom(dpy, "CLIPBOARD", False);
    utf8 = XInternAtom(dpy, "UTF8_STRING", False);

    owner = XGetSelectionOwner(dpy, sel);
    if (owner == None)
    {
        printf("'CLIPBOARD' has no owner\n");
        return;
    }
    printf("0x%lX\n", owner);

    /* The selection owner will store the data in a property on this
     * window: */
    target_window = XCreateSimpleWindow(dpy, root, -10, -10, 1, 1, 0, 0, 0);

    /* That's the property used by the owner. Note that it's completely
     * arbitrary. */
    target_property = XInternAtom(dpy, "PENGUIN", False);

    /* Request conversion to UTF-8. Not all owners will be able to
     * fulfill that request. */
    XConvertSelection(dpy, sel, utf8, target_property, target_window,
                      CurrentTime);

    for (;;)
    {
        XNextEvent(dpy, &ev);
        switch (ev.type)
        {
            case SelectionNotify:
            sev = (XSelectionEvent*)&ev.xselection;
            if (sev->property == None)
            {
                printf("Conversion could not be performed.\n");
                return;
            }
            else
            {
                show_utf8_prop(dpy, target_window, target_property, buffer, buffer_count);
                return;
            }
            break;
        }
    }
}
#endif

#if 0 /* todo */
void input_default_func(char *buffer, u8 bytes, u32 key_spec)
{
}
#endif

#define xlib_gui_init gui_init
void xlib_gui_init(u16 width, u16 height)
{
    GlobalGui.width = width;
    GlobalGui.height = height;
    GlobalGui.running = true;
    GlobalGui.render_func = render_default_func;
    // GlobalGui.input_func = input_default_func; /* to do */

    display = XOpenDisplay(NULL);

    XSetWindowAttributes attributes = { 0 };
    window = XCreateWindow(display,
                           DefaultRootWindow(display),
                           200, 200,
                           GlobalGui.width, GlobalGui.height,
                           0,
                           0, 
                           InputOutput,
                           XDefaultVisual(display, 0),
                           0,
                           &attributes);
    XStoreName(display,
               window,
               "LDC");

    XSelectInput(display,
                 window,
                 KeyPressMask |
                 KeyReleaseMask | 
                 ExposureMask |
                 StructureNotifyMask |
                 PropertyChangeMask);

    XMapWindow(display, window);


    // create ximage
    ximage = XCreateImage(display,
                          XDefaultVisual(display, 0),
                          24,
                          ZPixmap,
                          0,
                          NULL,
                          0,
                          0,
                          32,
                          0);
}

#define xlib_gui_update gui_update
void xlib_gui_update(void)
{
    render();

    XPutImage(display,
              window,
              XDefaultGC(display, 0),
              ximage,
              0, 0,
              0, 0,
              GlobalGui.width, GlobalGui.height);
}

#define xlib_gui_loop gui_loop
void xlib_gui_loop(void)
{
    char buffer[255];
    KeySym key;
    XEvent event;
    while (GlobalGui.running)
    {
        XNextEvent(display, &event);
        if (XFilterEvent(&event, window))
            continue;
        switch (event.type)
        {
            case KeyPress:
            {
                int bytes = XLookupString(&event.xkey, buffer, sizeof(buffer), &key, NULL);
                printf("buffer %s\n", buffer);
                printf("bytes %d\n", bytes);
                if (bytes == 0)
                {
                    switch (key)
                    {
                        /*
                        case XK_Control_L: buffer[0] = (char)KEY_CONTROL_L; break;
                        case XK_Control_R: buffer[0] = (char)KEY_CONTROL_R; break;
                        default: buffer[0] = (char)KEY_NOT_SPECIFIED; break;
                        */
                    }
                }

                printf("keypress\n");
                GlobalGui.input_func((u8)buffer[0]);
            } break;
            case KeyRelease:
            {
            } break;
            case DestroyNotify:
            {
                exit(0);
            } break;
            case Expose:
            {

                xlib_gui_update();
            } break;
            case ConfigureNotify:
            {
                GlobalGui.width = event.xconfigure.width;
                GlobalGui.height = event.xconfigure.height;
                GlobalGui.bits = realloc(GlobalGui.bits, GlobalGui.width * GlobalGui.height * sizeof(*GlobalGui.bits) + 10000);
                ximage->width = GlobalGui.width;
                ximage->height = GlobalGui.height;
                ximage->bytes_per_line = GlobalGui.width * sizeof(*GlobalGui.bits);
                ximage->data = (char *)GlobalGui.bits;
            } break;

            default:
            {
            } break;
        }
    }
}
#endif
