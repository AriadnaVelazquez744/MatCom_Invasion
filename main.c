#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Visual/Resources/xresources.h"
#include "Visual/Draw/draw.h"
#include "Game_Logic/Ship/ship.h"
#include "Game_Logic/Projectiles/projectile.h"
#include "Game_Logic/Aliens/enemy.h"

#define ENEMY_GENERATION_INTERVAL 2 // Intervalo para generar enemigos (5 segundos)

//Reiniciar juego
void resetGame()
{
    // Reiniciar los recursos del juego
    pthread_mutex_lock(&resources.mutex);
    resources.current_level = LEVEL_1;
    resources.max_enemies = 4;
    initEnemies();
    resources.shipPos.x = resources.WIDTH / 2;
    resources.shipPos.y = resources.HEIGHT - 50;
    for (int i = 0; i < MAX_PROJECTILES; ++i)
    {
        resources.projectiles[i].active = 0;
    }
    //resources.game_state = GAME_RUNNING;
    resources.game_state = GAME_SELECT_MODE;
    resources.should_draw = 1;
    pthread_mutex_unlock(&resources.mutex);
}

//Inicializar próximo nivel
void startNextLevel()
{
    pthread_mutex_lock(&resources.mutex);

    if (resources.current_level == LEVEL_1)
    {
        resources.current_level = LEVEL_2;
        resources.max_enemies = 7;
    }
    else if (resources.current_level == LEVEL_2)
    {
        resources.current_level = LEVEL_3;
        resources.max_enemies = 10;
    }

    initEnemies();
    resources.game_state = GAME_RUNNING;
    pthread_mutex_unlock(&resources.mutex);
}

//Detectar y manejar eventos del mouse
void *mouseControl(void *arg)
{
    XEvent event;
    while (1)
    {
        XNextEvent(resources.display, &event);
        if (resources.game_state == GAME_SELECT_MODE) {
            if (event.type == ButtonPress && event.xbutton.button == 1) {  // Clic izquierdo
                int click_x = event.xbutton.x;
                int click_y = event.xbutton.y;

                if (click_y >= resources.HEIGHT / 2 - 20 && click_y < resources.HEIGHT / 2 + 20) {
                    resources.game_mode = MODE_PROGRESSIVE;
                    resetGame();
                    resources.game_state = GAME_RUNNING;
                } else if (click_y >= resources.HEIGHT / 2 + 20 && click_y < resources.HEIGHT / 2 + 60) {
                    resources.game_mode = MODE_ALTERNATE;
                    resetGame();
                    resources.game_state = GAME_RUNNING;
                } else if (click_y >= resources.HEIGHT / 2 + 60 && click_y < resources.HEIGHT / 2 + 100) {
                    resources.game_mode = MODE_RANDOM;
                    resetGame();
                    resources.game_state = GAME_RUNNING;
                }
            }
        }else if (resources.game_state == GAME_RUNNING)
        {
            if (event.type == MotionNotify)
            {
                int new_x = event.xmotion.x;
                int new_y = event.xmotion.y;
                int can_move = 1;
                Enemy *enemies = getEnemies();
                for (int i = 0; i < MAX_ENEMIES; ++i)
                {
                    if (enemies[i].active &&
                        new_x >= enemies[i].x - 20 &&
                        new_x <= enemies[i].x + 20 &&
                        new_y >= enemies[i].y - 20 &&
                        new_y <= enemies[i].y + 20)
                    {
                        can_move = 0;
                        resources.game_state = GAME_OVER; // Game Over si la nave choca con un alien
                        break;
                    }
                }
                if (can_move)
                { // Detectar y actualizar movimientos del mouse válidos
                    pthread_mutex_lock(&resources.mutex);
                    updateShipPosition(new_x, new_y);
                    resources.should_draw = 1;
                    pthread_mutex_unlock(&resources.mutex);
                    pthread_cond_signal(&resources.cond);
                }
            }
            else if (event.type == ButtonPress && event.xbutton.button == 1)
            { // Detectar disparos
                pthread_mutex_lock(&resources.mutex);
                ShipPosition shipPos = getShipPosition();
                fireProjectile(shipPos.x, shipPos.y);
                resources.should_draw = 1;
                pthread_mutex_unlock(&resources.mutex);
                pthread_cond_signal(&resources.cond);
            }
        }
        else if (resources.game_state == GAME_OVER)
        { // Detectar si se presiona Restart
            if (event.type == ButtonPress && event.xbutton.button == 1)
            {
                int click_x = event.xbutton.x;
                int click_y = event.xbutton.y;
                int restart_text_width = XTextWidth(XQueryFont(resources.display, XGContextFromGC(resources.gc)), "Restart", strlen("Restart"));
                int button_x = (resources.WIDTH - restart_text_width) / 2 - 10;
                int button_y = resources.HEIGHT / 2 + 30;
                int button_width = restart_text_width + 20;
                int button_height = 30;
                if (click_x >= button_x && click_x <= button_x + button_width &&
                    click_y >= button_y && click_y <= button_y + button_height)
                {
                    resetGame();
                }
            }
        }
        else if (resources.game_state == GAME_WIN_LEVEL)
        {
            if (event.type == ButtonPress && event.xbutton.button == 3)
            { 
                if (resources.current_level == LEVEL_3)
                {
                    resources.game_state = GAME_WIN;
                }
                else
                {
                    startNextLevel();
                }
            }
        }
        else if (resources.game_state == GAME_WIN)
        {
            if (event.type == ButtonPress && event.xbutton.button == 3)
            {
                resetGame();
            }
        }
    }
    return NULL;
}

