/**
 * \file  4dt_eng.c
 * \brief This file is the game engine of the 4D Tetris.
 *
 *  Game space and element storage and handling.
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "4dt_main.h"
#include "4dt_m3d.h"
#include "4dt_m4d.h"
#include "4dt_hst.h"
#include "4dt_eng.h"
#include "4dt_scn.h"
#include "4dt_g3d.h"
#include "4dt_g4d.h"
#include "4dt_menu.h"
#include "4dt_ai.h"

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/** number of type of objects */
#define OBJECTTYPES (6)

/*------------------------------------------------------------------------------
  CONSTANTS
------------------------------------------------------------------------------*/

/** Time step for animation [msec] */
static const int engAnimationTimeStep = 15;
/** Time step for step downs when object dropped [msec] */
static const double engDropSolidTimeStep = 10;

/** Empty solid */
static const tEngSolid engEmptySolid =
{   {{{{0,0}, {0,0}}, {{0,0}, {0,0}}}, {{{0,0}, {0,0}}, {{0,0}, {0,0}}}}   };

/** defined solids */
static const tEngBlocks engObjects[OBJECTTYPES] =
{
/*  num 1.,3. x    y    z    w  2.,4.  x    y    z    w */
  {1, { {{ 0.5, 0.5, 0.5,-0.5}}, {{ 0.0, 0.0, 0.0, 0.0}},
        {{ 0.0, 0.0, 0.0, 0.0}}, {{ 0.0, 0.0, 0.0, 0.0}} } }, /*   . */
  {2, { {{ 0.5, 0.5, 0.5,-0.5}}, {{ 0.5, 0.5,-0.5,-0.5}},
        {{ 0.0, 0.0, 0.0, 0.0}}, {{ 0.0, 0.0, 0.0, 0.0}} } }, /*   : */
  {3, { {{ 0.5, 0.5, 0.5,-0.5}}, {{ 0.5, 0.5,-0.5,-0.5}},
        {{ 0.5,-0.5, 0.5,-0.5}}, {{ 0.0, 0.0, 0.0, 0.0}} } }, /*   :. */
  {4, { {{ 0.5, 0.5, 0.5,-0.5}}, {{ 0.5, 0.5,-0.5,-0.5}},
        {{ 0.5,-0.5, 0.5,-0.5}}, {{ 0.5,-0.5,-0.5,-0.5}} } }, /*   :: */
  {4, { {{ 0.5, 0.5, 0.5,-0.5}}, {{ 0.5, 0.5,-0.5,-0.5}},
        {{ 0.5,-0.5, 0.5,-0.5}}, {{-0.5, 0.5,-0.5,-0.5}} } }, /*  ':. */
  {4, { {{ 0.5, 0.5, 0.5,-0.5}}, {{ 0.5, 0.5,-0.5,-0.5}},
        {{ 0.5,-0.5, 0.5,-0.5}}, {{-0.5, 0.5, 0.5,-0.5}} } }, /*  ,:. */
};

/** probabilities of solids in different
    difficulty levels (easy, medium, hard) */
static const int engProbs[DIFFLEVELS][OBJECTTYPES] =
{{10, 5, 5, 1, 1, 1},
 { 5, 3, 2, 1, 1, 1},
 { 2, 1, 1, 1, 1, 1}};

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static tEngSolid engObject2Solid(tEngObject object, int *invalid, tEngGame *pEngGame);
static int engEqLevel(tEngLevel level1, int value, tEngGame *pEngGame);
static void engCopyLevel(tEngLevel target, tEngLevel source, tEngGame *pEngGame);
static void engClearLevel(tEngLevel level, tEngGame *pEngGame);
static void engNewSolid(tEngGame *pEngGame);
static int engOverlapping(tEngGame *pEngGame);
static void engKillFullLevels(tEngGame *pEngGame);
static int engAnimation(int interval, tEngGame *pEngGame);
static int engDropSolidTimer(int interval, tEngGame *pEngGame);
static int engTimer(int interval, tEngGame *pEngGame);
static int engGetTimestep(tEngGame *pEngGame);
static void engUpdateScore(int clearedLevels, tEngGame *pEngGame);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

/** get the cell of the level at x, y, z from
    the game space empty or full */
int engGetSpaceCell(int w, int x, int y, int z, tEngGame *pEngGame)
{
  return(pEngGame->space[w][x][y][z]);
}

/** Calculates scores for cleared levels */
static void engUpdateScore(int clearedLevels, tEngGame *pEngGame)
{
  int score = 100;
  int clearSpace = 1;
  int level;

  for(level = 0; level < pEngGame->spaceLength; level++)
  {
    clearSpace &= engEqLevel(pEngGame->space[level], 0, pEngGame);
  }

  if (clearSpace) { score *= 2; }

  score *= pow(2, pEngGame->game_opts.diff);

  score *= pow(clearedLevels, 2);

  /*  increase score if user plays */
  if (pEngGame->activeUser)
  {
    pEngGame->score += score;
  }
}


