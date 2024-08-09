#include "enemy.h"
#include <stdlib.h>
#include <stdio.h>
#include "../../Visual/Resources/xresources.h"
#include <math.h>

extern struct XResources resources;
Enemy enemies[MAX_ENEMIES];
int move_counter = 0;
int last_generated=0;

//Función para ordenar los aliens de menos a más vida
int compare(const void *a, const void *b) {
    Enemy *EnemyA = (Enemy *)a;
    Enemy *EnemyB = (Enemy *)b;
    return (EnemyA->life - EnemyB->life);
}

//Orden alternado(comprobar si funciona correctamente)
void alternateSort(Enemy enemies[]) {
    // Ordenar el array por tipo
    qsort(enemies, resources.max_enemies, sizeof(Enemy), compare);

    Enemy result[MAX_ENEMIES];

    int type1Index = 0;  // Índice para el tipo 1
    int type2Index = 0;  // Índice para el tipo 2

    // Encuentra el primer índice del tipo 2
    while (type2Index < resources.max_enemies && enemies[type2Index].type == ENEMY_TYPE_1) {
        type2Index++;
    }
    int first_type2Index=type2Index;

    int i = 0;
    // Alternar tipos en el array resultante
    while (type1Index < first_type2Index && type2Index < resources.max_enemies) {
        result[i++] = enemies[type1Index++]; // Añadir tipo 1
        result[i++] = enemies[type2Index++]; // Añadir tipo 2
    }

    // Si quedan elementos de tipo 1, añádelos al final
    while (type1Index < first_type2Index) {
        result[i++] = enemies[type1Index++];
    }
    // Si quedan elementos de tipo 2, añádelos al final
    while (type2Index < resources.max_enemies) {
        result[i++] = enemies[type2Index++];
    }

    // Copiar el resultado de vuelta al array original
    for (int j = 0; j < resources.max_enemies; j++) {
        enemies[j] = result[j];
    }
}

void initEnemies()
{

    //Limita la cantidad de enemigos de tipo 2 que se pueden generar en cada nivel
    int max_remaining_type2_level2=6;
    int max_remaining_type2_level3=11;
    for (int i = 0; i < resources.max_enemies; ++i)
    {
        enemies[i].active = 0; // Inicialmente todos los enemigos están inactivos
        if (resources.current_level==LEVEL_1)
        {
            enemies[i].type = ENEMY_TYPE_1;
                    enemies[i].life = 3;
        }
        else if (resources.current_level==LEVEL_2)
        {
            
            if (max_remaining_type2_level2>0)
            {
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
                    max_remaining_type2_level2--;
                }
            }
            else
            {
                enemies[i].type = ENEMY_TYPE_1;
                    enemies[i].life = 3;
            }
            
            
        }
        else
        {
            if (max_remaining_type2_level3>0)
            {
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
                    max_remaining_type2_level3--;
                }
            }
            else
            {
                enemies[i].type = ENEMY_TYPE_1;
                    enemies[i].life = 3;
            }
        }
        if (resources.max_enemies<MAX_ENEMIES)
        {
            for (int j = resources.max_enemies; j <MAX_ENEMIES ; ++j)
            {
                enemies[j].active = 0;
            }
            
        }
        
        
        
        
    }
    if (resources.game_mode==MODE_PROGRESSIVE)
    {
       //Política SJF
       qsort(enemies, resources.max_enemies, sizeof(Enemy), compare);
    }
    else if (resources.game_mode==MODE_ALTERNATE)//Round Robin(comprobar)
    {
        alternateSort(enemies);
    }
    //Modo Random es simplemente no ordenar el array generado(FIFO)
    //Se inicializan los contadores para determinar victoria en el nivel
    resources.remaining_enemies=resources.max_enemies;
    resources.destroyed_enemies=0;
    last_generated=0;
}



void generateEnemy()
{
    for (int i = last_generated; i < resources.max_enemies; ++i)
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

    for(int i = 0; i < resources.max_enemies; ++i)
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
