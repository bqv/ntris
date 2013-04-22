/**
 *  \file ai.c
 *  \brief This file is the Computer gamer's logic.
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "timer.h"
#include "m3d.h"
#include "m4d.h"
#include "eng.h"
#include "ai.h"

/*------------------------------------------------------------------------------
   CONSTANTS
------------------------------------------------------------------------------*/

/** Axices belongs to the turns around 1..4 axis */
static const char aiTurnAxices[4][2] = {{0, 1},{1, 2},{2, 0},{0, 3}};

/** Time step for AI turning object */
static const int aiTimeStepTurn = 300;

/*------------------------------------------------------------------------------
   VARIABLES
------------------------------------------------------------------------------*/

/** flag for auto gamer */
static int aiAutoGamerON = 0;

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static int aiFindBestSolution(int neededTurns[4],
                              int neededMoves[4],
                              tEngGame *pEngGame);
static double aiProcessSitu(tEngGame *pEngGame);
static int aiSearchBestSitu(double CoG[],
                            int turns[],
                            int situNum);
static void aiDoStep(tEngGame *pEngGame);
static int aiTimer(int interval, tEngGame *pEngGame);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

/** Get function for auto player enabled flag*/
int aiIsActive(void)
{
    return(aiAutoGamerON);
}

void aiSetActive(int active, tEngGame *pEngGame)
{
    int timerMustStarted = (active && !aiAutoGamerON);

    aiAutoGamerON = active;

    if (timerMustStarted)
    {
        setTimerCallback(0, (tTimerCallback)aiTimer, pEngGame);
    }
}

/** Timer function for Autoplayer. */
static int aiTimer(int interval, tEngGame *pEngGame)
{
    interval = aiTimeStepTurn;

    if (aiAutoGamerON)
    {
        if (pEngGame->gameOver == 0)
        {
            aiDoStep(pEngGame);
            return(interval);
        }
        else
        {
            aiAutoGamerON = 0;
            return(0);
        }
    }
    else
    {
        return(0);
    }
}

/** Trigger the AI to make a turn. */
static void aiDoStep(tEngGame *pEngGame)
{
    /*  Local variables: */
    char stepMade = 0; /*  inditcator of turn already made; */
    int i;   /*  loop counters; */
    /* Array of the number of turns by axises needed to the best situation. */
    static int neededTurns[4];
    static int neededMoves[4];

    static int solidnum = -1;

    /*  If got a new solid, */
    if (solidnum != pEngGame->solidnum)
    {
        /*  find the best situation (num of turns) to drop it. */
        aiFindBestSolution(neededTurns, neededMoves, pEngGame);

        solidnum = pEngGame->solidnum;
    }
    else
    {
        /*  else */
        /*  For each axis, */
        for (i = 0; i < 4; i++)
        {
            /*  if turn needed around and */
            /*  not yet made any turn, then */
            if (   (neededTurns[i] > 0)
                    && (!stepMade)
                    && (!pEngGame->lock))
            {
                /*  turn around the axis, */
                engTurn(aiTurnAxices[i][0],
                        aiTurnAxices[i][1],
                        1, 1, pEngGame);
                /*  decrease the turns needed, and */
                neededTurns[i]--;
                /*  indicate it. */
                stepMade = 1;
            }
        }
        for (i = 0; i < 3; i++)
        {
            if (   (neededMoves[i] != 0)
                    && (!stepMade)
                    && (!pEngGame->lock))
            {
                int direction = neededMoves[i] > 0 ? 1 : -1;
                engMove(i, direction, pEngGame);
                neededMoves[i] += -direction;
                stepMade = 1;
            }
        }
        /*  If no turn made, so no more turn needed, then */
        if ((!stepMade) && (!pEngGame->lock))
        {
            /*  drop the solid. */
            engDropSolid(pEngGame);
        }
    }
}

/** Finds the best situation from all turn variation
 *  (the most effective one with fewest turn).
 *  \return id of optimal turn variation */
