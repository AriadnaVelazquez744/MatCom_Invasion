#include "draw.h"
#include "../Resources/xresources.h"
#include "../../Game_Logic/Projectiles/projectile.h"
#include "../../Game_Logic/Aliens/enemy.h"
#include <time.h>
#include <stdio.h>
#include <string.h> 

extern struct XResources resources;
extern unsigned long orange_pixel;
//Dibujo de la nave
void drawShip() {
    XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));
    XClearWindow(resources.display, resources.window);

    ShipPosition shipPos = getShipPosition();
    XFillRectangle(resources.display, resources.window, resources.gc, shipPos.x - 30, shipPos.y - 7, 60, 14);
    XFillRectangle(resources.display, resources.window, resources.gc, shipPos.x - 22, shipPos.y + 7, 15, 7);
    XFillRectangle(resources.display, resources.window, resources.gc, shipPos.x + 10, shipPos.y + 7, 15, 7);
    XSetForeground(resources.display, resources.gc, WhitePixel(resources.display, resources.screen));
    XFillRectangle(resources.display, resources.window, resources.gc, shipPos.x - 7, shipPos.y - 7, 14, 7);
    XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));
    XFillRectangle(resources.display, resources.window, resources.gc, shipPos.x - 3, shipPos.y - 22, 6, 14);
}
//Dibujo de los disparos efectuados
void drawProjectiles() {
    XSetForeground(resources.display, resources.gc, orange_pixel);
    Projectile *projectiles = getProjectiles();
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
        if (projectiles[i].active) {
            XFillRectangle(resources.display, resources.window, resources.gc,
                           projectiles[i].x - 3, projectiles[i].y - 10, 6, 10);
        }
    }
}
//Dibujo de los aliens(mejorar estética)
void drawEnemies() {
    XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));
    Enemy *enemies = getEnemies();
    for (int i = 0; i < MAX_ENEMIES; ++i) {
        if (enemies[i].active) {
            XFillRectangle(resources.display, resources.window, resources.gc, enemies[i].x - 10, enemies[i].y - 10, 20, 20);
            if (enemies[i].type==ENEMY_TYPE_1)
            {
               XSetForeground(resources.display, resources.gc, WhitePixel(resources.display, resources.screen));
            XFillRectangle(resources.display, resources.window, resources.gc, enemies[i].x - 5, enemies[i].y - 5, 4, 4);
            XFillRectangle(resources.display, resources.window, resources.gc, enemies[i].x + 1, enemies[i].y - 5, 4, 4);
            XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));
            }
            else if (enemies[i].type==ENEMY_TYPE_2)
            {
                 XSetForeground(resources.display, resources.gc, WhitePixel(resources.display, resources.screen));
            XFillRectangle(resources.display, resources.window, resources.gc, enemies[i].x - 5, enemies[i].y - 5, 4, 4);
            XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));
            }
            
            
        }
    }
}
//Pantalla Final(Game Over)
void drawGameOver() {
    XClearWindow(resources.display, resources.window);
    const char* message = "Game Over";
    const char* restart_message = "Restart";

    XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));

    // Cargar fuente 
    XFontStruct* font = XLoadQueryFont(resources.display, "12x24");
    if (!font) {
        // Si no se puede cargar la fuente, usar una fuente predeterminada
        fprintf(stderr, "No se pudo cargar la fuente grande, usando la fuente predeterminada.\n");
        font = XLoadQueryFont(resources.display, "fixed");
        if (!font) {
            fprintf(stderr, "No se pudo cargar la fuente predeterminada.\n");
            return; // Salir si no se puede cargar ninguna fuente
        }
    }

    XSetFont(resources.display, resources.gc, font->fid);

    // Dibujar el mensaje de Game Over en el centro de la pantalla
    int text_width = XTextWidth(font, message, strlen(message));
    XDrawString(resources.display, resources.window, resources.gc, (resources.WIDTH - text_width) / 2, resources.HEIGHT / 2 - 20, message, strlen(message));

    // Dibujar el botón de Restart
    int restart_text_width = XTextWidth(font, restart_message, strlen(restart_message));
    int button_x = (resources.WIDTH - restart_text_width) / 2 - 10;
    int button_y = resources.HEIGHT / 2 + 30;
    int button_width = restart_text_width + 20;
    int button_height = 30;

    XDrawRectangle(resources.display, resources.window, resources.gc, button_x, button_y, button_width, button_height);
    XDrawString(resources.display, resources.window, resources.gc, button_x + 10, button_y + 20, restart_message, strlen(restart_message));

    // Liberar la fuente cargada
    if (font) {
        XFreeFont(resources.display, font);
        XClearWindow(resources.display, resources.window);
    const char* message = "Game Over";
    }

    XFlush(resources.display);
}

void drawWinnLevelOne(){//Por ahora no funciona,sistema desactivado por ahora
    XClearWindow(resources.display, resources.window);
    const char* message = "YOU HAVE PASSED LEVEL 1";
    XSetForeground(resources.display, resources.gc, BlackPixel(resources.display, resources.screen));

    // Cargar fuente 
    XFontStruct* font = XLoadQueryFont(resources.display, "12x24");
    if (!font) {
        // Si no se puede cargar la fuente, usar una fuente predeterminada
        fprintf(stderr, "No se pudo cargar la fuente grande, usando la fuente predeterminada.\n");
        font = XLoadQueryFont(resources.display, "fixed");
        if (!font) {
            fprintf(stderr, "No se pudo cargar la fuente predeterminada.\n");
            return; // Salir si no se puede cargar ninguna fuente
        }
    }

    XSetFont(resources.display, resources.gc, font->fid);

    // Dibujar el mensaje de victoria en el centro de la pantalla
    int text_width = XTextWidth(font, message, strlen(message));
    XDrawString(resources.display, resources.window, resources.gc, (resources.WIDTH - text_width) / 2, resources.HEIGHT / 2 - 20, message, strlen(message));
    
    // Liberar la fuente cargada
    if (font) {
        XFreeFont(resources.display, font);
        XClearWindow(resources.display, resources.window);
    const char* message = "Game Over";
    }

    XFlush(resources.display);
}

void *drawLoop(void *arg) {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 12000000L;

    while (1) {
        if (resources.game_state == GAME_OVER) {
            drawGameOver();
            while (resources.game_state == GAME_OVER) {
                nanosleep(&ts, NULL);
            }
        } else if (resources.game_state == GAME_RUNNING) {
            pthread_mutex_lock(&resources.mutex);
            resources.should_draw = 1;
            pthread_mutex_unlock(&resources.mutex);

            drawShip();
            updateProjectiles();
            updateEnemies(); 
            drawProjectiles();
            drawEnemies();
            XFlush(resources.display);

            nanosleep(&ts, NULL); 
            // if (resources.remaining_enemies<=0 && resources.active_enemies==0)
            // {
            //     if (resources.current_level==LEVEL_1)
            //     {
            //         drawWinnLevelOne();
            //         resources.current_level=LEVEL_2;
                    
            //     }
                
            // }
            
        }
    }
    return NULL;
}