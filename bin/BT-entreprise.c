/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#define SDL_MAIN_HANDLED

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "../headers/entreprise-header.h"

void printTime(timer element);

timer startTimer();

void netRequest(char *instruction);

void getClientInfo();

int checkClientInfo();

void setPlayerGrid(int grid[])
{

    for (int i = 0; i < 100; ++i)
    {
        setPlayerCase(i, grid[i]);
    }
}

void setIAGrid(int grid[])
{

    for (int i = 0; i < 100; ++i)
    {
        setIACase(i, grid[i]);
    }
}

void getPlayerGrid(int grid[])
{

    for (int i = 0; i < 100; ++i)
    {
        grid[i] = getPlayerCase(i);
    }
}

void getIAGrid(int grid[])
{

    for (int i = 0; i < 100; ++i)
    {
        grid[i] = getIACase(i);
    }
}

void printGrid(int isPlayer)
{

    int a = 0;

    for (int i = 0; i < 10; ++i)
    {
        printf("\n");

        for (int j = 0; j < 10; ++j)
        {
            if (isPlayer == 1)
            {
                printf("%d", getPlayerCase(a));
            }
            else
            {
                printf("%d", getIACase(a));
            }

            a++;
        }
    }

    printf("\n\n\n");
}

void setPlayerPlacement(SDL_Renderer **renderer, SDL_Cursor **cursor,
                        int *mainContinue, int *menuContinue, boat boats[])
{

    int grid[100];

    plateauPlacement(&*renderer, &*cursor, &*mainContinue, grid, &*menuContinue,
                     boats);
    setPlayerGrid(grid);
}

int getRandoms(int lower, int upper)
{
    int num = (rand() %
               (upper - lower + 1)) +
              lower;

    return num;
}

void setIAplacement(boat boatsIA[])
{

    int grid[100] = {0};
    int x = 0;
    int y = 0;
    int ready = 0;

    boatsIA[0].lenght = 5;
    boatsIA[1].lenght = 3;
    boatsIA[2].lenght = 4;
    boatsIA[3].lenght = 2;
    boatsIA[4].lenght = 3;

    for (int i = 0; i < 5; ++i)
    {
        boatsIA[i].isSunk = 0;
        boatsIA[i].id = i + 1;
        boatsIA[i].isCount = 0;
    }

    for (int i = 0; i < 5; ++i)
    {
        boatsIA[i].flipped = getRandoms(0, 1);
    }

    for (int a = 0; a < 5; ++a)
    {
        ready = 0;

        while (ready == 0)
        {

            int z = 0;

            x = getRandoms(0, 9);
            y = getRandoms(0, 9);

            for (int i = 1; i <= boatsIA[a].lenght; ++i)
            {
                if (boatsIA[a].flipped == 0)
                {
                    if (x + i <= 9 && grid[x + (y * 10) + i] == 0)
                    {
                        z++;
                    }
                }
                else
                {
                    if (y + i <= 9 && grid[x + (y * 10) + (i * 10)] == 0)
                    {
                        z++;
                    }
                }
            }

            if (z == boatsIA[a].lenght)
            {
                ready = 1;

                if (boatsIA[a].flipped == 0)
                {
                    for (int i = 1; i <= boatsIA[a].lenght; ++i)
                    {
                        grid[x + (y * 10) + i] = boatsIA[a].id;
                    }
                }
                else
                {

                    for (int i = 1; i <= boatsIA[a].lenght; ++i)
                    {
                        grid[x + (y * 10) + (i * 10)] = boatsIA[a].id;
                    }
                }
            }
        }
    }

    setIAGrid(grid);
}

