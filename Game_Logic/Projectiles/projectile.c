#include "projectile.h"
#include "../Aliens/enemy.h"
#include "../../Visual/Resources/xresources.h"

extern struct XResources resources;

// Inicializa un nuevo projectil
void fireProjectile(int x, int y)
{
    for (int i = 0; i < MAX_PROJECTILES; ++i)
    {
        if (!resources.projectiles[i].active)
        {
            resources.projectiles[i].x = x;
            resources.projectiles[i].y = y - 15;
            resources.projectiles[i].active = 1;
            break;
        }
    }
}
// Actualiza la ubicación de los projectiles activos
void updateProjectiles()
{
    Projectile *projectiles = getProjectiles();
    for (int i = 0; i < MAX_PROJECTILES; ++i)
    {
        if (projectiles[i].active)
        {
            projectiles[i].y -= 5;
            if (projectiles[i].y < 0)
            {
                projectiles[i].active = 0;
            }
        }
    }
    checkCollisions();
}

Projectile *getProjectiles()
{
    return resources.projectiles;
}
// Revisa si un disparo llega a algún alien
void checkCollisions()
{
    Projectile *projectiles = getProjectiles();
    Enemy *enemies = getEnemies();
    // Marca aliens a eliminar(su vida está en 0)
    int enemiesToRemove[MAX_ENEMIES] = {0};

    for (int i = 0; i < MAX_PROJECTILES; ++i)
    {
        if (projectiles[i].active)
        {
            for (int j = 0; j < resources.max_enemies; ++j)
            {
                if (enemies[j].active &&
                    projectiles[i].x >= enemies[j].x - 10 &&
                    projectiles[i].x <= enemies[j].x + 10 &&
                    projectiles[i].y >= enemies[j].y - 10 &&
                    projectiles[i].y <= enemies[j].y + 10)
                {
                    enemies[j].life -= 1;
                    projectiles[i].active = 0;
                    if (enemies[j].life <= 0)
                    {
                        enemiesToRemove[j] = 1;
                    }
                    break;
                }
            }
        }
    }

    // Eliminar los enemigos marcados después de la iteración
    for (int j = 0; j < resources.max_enemies; ++j)
    {
        if (enemiesToRemove[j])
        {
            enemies[j].active = 0;
            resources.destroyed_enemies++;
        }
        if (resources.destroyed_enemies >= resources.max_enemies)
        {
            resources.game_state = GAME_WIN_LEVEL; // Cambiar a estado de victoria
        }
    }
}