//Generación progresiva de enemigos
void *enemyGenerationLoop(void *arg)
{
    struct timespec ts;
    if (resources.current_level == LEVEL_1)
    {
        ts.tv_sec = 4;
        ts.tv_nsec = 0;
    }
    else if (resources.current_level == LEVEL_2)
    {
        ts.tv_sec = 3;
        ts.tv_nsec = 0;
    }
    else
    {
        ts.tv_sec = ENEMY_GENERATION_INTERVAL;
        ts.tv_nsec = 0;
    }

    while (1)
    {
        nanosleep(&ts, NULL);
        if (resources.game_state == GAME_RUNNING)
        {
            pthread_mutex_lock(&resources.mutex);
            generateEnemy();
            resources.should_draw = 1;
            pthread_mutex_unlock(&resources.mutex);
            pthread_cond_signal(&resources.cond);
        }
    }
    return NULL;
}

int main()
{
    initXResources();
    resources.game_state = GAME_SELECT_MODE;
    srand(time(NULL));

    pthread_t mouseThread, drawThread, enemyGenerationThread;
    if (pthread_create(&mouseThread, NULL, mouseControl, NULL) != 0)
    {
        fprintf(stderr, "Error al crear el hilo para control del ratón.\n");
        cleanupXResources();
        return EXIT_FAILURE;
    }

    if (pthread_create(&drawThread, NULL, drawLoop, NULL) != 0)
    {
        fprintf(stderr, "Error al crear el hilo para dibujo.\n");
        pthread_cancel(mouseThread);
        pthread_join(mouseThread, NULL);
        cleanupXResources();
        return EXIT_FAILURE;
    }

    if (pthread_create(&enemyGenerationThread, NULL, enemyGenerationLoop, NULL) != 0)
    {
        fprintf(stderr, "Error al crear el hilo para generación de enemigos.\n");
        pthread_cancel(mouseThread);
        pthread_cancel(drawThread);
        pthread_join(mouseThread, NULL);
        pthread_join(drawThread, NULL);
        cleanupXResources();
        return EXIT_FAILURE;
    }

    pthread_join(mouseThread, NULL);
    pthread_join(drawThread, NULL);
    pthread_join(enemyGenerationThread, NULL);

    cleanupXResources();
    return EXIT_SUCCESS;
}