int getPlayerAttackCase(SDL_Renderer **renderer, SDL_Cursor **cursor,
                        int *mainContinue, int *menuContinue, int isStart, boat boats[],
                        int IANbStrokes)
{

    int gridIA[100];
    int gridPlayer[100];
    int attackCase;
    int value;

    getIAGrid(gridIA);
    getPlayerGrid(gridPlayer);

    attackCase = plateauAttack(&*renderer, &*cursor, &*mainContinue,
                               &*menuContinue, isStart, gridIA, gridPlayer, boats, IANbStrokes);

    value = gridIA[attackCase];

    if (value != 0)
    {
        gridIA[attackCase] = gridIA[attackCase] * -1;
    }
    else
    {

        gridIA[attackCase] = -10;
    }

    setIAGrid(gridIA);

    return value;
}

void resetGrid(int *grid)
{

    for (int i = 0; i < 100; ++i)
    {
        grid[i] = 0;
    }
}

int getCaseValueOccurences(int value, int isPlayer)
{

    int occurences = 0;
    int grid[100];

    if (isPlayer)
    {
        getPlayerGrid(grid);
    }
    else
    {

        getIAGrid(grid);
    }

    for (int i = 0; i < 100; ++i)
    {
        if (grid[i] == value)
        {
            occurences++;
        }
    }

    return occurences;
}

