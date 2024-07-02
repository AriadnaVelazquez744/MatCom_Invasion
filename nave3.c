#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#define WIDTH 800 
#define HEIGHT 600

typedef struct {
    int x;
    int y;
} ShipPosition;

struct XResources {
    Display *display;
    Window window;
    GC gc;
    int screen;
    ShipPosition shipPos;
    pthread_mutex_t mutex; // Mutex para sincronización de datos
};

struct XResources resources;

// Declaración de drawShip antes de su uso
void drawShip();

void *mouseControl(void *arg) {
    XEvent event;
    while (1) {
        XNextEvent(resources.display, &event);
        if (event.type == MotionNotify) {
            pthread_mutex_lock(&resources.mutex); // Bloquear el mutex antes de modificar datos compartidos
            resources.shipPos.x = event.xmotion.x;
            resources.shipPos.y = event.xmotion.y;
            pthread_mutex_unlock(&resources.mutex); // Desbloquear el mutex después de la modificación
            // Redibujar la nave cuando se mueva el mouse
            drawShip();
        }
    }
    return NULL;
}

void drawShip() {
    XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));
    XClearWindow(resources.display, resources.window);
    pthread_mutex_lock(&resources.mutex); // Bloquear el mutex antes de leer datos compartidos
    XFillRectangle(resources.display, resources.window, resources.gc, resources.shipPos.x - 10, resources.shipPos.y - 10, 20, 20);
    pthread_mutex_unlock(&resources.mutex); // Desbloquear el mutex después de la lectura
    XFlush(resources.display);
}

int main() {
    XInitThreads();

    resources.display = XOpenDisplay(NULL);
    if (!resources.display) {
        fprintf(stderr, "Error al abrir el display.\n");
        return EXIT_FAILURE;
    }
    resources.screen = DefaultScreen(resources.display);

    resources.window = XCreateSimpleWindow(resources.display, RootWindow(resources.display, resources.screen), 0, 0, WIDTH, HEIGHT, 1,
                                           BlackPixel(resources.display, resources.screen), WhitePixel(resources.display, resources.screen));
    if (!resources.window) {
        fprintf(stderr, "Error al crear la ventana.\n");
        XCloseDisplay(resources.display);
        return EXIT_FAILURE;
    }

    XSelectInput(resources.display, resources.window, ExposureMask | KeyPressMask | PointerMotionMask);
    XMapWindow(resources.display, resources.window);

    resources.gc = XCreateGC(resources.display, resources.window, 0, 0);
    if (!resources.gc) {
        fprintf(stderr, "Error al crear el contexto gráfico.\n");
        XDestroyWindow(resources.display, resources.window);
        XCloseDisplay(resources.display);
        return EXIT_FAILURE;
    }

    pthread_mutex_init(&resources.mutex, NULL); // Inicializar el mutex

    pthread_t mouseThread;
    if (pthread_create(&mouseThread, NULL, mouseControl, NULL)!= 0) {
        fprintf(stderr, "Error al crear el hilo para control del ratón.\n");
        XFreeGC(resources.display, resources.gc);
        XDestroyWindow(resources.display, resources.window);
        XCloseDisplay(resources.display);
        return EXIT_FAILURE;
    }

    // Dibujar la nave inicialmente
    pthread_mutex_lock(&resources.mutex); // Bloquear el mutex antes de modificar datos compartidos
    resources.shipPos.x = WIDTH / 2;
    resources.shipPos.y = HEIGHT / 2;
    pthread_mutex_unlock(&resources.mutex); // Desbloquear el mutex después de la modificación
    drawShip();

    while (1) {
        XEvent event;
        XNextEvent(resources.display, &event);
        if (event.type == Expose) {
            drawShip();
        } else if (event.type == KeyPress) {
            break; // Salir del bucle cuando se presiona una tecla
        }
    }

    pthread_mutex_destroy(&resources.mutex); // Destruir el mutex al finalizar

    XFreeGC(resources.display, resources.gc);
    XDestroyWindow(resources.display, resources.window);
    XCloseDisplay(resources.display);
    return EXIT_SUCCESS;
}
