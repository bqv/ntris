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
#include "4dt_m4d.h"
#include "4dt_hst.h"
#include "4dt_g4d.h"
#include "4dt_eng.h"
#include "4dt_scn.h"
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

/** Empty level */
static const tEngLevel engEmptyLevel = {{{0,0}, {0,0}}, {{0,0}, {0,0}}};

/** Full level */
static tEngLevel engFullLevel = {{{1,1}, {1,1}}, {{1,1}, {1,1}}};

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

static tEngSolid engObject2Solid(tEngObject object);
static int engEqLevel(tEngLevel level1, tEngLevel level2);
static void engCopyLevel(tEngLevel target, tEngLevel source);

static void engNewSolid(tEngGame *pEngGame);
static int engOverlapping(tEngGame *pEngGame);
static void engKillFullLevels(tEngGame *pEngGame);
static int engAnimation(int interval, tEngGame *pEngGame);
static int engTimer(int interval, tEngGame *pEngGame);
static int engGetTimestep(tEngGame *pEngGame);
static void engUpdateScore(int clearedLevels, tEngGame *pEngGame);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

/** Calculates scores for cleared levels */
static void engUpdateScore(int clearedLevels, tEngGame *pEngGame)
{
  int score = 100;
  int clearSpace = 1;
  int level;

  for(level = 0; level < SPACELENGTH; level++)
  {
    clearSpace &= engEqLevel(pEngGame->space[level], engEmptyLevel);
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

/** Timer function for Game engine. */
int engDropSolid(int interval, tEngGame *pEngGame)
{
  if (engLowerSolid(pEngGame))
  {
    return(engDropSolidTimeStep);
  }
  else
  {
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

    refresh();

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

    pEngGame->object.pos.c[eM4dAxisW] -= pEngGame->animation.posDecrease;

    pEngGame->animation.num--;
  }

  refresh();

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
static tEngSolid engObject2Solid(tEngObject object)
{
  int i;
  tEngSolid solid;
  tM4dVector vec;

  solid = engEmptySolid;

  for (i = 0; i < object.block.num; i++)
  {
    vec = m4dMultiplyMV(object.axices, object.block.c[i]);

    solid.c[(vec.c[eM4dAxisW] > 0) ? 1 : 0]
           [(vec.c[eM4dAxisX] > 0) ? 1 : 0]
           [(vec.c[eM4dAxisY] > 0) ? 1 : 0]
           [(vec.c[eM4dAxisZ] > 0) ? 1 : 0] = 1;
  }

  return solid;
}

/** Prints out the game space to std out. */
void engPrintSpace(tEngGame *pEngGame)
{
  int w, x, y, z;

  tEngSolid solid = engObject2Solid(pEngGame->object);

  for(y = YSIZE-1; y >= 0; y--)
  {
    for(z = ZSIZE-1; z >= 0 ; z--)
    {
      printf((z == 1) ? " " : "");
      for(w = 0; w < SPACELENGTH; w++)
      {
        for(x = 0; x < XSIZE; x++)
        {
          printf(pEngGame->space[w][x][y][z] ||
                 (      (w - pEngGame->object.pos.c[eM4dAxisW] >= 0)
                     && (w - pEngGame->object.pos.c[eM4dAxisW] < WSIZE)
                     && solid.c[w - lround(pEngGame->object.pos.c[eM4dAxisW])]
                               [x][y][z])
                 ? "X" : ".");
          printf("  ");
        }
        printf(" ");
      }
      printf("\n");
    }
  }
  printf("\n");
}

/** Compare two levels */
static int engEqLevel(tEngLevel level1, tEngLevel level2)
{
  int x, y, z;
  int equal = 1;

  for(x = 0; x < XSIZE; x++)
  for(y = 0; y < YSIZE; y++)
  for(z = 0; z < ZSIZE; z++)
  {
    if (level1[x][y][z] != level2[x][y][z]) { equal = 0; }
  }

  return(equal);
}

/** Copy level to another. */
static void engCopyLevel(tEngLevel target, tEngLevel source)
{
  int x, y, z;

  for(x = 0; x < XSIZE; x++)
  for(y = 0; y < YSIZE; y++)
  for(z = 0; z < ZSIZE; z++)
  {
    target[x][y][z] = source[x][y][z];
  }
}

/** Clears a levels */
static void engClearLevel(tEngLevel level)
{
  int x, y, z;

  for(x = 0; x < XSIZE; x++)
  for(y = 0; y < YSIZE; y++)
  for(z = 0; z < ZSIZE; z++)
  {
    level[x][y][z] = 0;
  }
}

/** Reset game variables */
void engResetGame(tEngGame *pEngGame)
{
  int w;

  /*  for the every part of the space */
  for (w = 0; w < SPACELENGTH; w++)
  {
    engClearLevel(pEngGame->space[w]);
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
  pEngGame->animation.posDecrease = 0;
  pEngGame->animation.transform   = m4dUnitMatrix();

  setTimerCallback(engGetTimestep(pEngGame), engTimer, pEngGame);
}


/** initialize the game variables */
void engInitGame(tEngGame *pEngGame)
{
  /*  initialize random generator */
  srand(time(NULL));

  /*  set options */
  pEngGame->game_opts.diff = 2;
  pEngGame->animation.enable = 1;
  pEngGame->activeUser = 0;

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
  pEngGame->object.pos = m4dNullVector();
  pEngGame->object.pos.c[eM4dAxisW] = SPACELENGTH - WSIZE;

  /*  increase the number of the solid */
  pEngGame->solidnum++;
}

/** check overlap between solid and gamespace
 *  \return overlapping detected flag */
static int engOverlapping(tEngGame *pEngGame)
{
  int w, x, y, z, pos;
  int overlap = 0;

  tEngSolid solid = engObject2Solid(pEngGame->object);

  for(w = 0; w < WSIZE; w++)
  for(x = 0; x < XSIZE; x++)
  for(y = 0; y < YSIZE; y++)
  for(z = 0; z < ZSIZE; z++)
  {
    pos = pEngGame->object.pos.c[eM4dAxisW];

    overlap |= (((pos + w) >= 0) ? pEngGame->space[pos + w][x][y][z] : 1)
               && solid.c[w][x][y][z];
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
  for(t = 0; t < SPACELENGTH; t++)
  {
    /*  if full level found */
    if (engEqLevel(pEngGame->space[t], engFullLevel))
    {
      /*  step down every higher level */
      for (tn = t+1; tn < SPACELENGTH; tn++)
      {
        /*  get the next level */
        engCopyLevel(pEngGame->space[tn-1], pEngGame->space[tn]);
      } /*  end of every level */
     /*  0 on the top level */
     engClearLevel(pEngGame->space[SPACELENGTH-1]);
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
    if (engOverlapping(pEngGame))
    {
      pEngGame->object.pos.c[eM4dAxisW]++;
      onFloor = 1;
    }
    else
    {
      if (pEngGame->animation.enable)
      {
        pEngGame->object.pos.c[eM4dAxisW]++;

        pEngGame->lock = 1;
        pEngGame->animation.num  = 2.0;
        pEngGame->animation.posDecrease  = 1.0 / 2.0;
        pEngGame->animation.transform   = m4dUnitMatrix();

        setTimerCallback(engAnimationTimeStep, engAnimation, pEngGame);
      }
    }

    /*  if reached the floor, */
    if (onFloor)
    {
      tEngSolid solid = engObject2Solid(pEngGame->object);

      /*  put the solid to the space */
      for(w = 0; w < WSIZE; w++)
      for(x = 0; x < XSIZE; x++)
      for(y = 0; y < YSIZE; y++)
      for(z = 0; z < ZSIZE; z++)
      {
        pEngGame->space[lround(pEngGame->object.pos.c[eM4dAxisW])+w][x][y][z] |= solid.c[w][x][y][z];
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
        pEngGame->animation.posDecrease  = 0.0;
        pEngGame->animation.transform  = m4dRotMatrix(ax1, ax2, M_PI / 2.0 / 5.0);

        setTimerCallback(engAnimationTimeStep, engAnimation, pEngGame);
      }
    }
  }

  return(result);
}

