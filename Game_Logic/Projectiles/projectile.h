#ifndef PROJECTILE_H
#define PROJECTILE_H

#define MAX_PROJECTILES 15

//Propiedades de los proyectiles
typedef struct {
    int x;
    int y;
    int active;
} Projectile;

void fireProjectile(int x, int y);
void updateProjectiles();
Projectile* getProjectiles();
void checkCollisions();

#endif 