static int aiFindBestSolution(int neededTurns[4],
                              int neededMoves[4],
                              tEngGame *pEngGame)
{
    /*  Local variables: */
    int i, j, n;             /*  loop counter; */
    int x[7];                /*  loop counters; */
    int bestSitu;            /*  number of the best situation */
    tEngGame engGE;          /*  game engine copy; */
    int index, pos;
    /** Array contains the CenterOf Gravity of each situation. */
    double CoG[(SPACESIZE-1) * (SPACESIZE-1) * (SPACESIZE-1) * 4 * 4 * 4 * 4];
    /** Array contains the number of turns for each situation. */
    int turns[(SPACESIZE-1) * (SPACESIZE-1) * (SPACESIZE-1) * 4 * 4 * 4 * 4];

    /*  For each turn number variation: */
    for (x[6] = 0; x[6] < (pEngGame->size[0]-1); x[6]++)
        for (x[5] = 0; x[5] < (pEngGame->size[1]-1); x[5]++)
            for (x[4] = 0; x[4] < (pEngGame->size[2]-1); x[4]++)
                for (x[3] = 0; x[3] < 4; x[3]++)
                    for (x[1] = 0; x[1] < 4; x[1]++)
                        for (x[2] = 0; x[2] < 4; x[2]++)
                            for (x[0] = 0; x[0] < 4; x[0]++)
                            {
                                /*  Back up the actual situation. */
                                engGE = *pEngGame;
                                engGE.animation.enable = 0;
                                engGE.lock = 0;

                                for (j = 0; j < 4; j++)
                                {
                                    /*  Turn x[j] times around j.th axle. */
                                    for (i = 0; i < x[j]; i++)
                                    {
                                        engTurn(aiTurnAxices[j][0],
                                                aiTurnAxices[j][1],
                                                1, 1, &engGE);
                                    }
                                }


                                for (j = 0; j < 3; j++)
                                {
                                    pos = lround(engGE.object.pos.c[j] - 1);

                                    engMove(j, x[6-j] - pos, &engGE);
                                }

                                while (engLowerSolid(&engGE)) {};

                                /*  Number of the situation. */
                                n =   x[6]*256*(pEngGame->size[2]-1)*(pEngGame->size[1]-1)
                                      + x[5]*256*(pEngGame->size[1]-1)
                                      + x[4]*256 +
                                      + x[0]*64 + x[1]*16 + x[2]*4 + x[3];

                                /*  Calculate Cog of the situation. */
                                CoG[n] = aiProcessSitu(&engGE);

                                /*  Calculate number of turns made. */
                                /* \todo x4,x5,x6 wrong */
                                turns[n] = x[0] + x[1] + x[2] + x[3] + x[4] + x[5] + x[6];
                            }

    /*  Return with the best of situations. */
    bestSitu = aiSearchBestSitu(CoG, turns,
                                (pEngGame->size[0]-1) *
                                (pEngGame->size[1]-1) *
                                (pEngGame->size[2]-1) *
                                4 * 4 * 4 * 4);

    /*  Fill the array of the required steps. */
    engGE = *pEngGame;
    neededTurns[3] = bestSitu % 4;
    neededTurns[2] = bestSitu / 4 % 4;
    neededTurns[1] = bestSitu / 16 % 4;
    neededTurns[0] = bestSitu / 64 % 4;
    index = bestSitu / 256 % (pEngGame->size[2]-1);
    pos = lround(engGE.object.pos.c[2]) - 1;
    neededMoves[2] = index - pos;
    index = bestSitu / (256*(pEngGame->size[2]-1)) % (pEngGame->size[1]-1);
    pos = lround(engGE.object.pos.c[1]) - 1;
    neededMoves[1] = index - pos;
    index = bestSitu / (256*(pEngGame->size[2]-1)*(pEngGame->size[1]-1)) % (pEngGame->size[0]-1);
    pos = lround(engGE.object.pos.c[0]) - 1;
    neededMoves[0] = index - pos;
    return bestSitu;

}  /*  End of function */

/** Calculate Center of gravity of the game space with landed object.
 *  \return height (on 4th axis) of CoG */
static double aiProcessSitu(tEngGame *pEngGame)
{
    /*  Loop counters for axises. */
    int x, y, z, l;
    /*  Center of gravity on l axis, sum of full cells. */
    int cog, sum;

    cog = 0;
    sum = 0;

    /*  For each cell of gamespace, */
    for (l = 0; l < pEngGame->spaceLength; l++)
        for (x = 0; x < pEngGame->size[0]; x++)
            for (y = 0; y < pEngGame->size[1]; y++)
                for (z = 0; z < pEngGame->size[2]; z++)
                {
                    /*  if the cell is full, */
                    if (engGetSpaceCell(l,x,y,z, pEngGame))
                    {
                        /*  inrease sum and */
                        sum += 1;
                        /*  add the position to Cog. */
                        cog += l;
                    }
                }

    /*  'Normalise' CoG. */
    return ( (sum == 0) ? 0.0 : (double)cog / sum);
}  /*  End of function. */

/** Search the best situation.
 *  \return ID of optimal situation */
static int aiSearchBestSitu(double CoG[],
                            int turns[],
                            int situNum)
{
    /*  Local variables: */
    int i,       /*  loop counter; */
        cntCog,  /*  counter for; */
        cntTurn, /*  counter for turns; */
        bestSitu;/*  selected best situation */
    /*  best situation container array; */
    int *bestSitusCog  = malloc(situNum * sizeof(int));
    /*  best situation container array; */
    int *bestSitusTurn = malloc(situNum * sizeof(int));

    /*  lowest value of Cog; */
    double lowerCog = INT_MAX;
    /*  lowest value of turn numbers. */
    double fewerTurn = situNum;

    /*  Find the minimal CoG result. */

    /*  For each situation */
    for (i = 0; i < situNum; i++)
    {
        /*  If actual CoG lower then lowest, */
        if (CoG[i] < lowerCog)
        {
            /*  the actual is the lowest. */
            lowerCog = CoG[i];
        }
    }

    /*  No best situation found. */
    cntCog = 0;

    /*  For each situation */
    for (i = 0; i < situNum; i++)
    {
        /*  If the situations cog is the best */
        if (CoG[i] == lowerCog)
        {
            return i;

            /*  the situ. is one of the best situs. */
            bestSitusCog[cntCog] = i;
            /*  Increase counter. */
            cntCog++;
        }
    }


    /*  Find the one with minimal turning number */
    /*  from best situations. */

    /*  For each best situation */
    for (i = 0; i < cntCog; i++)
    {
        /*  if actual situ. turns num. fewer then lowest, */
        if (turns[bestSitusCog[i]] < fewerTurn)
        {
            /*  the actual situs's turn num is the fewest. */
            fewerTurn = turns[bestSitusCog[i]];
        }
    }

    /*  No best situation found regarding turn numbers. */
    cntTurn = 0;

    /*  For the best situations */
    for (i = 0; i < cntCog; i++)
    {
        /*  if the turn number match with the best, */
        if (turns[bestSitusCog[i]] == fewerTurn)
        {
            return i;
            /*  the situation is best. */
            bestSitusTurn[cntTurn] = i;
            /*  Increase counter. */
            cntTurn++;
        }
    }

    bestSitu = bestSitusTurn[rand() % cntTurn];

    free(bestSitusCog);
    free(bestSitusTurn);

    /*  Select one from the best situations and return */
    return(bestSitu);

}  /*  End of function. */
