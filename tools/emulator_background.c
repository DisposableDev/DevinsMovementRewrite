/* Keep private X11 emulator captures below other windows and out of focus. */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <xcb/xcb.h>

static void *next_symbol(const char *name) {
    void *result = dlsym(RTLD_NEXT, name);
    if (result == NULL) {
        abort();
    }
    return result;
}

static void prepare_x_window(Display *display, Window window) {
    XWMHints hints = { .flags = InputHint, .input = False };
    Atom below = XInternAtom(display, "_NET_WM_STATE_BELOW", False);
    XSetWMHints(display, window, &hints);
    XChangeProperty(display, window, XInternAtom(display, "_NET_WM_STATE", False), XA_ATOM, 32,
                    PropModeReplace, (unsigned char *) &below, 1);
}

int XMapWindow(Display *display, Window window) {
    int (*map)(Display *, Window) = next_symbol("XMapWindow");
    prepare_x_window(display, window);
    return map(display, window);
}

int XMapRaised(Display *display, Window window) {
    return XMapWindow(display, window);
}

int XRaiseWindow(Display *display, Window window) {
    (void) display;
    (void) window;
    return 0;
}

int XSetInputFocus(Display *display, Window window, int revert, Time time) {
    (void) display;
    (void) window;
    (void) revert;
    (void) time;
    return 0;
}

int XSendEvent(Display *display, Window window, Bool propagate, long mask, XEvent *event) {
    int (*send)(Display *, Window, Bool, long, XEvent *) = next_symbol("XSendEvent");
    if (event->type == ClientMessage
        && event->xclient.message_type == XInternAtom(display, "_NET_ACTIVE_WINDOW", False)) {
        return 0;
    }
    return send(display, window, propagate, mask, event);
}

static xcb_atom_t xcb_atom(xcb_connection_t *connection, const char *name) {
    xcb_intern_atom_reply_t *reply =
        xcb_intern_atom_reply(connection, xcb_intern_atom(connection, 0, strlen(name), name), NULL);
    if (reply == NULL) {
        abort();
    }
    xcb_atom_t result = reply->atom;
    free(reply);
    return result;
}

xcb_void_cookie_t xcb_map_window(xcb_connection_t *connection, xcb_window_t window) {
    xcb_void_cookie_t (*map)(xcb_connection_t *, xcb_window_t) = next_symbol("xcb_map_window");
    uint32_t hints[9] = { 1, 0 };
    uint32_t below = xcb_atom(connection, "_NET_WM_STATE_BELOW");
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, xcb_atom(connection, "WM_HINTS"),
                        xcb_atom(connection, "WM_HINTS"), 32, 9, hints);
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window,
                        xcb_atom(connection, "_NET_WM_STATE"), XCB_ATOM_ATOM, 32, 1, &below);
    return map(connection, window);
}

xcb_void_cookie_t xcb_set_input_focus(xcb_connection_t *connection, uint8_t revert, xcb_window_t window,
                                      xcb_timestamp_t time) {
    (void) connection;
    (void) revert;
    (void) window;
    (void) time;
    return (xcb_void_cookie_t){ 0 };
}

xcb_void_cookie_t xcb_send_event(xcb_connection_t *connection, uint8_t propagate,
                                 xcb_window_t destination, uint32_t mask, const char *event) {
    xcb_void_cookie_t (*send)(xcb_connection_t *, uint8_t, xcb_window_t, uint32_t, const char *) =
        next_symbol("xcb_send_event");
    const xcb_client_message_event_t *message = (const void *) event;
    if ((message->response_type & 127) == XCB_CLIENT_MESSAGE
        && message->type == xcb_atom(connection, "_NET_ACTIVE_WINDOW")) {
        return (xcb_void_cookie_t){ 0 };
    }
    return send(connection, propagate, destination, mask, event);
}

void SDL_RaiseWindow(void *window) {
    (void) window;
}

int SDL_SetWindowInputFocus(void *window) {
    (void) window;
    return 0;
}
