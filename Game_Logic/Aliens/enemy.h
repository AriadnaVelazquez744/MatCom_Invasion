#ifndef ENEMY_H
#define ENEMY_H

#define MAX_ENEMIES 10 //Máximo de enemigos entre los límites de todos los niveles

//Tipos de aliens
typedef enum {
    ENEMY_TYPE_1,
    ENEMY_TYPE_2,
} EnemyType;

//Propiedades de los aliens
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

