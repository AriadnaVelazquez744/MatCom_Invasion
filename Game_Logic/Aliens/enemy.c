#include "enemy.h"
#include <stdlib.h>
#include "../../Visual/Resources/xresources.h"

extern struct XResources resources;
Enemy enemies[MAX_ENEMIES];
int move_counter = 0;  

void initEnemies() {
    for (int i = 0; i < MAX_ENEMIES; ++i) {
        enemies[i].x = rand() % (resources.WIDTH - 20) + 10;  
        enemies[i].y = rand() % 20;  // Generar en la parte superior de la pantalla
        enemies[i].life = 3;
        enemies[i].active = 1;
    }
}

void updateEnemies() {
    move_counter++;
    if (move_counter < 5) {  // Ajustar el valor para cambiar la velocidad de movimiento de los aliens
        return; 
    }
    move_counter = 0;  // Reiniciar el contador

    for (int i = 0; i < MAX_ENEMIES; ++i) {
        if (enemies[i].active) {
            if (enemies[i].y < resources.HEIGHT - 98) { 
                enemies[i].y += 1;  // Movimiento simple hacia abajo(por ahora)
            } else {
                resources.game_state = GAME_OVER;  // Game Over si el enemigo llega a la parte inferior(suelo)
            }
        }
    }
}

Enemy* getEnemies() {
    return enemies;
}
