#ifndef ENT_H
#define ENT_H

#include <SDL2/SDL.h>
#include "../headers/SDL2/SDL_mixer.h"
#include "../headers/SDL2/SDL_image.h"
#include "../headers/SDL2/SDL_net.h"
#include "../headers/SDL2/SDL_ttf.h"

typedef struct
{
    int id;
    int lenght;
    int isSunk;
    int isCount;
    SDL_Point actualCase;
    int flipped;
    SDL_Texture *texture;

} boat;

typedef struct
{
    int mili;
    int sec;
    int min;
    int hours;

} gameTime;

typedef struct
{
    int running;
    int endTime;
    int initialTime;
    gameTime time;

} timer;

void SDL_start( SDL_Renderer **renderer, Mix_Music *music);
void SDL_stop( SDL_Renderer **renderer, Mix_Music **music);
int mainMenuDisplay(SDL_Renderer **renderer, SDL_Cursor **cursor, int *mainContinue, int clientStatut);
void plateauPlacement(SDL_Renderer **renderer, SDL_Cursor **cursor, int *mainContinue, int playerGrid[], int *menuContinue, boat boatsPlayer[]);
int getPlayerCase(int i);
int getIACase(int i);
int getIAProbaCase(int i);
void setPlayerCase(int i, int value);
void setIACase(int i, int value);
void setIAProbaCase(int i, int value);
int plateauAttack(SDL_Renderer **renderer, SDL_Cursor** cursor, int *mainContinue, int *menuContinue, int isStart, int gridIA[], int gridPlayer[], boat boats[], int IANbStrokes);
void playVideo(SDL_Renderer **renderer, SDL_Cursor **cursor, int *mainContinue, int *menuContinue, char location[], int nbFrames, int fps);
void playRes(SDL_Renderer **renderer, SDL_Cursor **cursor, int *mainContinue, int *menuContinue, int isHit);
void displayWin(SDL_Renderer **renderer, SDL_Cursor** cursor, int *mainContinue, int *menuContinue, timer MainTimer, int scoreIA, int scorePlayer);
void saveSettings();

int getRandoms(int lower, int upper);

#ifdef __cplusplus

#endif

#endif