#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Visual/Resources/xresources.h"
#include "Visual/Draw/draw.h"
#include "Game_Logic/Ship/ship.h"
#include "Game_Logic/Projectiles/projectile.h"
#include "Game_Logic/Aliens/enemy.h"


void resetGame() {
    // Reiniciar los recursos del juego
    pthread_mutex_lock(&resources.mutex);
    initEnemies();
    resources.shipPos.x = resources.WIDTH / 2;
    resources.shipPos.y = resources.HEIGHT - 50;
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
        resources.projectiles[i].active = 0;
    }
    resources.game_state = GAME_RUNNING;
    resources.should_draw = 1;
    pthread_mutex_unlock(&resources.mutex);
}

void *mouseControl(void *arg) {
    XEvent event;
    while (1) {
        XNextEvent(resources.display, &event);
        if (resources.game_state == GAME_RUNNING) {
            if (event.type == MotionNotify) {
                int new_x = event.xmotion.x;
                int new_y = event.xmotion.y;
                int can_move = 1;
                Enemy* enemies = getEnemies();
                for (int i = 0; i < MAX_ENEMIES; ++i) {
                    if (enemies[i].active &&
                        new_x >= enemies[i].x - 20 &&
                        new_x <= enemies[i].x + 20 &&
                        new_y >= enemies[i].y - 20 &&
                        new_y <= enemies[i].y + 20) {
                        can_move = 0;
                        resources.game_state = GAME_OVER;  // Game Over si la nave choca con un alien
                        break;
                    }
                }
                if (can_move) { //Detectar y actualizar movimientos del mouse válidos
                    pthread_mutex_lock(&resources.mutex);
                    updateShipPosition(new_x, new_y);
                    resources.should_draw = 1;
                    pthread_mutex_unlock(&resources.mutex);
                    pthread_cond_signal(&resources.cond);
                }
            } else if (event.type == ButtonPress && event.xbutton.button == 1) { //Detectar disparos
                pthread_mutex_lock(&resources.mutex);
                ShipPosition shipPos = getShipPosition();
                fireProjectile(shipPos.x, shipPos.y);
                resources.should_draw = 1;
                pthread_mutex_unlock(&resources.mutex);
                pthread_cond_signal(&resources.cond);
            }
        } else if (resources.game_state == GAME_OVER) { //Detectar si se presiona Restart
            if (event.type == ButtonPress && event.xbutton.button == 1) {
                int click_x = event.xbutton.x;
                int click_y = event.xbutton.y;
                int restart_text_width = XTextWidth(XQueryFont(resources.display, XGContextFromGC(resources.gc)), "Restart", strlen("Restart"));
                int button_x = (resources.WIDTH - restart_text_width) / 2 - 10;
                int button_y = resources.HEIGHT / 2 + 30;
                int button_width = restart_text_width + 20;
                int button_height = 30;
                if (click_x >= button_x && click_x <= button_x + button_width &&
                    click_y >= button_y && click_y <= button_y + button_height) {
                    resetGame();
                }
            }
        }
    }
    return NULL;
}

int main() {
    initXResources();

    pthread_t mouseThread, drawThread;
    if (pthread_create(&mouseThread, NULL, mouseControl, NULL) != 0) {
        fprintf(stderr, "Error al crear el hilo para control del ratón.\n");
        cleanupXResources();
        return EXIT_FAILURE;
    }

    if (pthread_create(&drawThread, NULL, drawLoop, NULL) != 0) {
        fprintf(stderr, "Error al crear el hilo para dibujo.\n");
        pthread_cancel(mouseThread);
        pthread_join(mouseThread, NULL);
        cleanupXResources();
        return EXIT_FAILURE;
    }

    pthread_join(mouseThread, NULL);
    pthread_join(drawThread, NULL);

    cleanupXResources();
    return EXIT_SUCCESS;
}
