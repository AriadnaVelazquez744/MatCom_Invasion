#ifndef XRESOURCES_H
#define XRESOURCES_H

#include <X11/Xlib.h>
#include <pthread.h>
#include "../../Game_Logic/Ship/ship.h"
#include "../../Game_Logic/Projectiles/projectile.h"
#include "../../Game_Logic/Aliens/enemy.h"

//Estados del juego
typedef enum {
     GAME_SELECT_MODE,
    GAME_RUNNING,
    GAME_OVER,
    GAME_WIN_LEVEL,
    GAME_WIN
} GameState;

//Modos de Juego
typedef enum {
    MODE_PROGRESSIVE,  // Primero tipo 1, luego tipo 2
    MODE_ALTERNATE,    // Alterna entre tipo 1 y tipo 2
    MODE_RANDOM        // Genera enemigos de tipo aleatorio
} GameMode;

//Niveles
typedef enum{
    LEVEL_1,
    LEVEL_2,
    LEVEL_3
}LEVEL;

//Recursos del juego
struct XResources {
    Display *display;
    Window window;
    GC gc;
    int screen;
    ShipPosition shipPos;
    Projectile projectiles[MAX_PROJECTILES];
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int should_draw;
    unsigned int WIDTH; 
    unsigned int HEIGHT;
    GameState game_state; 
    LEVEL current_level;
    int max_enemies;
    int remaining_enemies;
    int destroyed_enemies;
    GameMode game_mode;
    Pixmap buffer;
};

extern struct XResources resources;
extern unsigned long orange_pixel;

void initXResources();
void cleanupXResources();

#endif 