int chanceOn(int value)
{

    int chance = getRandoms(1, 100);

    if (chance <= value)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int getIAattackCase(boat boatsPlayer[], int IAlevel)
{

    int gridPlayer[100];
    int tempProbaGrid[100] = {0};
    int probaGrid[100] = {0};
    int valuePlaces[100] = {0};
    int attackCase = 0;

    int x;
    int y;
    int isFlipped;

    getPlayerGrid(gridPlayer);

    int a = 0;
    int plcH = 0;
    int plcV = 0;

    int z = 0;
    int ready = 0;

    if (chanceOn(IAlevel))
    {
        do
        {
            resetGrid(probaGrid);

            for (int l = 0; l < IAlevel * 10; ++l)
            {
                for (int a = 0; a < 5; ++a)
                {
                    if (!boatsPlayer[a].isSunk)
                    {
                        ready = 0;

                        while (ready == 0)
                        {

                            z = 0;

                            x = getRandoms(0, 9);
                            y = getRandoms(0, 9);
                            isFlipped = getRandoms(0, 1);

                            for (int i = 1; i <= boatsPlayer[a].lenght; ++i)
                            {
                                plcH = x + (y * 10) + i;
                                plcV = x + (y * 10) + (i * 10);

                                if (isFlipped == 0)
                                {
                                    if (x + i <= 9 && tempProbaGrid[plcH] == 0 && gridPlayer[plcH] != -10)
                                    {
                                        z++;
                                    }
                                }
                                else
                                {
                                    if (y + i <= 9 && tempProbaGrid[plcV] == 0 && gridPlayer[plcV] != -10)
                                    {
                                        z++;
                                    }
                                }
                            }

                            if (z == boatsPlayer[a].lenght)
                            {
                                ready = 1;

                                if (isFlipped == 0)
                                {
                                    for (int i = 1; i <= boatsPlayer[a].lenght; ++i)
                                    {
                                        plcH = x + (y * 10) + i;

                                        tempProbaGrid[plcH] = 1;
                                        probaGrid[plcH] = probaGrid[plcH] + 1;

                                        if (((gridPlayer[(x - 1) + (y * 10) + i] < 0 &&
                                              gridPlayer[(x - 1) + (y * 10) + i] != -10) &&
                                             (x - 1) >= 0) ||
                                            ((gridPlayer[(x + 1) + (y * 10) + i] < 0 &&
                                              gridPlayer[(x + 1) + (y * 10) + i] != -10) &&
                                             (x + 1) <= 9))
                                        {
                                            probaGrid[x + (y * 10) + i] = probaGrid[x + (y * 10) + i] + 1;
                                        }
                                    }
                                }
                                else
                                {

                                    for (int i = 1; i <= boatsPlayer[a].lenght; ++i)
                                    {
                                        plcV = x + (y * 10) + (i * 10);

                                        tempProbaGrid[plcV] = 1;
                                        probaGrid[plcV] = probaGrid[plcV] + 1;

                                        if (((gridPlayer[x + ((y - 1) * 10) + (i * 10)] < 0 &&
                                              gridPlayer[x + ((y - 1) * 10) + (i * 10)] != -10) &&
                                             (y - 1) >= 0) ||
                                            ((gridPlayer[x + ((y + 1) * 10) + (i * 10)] < 0 &&
                                              gridPlayer[x + ((y + 1) * 10) + (i * 10)] != -10) &&
                                             (y + 1) <= 9))
                                        {
                                            probaGrid[x + ((y)*10) + (i * 10)] = probaGrid[x + (y * 10) + (i * 10)] + 1;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    resetGrid(tempProbaGrid);
                }
            }

            for (int i = 0; i < 100; ++i)
            {
                valuePlaces[i] = i;
            }

            int disorder = 1;

            for (int i = 0; i < 100 && disorder; ++i)
            {
                disorder = 0;
                for (int j = 1; j < 100 - i; ++j)
                {
                    if (probaGrid[j - 1] < probaGrid[j])
                    {
                        int temp = probaGrid[j - 1];
                        probaGrid[j - 1] = probaGrid[j];
                        probaGrid[j] = temp;

                        temp = valuePlaces[j - 1];
                        valuePlaces[j - 1] = valuePlaces[j];
                        valuePlaces[j] = temp;
                        disorder = 1;
                    }
                }
            }

            a = 0;

            do
            {

                attackCase = valuePlaces[a];

                a++;

            } while (gridPlayer[attackCase] < 0 && a < 100);

        } while (a == 100);
    }
    else
    {
        do
        {
            attackCase = getRandoms(0, 99);

        } while (gridPlayer[attackCase] < 0);
    }

    int caseValue = gridPlayer[attackCase];

    if (gridPlayer[attackCase] != 0)
    {
        gridPlayer[attackCase] = gridPlayer[attackCase] * -1;
    }
    else
    {

        gridPlayer[attackCase] = -10;
    }

    setPlayerGrid(gridPlayer);

    return caseValue;
}

int isPlayerBoatSunk(int boatId, int numberOfCases)
{

    if (getCaseValueOccurences(boatId * -1, 1) == numberOfCases)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int isIABoatSunk(int boatId, int numberOfCases)
{

    if (getCaseValueOccurences(boatId * -1, 0) == numberOfCases)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int isWin(boat boatsPlayer[], boat boatsIA[])
{

    int playerSunkCount = 0;
    int IASunkCount = 0;

    for (int i = 0; i < 5; ++i)
    {
        if (boatsIA[i].isSunk == 1)
        {
            IASunkCount++;
        }

        if (boatsPlayer[i].isSunk == 1)
        {
            playerSunkCount++;
        }
    }

    if (IASunkCount == 5 || playerSunkCount == 5)
    {
        return 1;
    }

    return 0;
}

void createProfile(SDL_Renderer **renderer,
                   int *mainContinue, SDL_Cursor **cursor,
                   int *menuContinue);

void getInitialLang(SDL_Renderer **renderer,
                    int *mainContinue, SDL_Cursor **cursor,
                    int *menuContinue);

int isSettingsInitialized();

void game(SDL_Renderer **renderer, int *mainContinue)
{

    SDL_Cursor *cursor;

    int menuContinue = 1;
    int win = 0;
    int IAlevel = 0;
    int attackCase = 0;
    int IANbStrokes = 0;
    int playerNbStrokes = 0;
    int maxStrokes = 0;
    int scorePlayer = 0;
    int scoreIA = 0;
    int result = 0;
    //int isCompetitive = 0;

    boat boatsIA[5];
    boat boatsPlayer[5];

    timer mainTimer;

    int isStart = 1;
    int isSunk = 0;

    getClientInfo();

    if (!isSettingsInitialized())
    {
        getInitialLang(&*renderer, &*mainContinue, &cursor, &menuContinue);
        if (!*mainContinue || !menuContinue)
            return;
    }

    if (checkClientInfo() != 0)
    {
        createProfile(&*renderer, &*mainContinue, &cursor, &menuContinue);
        if (!*mainContinue || !menuContinue)
            return;

        getClientInfo();
    }

    result = mainMenuDisplay(&*renderer, &cursor, &*mainContinue,
                             checkClientInfo(0));

    if (result & 0x000000FF)
    {
        maxStrokes = (result & 0xFF000000) >> 24;
        //isCompetitive = (result & 0x00FF0000) >> 16;
        IAlevel = (result & 0x0000FF00) >> 8;

        setPlayerPlacement(&*renderer, &cursor, &*mainContinue, &menuContinue,
                           boatsPlayer);
        if (!*mainContinue || !menuContinue)
            return;

        setIAplacement(boatsIA);

        mainTimer = startTimer();

        while (win == 0)
        {

            playerNbStrokes = 0;

            do
            {

                attackCase = getPlayerAttackCase(&*renderer, &cursor,
                                                 &*mainContinue, &menuContinue, isStart, boatsPlayer,
                                                 IANbStrokes);
                if (!*mainContinue || !menuContinue)
                    return;

                IANbStrokes = 0;

                isSunk = 0;

                if (attackCase != 0)
                {
                    scorePlayer++;

                    for (int i = 0; i < 5; ++i)
                    {
                        if (boatsIA[i].isSunk == 0 && isIABoatSunk(boatsIA[i].id,
                                                                   boatsIA[i].lenght) == 1)
                        {
                            boatsIA[i].isSunk = 1;

                            playRes(&*renderer, &cursor, &*mainContinue,
                                    &menuContinue, 2);
                            if (!*mainContinue || !menuContinue)
                                return;

                            isSunk = 1;
                        }
                    }

                    if (isSunk == 0)
                    {
                        playRes(&*renderer, &cursor, &*mainContinue,
                                &menuContinue, 1);
                        if (!*mainContinue || !menuContinue)
                            return;
                    }
                }
                else
                {

                    playRes(&*renderer, &cursor, &*mainContinue, &menuContinue,
                            0);
                    if (!*mainContinue || !menuContinue)
                        return;
                }

                isStart = 0;

                playerNbStrokes++;

                win = isWin(boatsPlayer, boatsIA);

            } while (attackCase != 0 && playerNbStrokes < maxStrokes &&
                     win == 0);

            if (win == 0)
            {
                do
                {

                    attackCase = getIAattackCase(boatsPlayer, IAlevel);

                    IANbStrokes++;

                    if (attackCase != 0)
                    {
                        scoreIA++;

                        for (int i = 0; i < 5; ++i)
                        {
                            if (boatsPlayer[i].isSunk == 0 &&
                                isPlayerBoatSunk(boatsPlayer[i].id,
                                                 boatsPlayer[i].lenght) == 1)
                            {
                                boatsPlayer[i].isSunk = 1;
                            }
                        }
                    }

                    win = isWin(boatsPlayer, boatsIA);

                } while (attackCase != 0 && IANbStrokes < maxStrokes &&
                         win == 0);
            }

            if (win != 0)
            {
                displayWin(&*renderer, &cursor, &*mainContinue, &menuContinue,
                           mainTimer, scoreIA * 100, scorePlayer * 100);
                if (!*mainContinue || !menuContinue)
                    return;
            }
        }
    }
}

int main(int argc, char **argv)
{
    Mix_Music *music = NULL;
    SDL_Renderer *renderer = NULL;
    int mainContinue = 1;

    srand(time(NULL));

    SDL_start(&renderer, music);

    while (mainContinue)
    {
        game(&renderer, &mainContinue);
    }

    SDL_stop(&renderer, &music);

    return 0;
}
