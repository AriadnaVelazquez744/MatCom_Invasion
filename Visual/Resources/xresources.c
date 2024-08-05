#include "xresources.h"
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

struct XResources resources;
unsigned long orange_pixel;


void initXResources() {
    XInitThreads();
    resources.display = XOpenDisplay(NULL);
    if (!resources.display) {
        fprintf(stderr, "Error al abrir el display.\n");
        exit(EXIT_FAILURE);
    }
    resources.screen = DefaultScreen(resources.display);
    resources.WIDTH = DisplayWidth(resources.display, resources.screen);
    resources.HEIGHT = DisplayHeight(resources.display, resources.screen);
    resources.current_level=LEVEL_1;

    resources.window = XCreateSimpleWindow(resources.display, RootWindow(resources.display, resources.screen), 0, 0, resources.WIDTH, resources.HEIGHT, 1,
                                           BlackPixel(resources.display, resources.screen), WhitePixel(resources.display, resources.screen));
    if (!resources.window) {
        fprintf(stderr, "Error al crear la ventana.\n");
        XCloseDisplay(resources.display);
        exit(EXIT_FAILURE);
    }

    XSelectInput(resources.display, resources.window, ExposureMask | PointerMotionMask | ButtonPressMask);
    XMapWindow(resources.display, resources.window);

    resources.gc = XCreateGC(resources.display, resources.window, 0, 0);
    if (!resources.gc) {
        fprintf(stderr, "Error al crear el contexto gráfico.\n");
        XDestroyWindow(resources.display, resources.window);
        XCloseDisplay(resources.display);
        exit(EXIT_FAILURE);
    }

    XColor orange_color, exact_color;
    if (XAllocNamedColor(resources.display, DefaultColormap(resources.display, resources.screen), "orange", &orange_color, &exact_color)) {
        orange_pixel = orange_color.pixel;
    } else {
        fprintf(stderr, "Error al asignar el color naranja.\n");
        XFreeGC(resources.display, resources.gc);
        XDestroyWindow(resources.display, resources.window);
        XCloseDisplay(resources.display);
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(&resources.mutex, NULL);
    pthread_cond_init(&resources.cond, NULL);
    resources.should_draw = 0;

    for (int i = 0; i < MAX_PROJECTILES; ++i) {
        resources.projectiles[i].active = 0;
    }

    initEnemies();
}

void cleanupXResources() {
    pthread_mutex_destroy(&resources.mutex);
    pthread_cond_destroy(&resources.cond);
    XFreeGC(resources.display, resources.gc);
    XDestroyWindow(resources.display, resources.window);
    XCloseDisplay(resources.display);
}