/** time step, while the solid steps one level down in msec; */
static int engGetTimestep(tEngGame *pEngGame)
{
  /*  calculate timestep depending on actual score */
  return(10000/(4+pEngGame->score/2000));
}

/** Drop object */
void engDropSolid(tEngGame *pEngGame)
{
  pEngGame->fnID_dropdown = setTimerCallback(1, engDropSolidTimer, pEngGame);
}

/** Timer for drop object */
static int engDropSolidTimer(int interval, tEngGame *pEngGame)
{
  if (engLowerSolid(pEngGame))
  {
    return(engDropSolidTimeStep);
  }
  else
  {
    pEngGame->fnID_dropdown = NULL;
    return(0);
  }
}

/** Timer function for Game engine. */
static int engTimer(int interval, tEngGame *pEngGame)
{
  if (pEngGame->gameOver == 0)
  {
    if (!menuIsActived())
    {
      engLowerSolid(pEngGame);
    }

    return(engGetTimestep(pEngGame));
  }
  else
  {
    return(0);
  }
}

/** Game over handling */
static void engGameOver(tEngGame *pEngGame)
{
  pEngGame->gameOver = 1;
  pEngGame->activeUser = 0;

  aiSetActive(0, pEngGame);

  hstAddScore(pEngGame->score);

  menuGotoItem(eMenuGameOver);
}

/** Performing the queued transformation, return flag indicates if more
 *  call needed (1), or queue empty (0). */
static int engAnimation(int interval, tEngGame *pEngGame)
{
  if (pEngGame->animation.num > 0)
  {
    pEngGame->object.axices = m4dMultiplyMM(pEngGame->animation.transform,
                                        pEngGame->object.axices);

    pEngGame->object.pos = m4dAddVectors(pEngGame->object.pos,
                                         pEngGame->animation.translation);

    pEngGame->animation.num--;
  }

  if (pEngGame->animation.num <= 0)
  {
    pEngGame->lock = 0;
    return(0);
  }
  else
  {
    return(interval);
  }
}

/** Render/convert an object to gamespace array */
static tEngSolid engObject2Solid(tEngObject object,
                                 int *invalid,
                                 tEngGame *pEngGame)
{
  int i, x, y, z, w;
  tEngSolid solid;
  tM4dVector vec;

  if (invalid != NULL) { *invalid = 0; }

  solid = engEmptySolid;

  for (i = 0; i < object.block.num; i++)
  {
    vec = m4dMultiplyMV(object.axices, object.block.c[i]);
    vec = m4dAddVectors(object.pos, vec);

    w = floorl(vec.c[eM4dAxisW]);
    x = floorl(vec.c[eM4dAxisX]);
    y = floorl(vec.c[eM4dAxisY]);
    z = floorl(vec.c[eM4dAxisZ]);

    if (    (x >= 0) && (x < pEngGame->size[0])
         && (y >= 0) && (y < pEngGame->size[1])
         && (z >= 0) && (z < pEngGame->size[2])
         && (w >= 0) && (w < pEngGame->spaceLength)
       )
    {
      solid.c[w][x][y][z] = 1;
    }
    else
    {
      if (invalid != NULL) { *invalid = 1; }
    }
  }

  return solid;
}

/** Check if a levels contains the same value at each cells */
static int engEqLevel(tEngLevel level,
                      int value,
                      tEngGame *pEngGame)
{
  int x, y, z;
  int equal = 1;

  for(x = 0; x < pEngGame->size[0]; x++)
  for(y = 0; y < pEngGame->size[1]; y++)
  for(z = 0; z < pEngGame->size[2]; z++)
  {
    if (level[x][y][z] != value) { equal = 0; }
  }

  return(equal);
}

/** Copy level to another. */
static void engCopyLevel(tEngLevel target,
                         tEngLevel source,
                         tEngGame *pEngGame)
{
  int x, y, z;

  for(x = 0; x < pEngGame->size[0]; x++)
  for(y = 0; y < pEngGame->size[1]; y++)
  for(z = 0; z < pEngGame->size[2]; z++)
  {
    target[x][y][z] = source[x][y][z];
  }
}

/** Clears a levels */
static void engClearLevel(tEngLevel level,
                          tEngGame *pEngGame)
{
  int x, y, z;

  for(x = 0; x < pEngGame->size[0]; x++)
  for(y = 0; y < pEngGame->size[1]; y++)
  for(z = 0; z < pEngGame->size[2]; z++)
  {
    level[x][y][z] = 0;
  }
}

