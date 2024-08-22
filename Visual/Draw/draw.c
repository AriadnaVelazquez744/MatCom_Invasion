#include "draw.h"
#include "../Resources/xresources.h"
#include "../../Game_Logic/Projectiles/projectile.h"
#include "../../Game_Logic/Aliens/enemy.h"
#include <time.h>
#include <stdio.h>
#include <string.h> 

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h> // Incluimos TTF para manejar texto
#include <X11/Xlib.h>


extern struct XResources resources;
extern unsigned long orange_pixel;

SDL_Window* sdl_window = NULL;
SDL_Renderer* sdl_renderer = NULL;
SDL_Texture* background_texture = NULL;
TTF_Font* font = NULL; // Fuente para texto

SDL_Texture* game_running_texture = NULL;
SDL_Texture* select_screen = NULL;
SDL_Texture* ship = NULL;
SDL_Texture* creature_T1 = NULL;
SDL_Texture* creature_T2 = NULL;


void init_sdl_for_background() 
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        fprintf(stderr, "SDL no pudo inicializarse. SDL_Error: %s\n", SDL_GetError());
        return;
    }

    if (TTF_Init() == -1) 
    {
        fprintf(stderr, "SDL_ttf no pudo inicializarse. TTF_Error: %s\n", TTF_GetError());
        SDL_Quit();
        return;
    }

    // Crear una ventana SDL usando el mismo tamaño de la ventana X11
    sdl_window = SDL_CreateWindowFrom((void*)resources.window);
    if (sdl_window == NULL) 
    {
        fprintf(stderr, "La ventana SDL no pudo ser creada. SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdl_renderer == NULL) 
    {
        fprintf(stderr, "El renderer SDL no pudo ser creado. SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(sdl_window);
        SDL_Quit();
        return;
    }

    // Cargar fuente
    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf", 32);
    
    if (!font) 
    {
        fprintf(stderr, "No se pudo cargar la fuente. TTF_Error: %s\n", TTF_GetError());
        SDL_DestroyTexture(background_texture);
        SDL_DestroyRenderer(sdl_renderer);
        SDL_DestroyWindow(sdl_window);
        SDL_Quit();
        return;
    }
}

void cleanup_sdl() 
{
    if (font) 
    {
        TTF_CloseFont(font);
    }
    if (background_texture) 
    {
        SDL_DestroyTexture(background_texture);
    }
    if (sdl_renderer) 
    {
        SDL_DestroyRenderer(sdl_renderer);
    }
    if (sdl_window) 
    {
        SDL_DestroyWindow(sdl_window);
    }
    TTF_Quit();
    SDL_Quit();
}

void load_textures() 
{
    // Cargar la textura para GAME_RUNNING
    SDL_Surface* loaded_surface = IMG_Load("galaxy.jpeg");
    game_running_texture = SDL_CreateTextureFromSurface(sdl_renderer, loaded_surface);
    SDL_FreeSurface(loaded_surface);
    
    // Cargar texturas para otros estados
    loaded_surface = IMG_Load("night_sky.png");
    select_screen = SDL_CreateTextureFromSurface(sdl_renderer, loaded_surface);
    SDL_FreeSurface(loaded_surface);

    // Cargar la textura para la nave
    loaded_surface = IMG_Load("ship.png");
    ship = SDL_CreateTextureFromSurface(sdl_renderer, loaded_surface);
    SDL_FreeSurface(loaded_surface);

    // Cargar texturas para los enemigos
    loaded_surface = IMG_Load("creature_T1-sheet.png");
    creature_T1 = SDL_CreateTextureFromSurface(sdl_renderer, loaded_surface);
    SDL_FreeSurface(loaded_surface);

    loaded_surface = IMG_Load("creature_T2-sheet.png");
    creature_T2 = SDL_CreateTextureFromSurface(sdl_renderer, loaded_surface);
    SDL_FreeSurface(loaded_surface);
}


void render_background(SDL_Texture* texture) 
{
    if (sdl_renderer && texture) {
        SDL_RenderClear(sdl_renderer);
        SDL_RenderCopy(sdl_renderer, texture, NULL, NULL);
    }
}

void drawText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color textColor, SDL_Color outlineColor) 
{
    // Crear el contorno
    TTF_SetFontOutline(font, 2); // Grosor del contorno
    SDL_Surface* outlineSurface = TTF_RenderText_Solid(font, text, outlineColor);
    SDL_Texture* outlineTexture = SDL_CreateTextureFromSurface(renderer, outlineSurface);

    // Renderizar contorno
    SDL_Rect outlineQuad = {x - 2, y - 2, outlineSurface->w, outlineSurface->h};
    SDL_RenderCopy(renderer, outlineTexture, NULL, &outlineQuad);

    // Crear el texto
    TTF_SetFontOutline(font, 0); // Desactivar contorno
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // Renderizar texto
    SDL_Rect textQuad = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textQuad);

    SDL_FreeSurface(outlineSurface);
    SDL_DestroyTexture(outlineTexture);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

//Dibujo de la nave
void drawShip() 
{
    ShipPosition shipPos = getShipPosition();
    
    // Crear rectángulo donde se va a renderizar la textura de la nave
    //SDL_Rect ship_rect = {shipPos.x - 30, shipPos.y - 22, 60, 44};

    // Ampliar el tamaño de la nave y mantener el centro
    SDL_Rect ship_rect = {shipPos.x - 40, shipPos.y - 30, 80, 60};  // Aumentado


    // Dibujar la textura de la nave
    SDL_RenderCopy(sdl_renderer, ship, NULL, &ship_rect);
}

//Dibujo de los disparos efectuados
void drawProjectiles() 
{    
    SDL_SetRenderDrawColor(sdl_renderer, 255, 69, 0, 255);  // Rojo Naranja brillante para proyectiles
    Projectile *projectiles = getProjectiles();
    for (int i = 0; i < MAX_PROJECTILES; ++i) 
    {
        if (projectiles[i].active) 
        {
            SDL_Rect projectile_rect = {projectiles[i].x - 3, projectiles[i].y - 10, 6, 10};
            SDL_RenderFillRect(sdl_renderer, &projectile_rect);
        }
    }
}

//Dibujo de los aliens
void drawEnemies() 
{
    Enemy *enemies = getEnemies();
    static int frames[MAX_ENEMIES] = {0};  // Array local para almacenar el frame actual
    static int frame_counters[MAX_ENEMIES] = {0};  // Contadores para cambiar frames


    int frame_width_T1, frame_height_T1, frame_width_T2, frame_height_T2;

    SDL_QueryTexture(creature_T1, NULL, NULL, &frame_width_T1, &frame_height_T1);
    frame_width_T1 /= 4;  // Ya que el sheet tiene 4 cuadros de ancho
    SDL_QueryTexture(creature_T2, NULL, NULL, &frame_width_T2, &frame_height_T2);
    frame_width_T2 /= 4;  // Ya que el sheet tiene 4 cuadros de ancho

    for (int i = 0; i < MAX_ENEMIES; ++i) 
    {
        if (enemies[i].active) 
        {
            SDL_Rect src_rect, dest_rect;

            if (enemies[i].type == ENEMY_TYPE_1) 
            {
                src_rect = (SDL_Rect){frames[i] * frame_width_T1, 0, frame_width_T1, frame_height_T1};
                dest_rect = (SDL_Rect){enemies[i].x - frame_width_T1 / 2, enemies[i].y - frame_height_T1 / 2, frame_width_T1 * 1.7, frame_height_T1 * 1.7};  // Aumentado

                SDL_RenderCopy(sdl_renderer, creature_T1, &src_rect, &dest_rect);
            } 
            else if (enemies[i].type == ENEMY_TYPE_2) 
            {
                src_rect = (SDL_Rect){frames[i] * frame_width_T2, 0, frame_width_T2, frame_height_T2};
                dest_rect = (SDL_Rect){enemies[i].x - frame_width_T2 / 2, enemies[i].y - frame_height_T2 / 2, frame_width_T2 * 1.8, frame_height_T2 * 1.8};  // Aumentado

                SDL_RenderCopy(sdl_renderer, creature_T2, &src_rect, &dest_rect);
            }
            
            // Controlar el cambio de frame cada 10 ciclos
            frame_counters[i]++;
            if (frame_counters[i] >= 6) 
            {
                frames[i] = (frames[i] + 1) % 4; // Cambiar cuadro para la animación
                frame_counters[i] = 0;
            }        
        }
    }
}

// Pantalla de selección de modo de juego
void drawSelectModeScreen() 
{
    render_background(select_screen);
    SDL_Color textColor = {255, 182, 193, 255}; // Rosado claro (Light Pink)
    SDL_Color outlineColor = {255, 0, 0, 255};  // Rojo brillante

    drawText(sdl_renderer, font, "Select Game Mode:", resources.WIDTH / 2 - 100, resources.HEIGHT / 2 - 60, textColor, outlineColor);
    drawText(sdl_renderer, font, "1. Progressive", resources.WIDTH / 2 - 80, resources.HEIGHT / 2 - 10, textColor, outlineColor);
    drawText(sdl_renderer, font, "2. Alternate", resources.WIDTH / 2 - 80, resources.HEIGHT / 2 + 40, textColor, outlineColor);
    drawText(sdl_renderer, font, "3. Random", resources.WIDTH / 2 - 80, resources.HEIGHT / 2 + 90, textColor, outlineColor);

    SDL_RenderPresent(sdl_renderer);
}

// Pantalla Final (Game Over)
void drawGameOver() 
{
    render_background(select_screen);  // Usar la textura específica
    SDL_Color textColor = {255, 182, 193, 255}; // Rosado claro (Light Pink)
    SDL_Color outlineColor = {255, 0, 0, 255};  // Rojo brillante

    drawText(sdl_renderer, font, "Game Over", resources.WIDTH / 2 - 85, resources.HEIGHT / 2 - 20, textColor, outlineColor);
    drawText(sdl_renderer, font, "Restart", resources.WIDTH / 2 - 60, resources.HEIGHT / 2 + 30, textColor, outlineColor);

    SDL_RenderPresent(sdl_renderer);
}

// Pantalla de victoria del nivel 1
void drawWinnLevelOne() 
{
    render_background(select_screen);  // Usar la textura específica
    SDL_Color textColor = {255, 182, 193, 255}; // Rosado claro (Light Pink)
    SDL_Color outlineColor = {255, 0, 0, 255};  // Rojo brillante

    drawText(sdl_renderer, font, "You have passed Level 1!", resources.WIDTH / 2 - 160, resources.HEIGHT / 2 - 20, textColor, outlineColor);
    drawText(sdl_renderer, font, "Press Right-Click to start Level 2", resources.WIDTH / 2 - 225, resources.HEIGHT / 2 + 30, textColor, outlineColor);

    SDL_RenderPresent(sdl_renderer);
}

// Pantalla de victoria del nivel 2
void drawWinnLevelTwo() 
{
    render_background(select_screen);  // Usar la textura específica
    SDL_Color textColor = {255, 182, 193, 255}; // Rosado claro (Light Pink)
    SDL_Color outlineColor = {255, 0, 0, 255};  // Rojo brillante

    drawText(sdl_renderer, font, "You have passed Level 2!", resources.WIDTH / 2 - 160, resources.HEIGHT / 2 - 20, textColor, outlineColor);
    drawText(sdl_renderer, font, "Press Right-Click to start Level 3", resources.WIDTH / 2 - 225, resources.HEIGHT / 2 + 30, textColor, outlineColor);

    SDL_RenderPresent(sdl_renderer);
}

// Pantalla de victoria del nivel 3
void drawWinnLevelThree() 
{
    render_background(select_screen);  // Usar la textura específica
    SDL_Color textColor = {255, 182, 193, 255}; // Rosado claro (Light Pink)
    SDL_Color outlineColor = {255, 0, 0, 255};  // Rojo brillante

    drawText(sdl_renderer, font, "You have passed Level 3!", resources.WIDTH / 2 - 160, resources.HEIGHT / 2 - 20, textColor, outlineColor);
    drawText(sdl_renderer, font, "Press Right-Click to complete Game", resources.WIDTH / 2 - 225, resources.HEIGHT / 2 + 30, textColor, outlineColor);

    SDL_RenderPresent(sdl_renderer);
}

// Pantalla de victoria final (You Won)
void drawWinn() 
{
    render_background(select_screen);  // Usar la textura específica
    SDL_Color textColor = {255, 182, 193, 255}; // Rosado claro (Light Pink)
    SDL_Color outlineColor = {255, 0, 0, 255};  // Rojo brillante

    drawText(sdl_renderer, font, "You Won", resources.WIDTH / 2 - 60, resources.HEIGHT / 2 - 20, textColor, outlineColor);
    drawText(sdl_renderer, font, "Press Right-Click to restart", resources.WIDTH / 2 - 210, resources.HEIGHT / 2 + 30, textColor, outlineColor);

    SDL_RenderPresent(sdl_renderer);
}


void *drawLoop(void *arg) 
{
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 12000000L;

    init_sdl_for_background(); // Inicializar SDL y cargar el fondo
    load_textures(); // Cargar todas las texturas

    while (1) 
    {
        // Dibujar las pantallas estáticas usando SDL
        if (resources.game_state == GAME_SELECT_MODE) 
        {
            drawSelectModeScreen();
        } 
        else if (resources.game_state == GAME_OVER) 
        {
            drawGameOver();  
        } 
        else if (resources.game_state == GAME_WIN_LEVEL) 
        {
            if (resources.current_level == LEVEL_1) 
            {
                drawWinnLevelOne();
            } 
            else if (resources.current_level == LEVEL_2) 
            {
                drawWinnLevelTwo();
            } 
            else 
            {
                drawWinnLevelThree();
            }
        } 
        else if (resources.game_state == GAME_WIN) 
        {
            drawWinn();
        } 
        else if (resources.game_state == GAME_RUNNING) 
        {
            pthread_mutex_lock(&resources.mutex);
            resources.should_draw = 1;
            pthread_mutex_unlock(&resources.mutex);

            render_background(game_running_texture); // Usar textura de game running

            drawShip();
            updateProjectiles();
            updateEnemies(); 
            drawProjectiles();
            drawEnemies();

            SDL_RenderPresent(sdl_renderer);
            nanosleep(&ts, NULL);
        }
    }

    cleanup_sdl();
    return NULL;
}
