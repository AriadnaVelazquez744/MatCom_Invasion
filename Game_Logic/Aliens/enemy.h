#ifndef ENEMY_H
#define ENEMY_H

#define MAX_ENEMIES 4

typedef enum {
    ENEMY_TYPE_1,
    ENEMY_TYPE_2,
} EnemyType;


typedef struct {
    int x;
    int y;
    int life;
    int active;
    float phase;
    EnemyType type;
} Enemy;

void initEnemies();
void updateEnemies();
void generateEnemy();
Enemy* getEnemies();

#endif 