/** Reset game variables */
void engResetGame(tEngGame *pEngGame)
{
  int w;

  /*  for the every part of the space */
  for (w = 0; w < pEngGame->spaceLength; w++)
  {
    engClearLevel(pEngGame->space[w], pEngGame);
  }

  /*  initialise the number of solids dropped */
  pEngGame->solidnum = 0;

  /*  get new solid */
  engNewSolid(pEngGame);

  /*  init score value */
  pEngGame->score = 0;
  pEngGame->gameOver = 0;

  pEngGame->animation.num = 0;
  pEngGame->lock = 0;

  pEngGame->animation.translation = m4dNullVector();
  pEngGame->animation.transform   = m4dUnitMatrix();

  setTimerCallback(engGetTimestep(pEngGame), engTimer, pEngGame);

  clearTimerCallback(pEngGame->fnID_dropdown);
}


/** initialize the game variables */
void engInitGame(tEngGame *pEngGame)
{
  /*  initialize random generator */
  srand(time(NULL));

  /*  set options */
  pEngGame->game_opts.diff   = 2;
  pEngGame->animation.enable = 1;
  pEngGame->activeUser       = 0;
  pEngGame->spaceLength      = 12;
  pEngGame->size[0]          = 2;
  pEngGame->size[1]          = 2;
  pEngGame->size[2]          = 2;
  pEngGame->fnID_dropdown    = NULL;

  /*  reset parameters */
  engResetGame(pEngGame);
}

/** get random object index based on difficulty level */
static int engRandSolidnum(tEngGame *pEngGame)
{
  /*  probability */
  int prob, prb;
  /*  sum of probabilities */
  int sum = 0;
  /*  loop counter / index of random object */
  int i;

  /*  for every solid type */
  for (i = 0; i < OBJECTTYPES; i++)
  {
     /*  summarize the probability */
     sum += engProbs[pEngGame->game_opts.diff][i];
  }

  /*  initialize loop counter */
  i = 0;

  /*  get a random probability */
  prob = (long int) (sum * (rand() / (RAND_MAX + 1.0) ));

  prb = engProbs[pEngGame->game_opts.diff][i];

  while (!(prob < prb))
  {
    i++;
    prb += engProbs[pEngGame->game_opts.diff][i];
  }

  return(i);
}


/** get a new random solid */
static void engNewSolid(tEngGame *pEngGame)
{
  int index = engRandSolidnum(pEngGame);

  pEngGame->object.axices = m4dRandUnitMatrix();

  pEngGame->object.block = engObjects[index];

  /*  position the new solid to the */
  /*  top 2 level of the space */
  pEngGame->object.pos = m4dVector(1.0 + rand() % (pEngGame->size[0]-1),
                                   1.0 + rand() % (pEngGame->size[1]-1),
                                   1.0 + rand() % (pEngGame->size[2]-1),
                                   pEngGame->spaceLength - 1.0);

  /*  increase the number of the solid */
  pEngGame->solidnum++;
}

/** check overlap between solid and gamespace
 *  \return overlapping detected flag */
static int engOverlapping(tEngGame *pEngGame)
{
  int w, x, y, z;
  int overlap = 0;

  tEngSolid solid = engObject2Solid(pEngGame->object, &overlap, pEngGame);

  for(w = 0; w < pEngGame->spaceLength; w++)
    for(x = 0; x < pEngGame->size[0]; x++)
      for(y = 0; y < pEngGame->size[1]; y++)
        for(z = 0; z < pEngGame->size[2]; z++)
  {
    if (solid.c[w][x][y][z])
    {
      overlap |=  pEngGame->space[w][x][y][z];
    };
  }

  return(overlap);

}/* end of checkOverlap */

/** deletes the full levels */
static void engKillFullLevels(tEngGame *pEngGame)
{
  /*  loop counter */
  int t, tn;
  int clearedLevels = 0;

  /*  for every level */
  for(t = 0; t < pEngGame->spaceLength; t++)
  {
    /*  if full level found */
    if (engEqLevel(pEngGame->space[t], 1, pEngGame))
    {
      /*  step down every higher level */
      for (tn = t+1; tn < pEngGame->spaceLength; tn++)
      {
        /*  get the next level */
        engCopyLevel(pEngGame->space[tn-1], pEngGame->space[tn], pEngGame);
      } /*  end of every level */
     /*  0 on the top level */
     engClearLevel(pEngGame->space[pEngGame->spaceLength-1], pEngGame);
     clearedLevels++;

     /*  step back with the loop counter to get the same level checked again */
     t--;
    }
  } /*  end of every level */

  engUpdateScore(clearedLevels, pEngGame);
} /* end of checkFullLevels */

/** lower the solid with one level
   \return false if invalid (end of game) */
