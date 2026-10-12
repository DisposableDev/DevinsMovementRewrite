/* GTK interposition for background ares launches; clicking still gives focus. */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>

typedef struct _GtkWidget GtkWidget;
typedef struct _GtkWindow GtkWindow;

extern void gtk_window_set_focus_on_map(GtkWindow *window, int setting);
extern void gtk_window_set_keep_below(GtkWindow *window, int setting);
extern int gtk_window_is_active(GtkWindow *window);
extern void gtk_widget_show(GtkWidget *widget);
extern unsigned long g_signal_connect_data(void *instance, const char *signal, void (*callback)(void),
                                           void *data, void (*destroy_data)(void *, void *), int flags);

/* Keep startup behind other windows until the user activates this window. */
static void release_keep_below(GtkWindow *window, void *property, void *data) {
    (void) property;
    (void) data;
    if (gtk_window_is_active(window)) {
        gtk_window_set_keep_below(window, 0);
    }
}

GtkWidget *gtk_window_new(int type) {
    static GtkWidget *(*create_window)(int);
    if (create_window == NULL) {
        create_window = dlsym(RTLD_NEXT, "gtk_window_new");
        if (create_window == NULL) {
            abort();
        }
    }
    GtkWidget *widget = create_window(type);
    gtk_window_set_focus_on_map((GtkWindow *) widget, 0);
    gtk_window_set_keep_below((GtkWindow *) widget, 1);
    g_signal_connect_data(widget, "notify::is-active", (void (*)(void)) release_keep_below, NULL, NULL,
                          0);
    return widget;
}

/* ares calls present explicitly when loading a ROM. Show without activating. */
void gtk_window_present(GtkWindow *window) {
    gtk_widget_show((GtkWidget *) window);
}

void gtk_window_present_with_time(GtkWindow *window, unsigned int timestamp) {
    (void) timestamp;
    gtk_window_present(window);
}
