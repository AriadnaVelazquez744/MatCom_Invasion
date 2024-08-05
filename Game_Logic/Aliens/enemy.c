#include "enemy.h"
#include <stdlib.h>
#include <stdio.h>
#include "../../Visual/Resources/xresources.h"
#include <math.h>

extern struct XResources resources;
Enemy enemies[MAX_ENEMIES];
int move_counter = 0;
int last_generated=0;


int compare(const void *a, const void *b) {
    Enemy *EnemyA = (Enemy *)a;
    Enemy *EnemyB = (Enemy *)b;
    return (EnemyA->life - EnemyB->life);
}

void initEnemies()
{
    for (int i = 0; i < MAX_ENEMIES; ++i)
    {
        enemies[i].active = 0; // Inicialmente todos los enemigos están inactivos
        int randType = rand() % 2;
                if (randType == 0)
                {
                    enemies[i].type = ENEMY_TYPE_1;
                    enemies[i].life = 3;
                }
                else
                {
                    enemies[i].type = ENEMY_TYPE_2;
                    enemies[i].life = 8;
                }
    }
    qsort(enemies, MAX_ENEMIES, sizeof(Enemy), compare);
    resources.remaining_enemies=MAX_ENEMIES;
    resources.active_enemies=0;
}



void generateEnemy()
{
    for (int i = last_generated; i < MAX_ENEMIES; ++i)
    {
        if (resources.remaining_enemies > 0)
        {
            if (!enemies[i].active)
            {
                enemies[i].x = rand() % (resources.WIDTH - 30) + 10; // Para evitar que se generen muy cerca de los bordes
                enemies[i].y = rand() % 20;                          // Generar en la parte superior de la pantalla
                 enemies[i].phase = (float)(rand() % 360);
                enemies[i].active = 1;

                resources.remaining_enemies--;
                resources.active_enemies++;
                last_generated=i+1;
                break;
            }
        }
    }
}

void updateEnemies()
{
    move_counter++;
    if (move_counter < 5)
    { // Ajustar el valor para cambiar la velocidad de movimiento de los aliens
        return;
    }
    move_counter = 0; // Reiniciar el contador

    for(int i = 0; i < MAX_ENEMIES; ++i)
    {
        if (enemies[i].active)
        {
            if (enemies[i].y < resources.HEIGHT - 98)
            {
                enemies[i].y += 1;                                                   // Movimiento simple hacia abajo
                enemies[i].x += (int)(6.5 * sin(enemies[i].phase * 3.141592 / 180)); // Movimiento ondulado
                enemies[i].phase += 10;                                              // Incrementar la fase
                // Asegurarse de que el enemigo no se salga de los límites de la pantalla
                if (enemies[i].x < 0)
                {
                    enemies[i].x = 16;
                    enemies[i].phase = 180 - enemies[i].phase; // Reflejar la fase para invertir el movimiento
                }
                else if (enemies[i].x > resources.WIDTH-90) {
                    enemies[i].x = resources.WIDTH-98;
                    enemies[i].phase = 180 - enemies[i].phase;  // Reflejar la fase para invertir el movimiento
                }
                if (enemies[i].phase >= 360)
                {
                    enemies[i].phase -= 360; // Mantener la fase dentro del rango [0, 360)
                }
            }
            else
            {
                resources.game_state = GAME_OVER; // Game Over si el enemigo llega a la parte inferior(suelo)
            }
        }
    }
}

Enemy *getEnemies()
{
    return enemies;
}