int engLowerSolid(tEngGame *pEngGame)
{
  int w, x, y, z;
  int onFloor = 0;

  if (!pEngGame->lock)
  {
    pEngGame->object.pos.c[eM4dAxisW]--;

    onFloor = engOverlapping(pEngGame);

    if (onFloor)
    {
      pEngGame->object.pos.c[eM4dAxisW]++;
    }
    else
    {
      if (pEngGame->animation.enable)
      {
        pEngGame->object.pos.c[eM4dAxisW]++;

        pEngGame->lock = 1;
        pEngGame->animation.num = 2;
        pEngGame->animation.translation = m4dVector(0.0,0.0,0.0,
                                                -1.0 / pEngGame->animation.num);
        pEngGame->animation.transform   = m4dUnitMatrix();

        setTimerCallback(engAnimationTimeStep, engAnimation, pEngGame);
      }
    }

    /*  if reached the floor, */
    if (onFloor)
    {
      tEngSolid solid = engObject2Solid(pEngGame->object, NULL, pEngGame);

      /*  put the solid to the space */
      for(w = 0; w < pEngGame->spaceLength; w++)
        for(x = 0; x < pEngGame->size[0]; x++)
          for(y = 0; y < pEngGame->size[1]; y++)
            for(z = 0; z < pEngGame->size[2]; z++)
      {
        pEngGame->space[w][x][y][z] |= solid.c[w][x][y][z];
      }

      /*  delete the full levels */
      engKillFullLevels(pEngGame);
      /*  get new solid */
      engNewSolid(pEngGame);

      /*  check new solid already overlapped */
      if (engOverlapping(pEngGame))
      {
        engGameOver(pEngGame);
      }

      return (pEngGame->gameOver);
    }
  }
  else
  {
    /** \todo should be lowered also in case of locked*/
  }

  return(!onFloor);
}

/** turns the solid from axis 1 to axis 2
   \return indicator of turn availability */
int engTurn(char ax1, char ax2, tEngGame *pEngGame)
{
  tEngObject obj;
  int result;

  /*  store object */
  obj = pEngGame->object;

  /*  turn it */
  pEngGame->object.axices = m4dMultiplyMM(m4dRotMatrix(ax1, ax2, M_PI / 2.0),
                                      pEngGame->object.axices);

  /*  if overlapped, invalid turn */
  /*  get back the original */
  if (engOverlapping(pEngGame))
  {
    pEngGame->object = obj;
    result = 0;
  }
  else
  {
    result = 1;

    if (pEngGame->animation.enable)
    {
      pEngGame->object = obj;
      if (!pEngGame->lock)
      {
        pEngGame->lock = 1;
        pEngGame->animation.num  = 5;
        pEngGame->animation.translation = m4dNullVector();
        pEngGame->animation.transform   = m4dRotMatrix(ax1, ax2, M_PI / 2.0
                                                     / pEngGame->animation.num);

        setTimerCallback(engAnimationTimeStep, engAnimation, pEngGame);
      }
    }
  }

  return(result);
}

/** moves the solid on 'axle' to the 'direction=[-1,+1]' */
int engMove(char axle, int direction, tEngGame *pEngGame)
{
  tEngObject objStored  = pEngGame->object;
  tM4dVector moveVector = m4dMultiplySV(direction, m4dUnitVector(axle));
  int valid;

  pEngGame->object.pos = m4dAddVectors(pEngGame->object.pos, moveVector);

  valid = !engOverlapping(pEngGame);

  if (valid)
  {
    if (pEngGame->animation.enable)
    {
      pEngGame->object = objStored;

      if (!pEngGame->lock)
      {
        pEngGame->lock = 1;
        pEngGame->animation.num  = 3;
        pEngGame->animation.translation = m4dMultiplySV(1.0 / pEngGame->animation.num,
                                                        moveVector);
        pEngGame->animation.transform   = m4dUnitMatrix();

        setTimerCallback(engAnimationTimeStep, engAnimation, pEngGame);
      }
    }
  }
  else
  {
    pEngGame->object = objStored;
  }

  return(valid);
}


/** Prints out the game space to std out. */
void engPrintSpace(tEngGame *pEngGame)
{
  int w, x, y, z;

  tEngSolid solid = engObject2Solid(pEngGame->object, NULL, pEngGame);

  for(y = pEngGame->size[1]-1; y >= 0; y--)
  {
    for(z = pEngGame->size[2]-1; z >= 0 ; z--)
    {
      printf((z == 1) ? " " : "");
      for(w = 0; w < pEngGame->spaceLength; w++)
      {
        for(x = 0; x < pEngGame->size[0]; x++)
        {
          printf(pEngGame->space[w][x][y][z] ? "X" :
                 solid.c[w][x][y][z]         ? "#" :
                                               ".");
          printf("  ");
        }
        printf(" ");
      }
      printf("\n");
    }
  }
  printf("\n");
}
