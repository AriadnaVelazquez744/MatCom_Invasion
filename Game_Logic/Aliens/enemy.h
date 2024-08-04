#ifndef ENEMY_H
#define ENEMY_H

#define MAX_ENEMIES 7

typedef struct {
    int x;
    int y;
    int life;
    int active;
} Enemy;

void initEnemies();
void updateEnemies();
Enemy* getEnemies();

#endif 

