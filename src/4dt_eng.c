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
#include <math.h>

#include "4dt_m4d.h"
#include "4dt_eng.h"
#include "4dt_scn.h"

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/** number of type of objects */
#define OBJECTTYPES (6)

/*------------------------------------------------------------------------------
// CONSTANTS
------------------------------------------------------------------------------*/

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
// num 1.,3. x    y    z    w  2.,4.  x    y    z    w
  {1, { {{ 0.5, 0.5, 0.5,-0.5}}, {{ 0.0, 0.0, 0.0, 0.0}},
        {{ 0.0, 0.0, 0.0, 0.0}}, {{ 0.0, 0.0, 0.0, 0.0}} } }, //  .
  {2, { {{ 0.5, 0.5, 0.5,-0.5}}, {{ 0.5, 0.5,-0.5,-0.5}},
        {{ 0.0, 0.0, 0.0, 0.0}}, {{ 0.0, 0.0, 0.0, 0.0}} } }, //  :
  {3, { {{ 0.5, 0.5, 0.5,-0.5}}, {{ 0.5, 0.5,-0.5,-0.5}},
        {{ 0.5,-0.5, 0.5,-0.5}}, {{ 0.0, 0.0, 0.0, 0.0}} } }, //  :.
  {4, { {{ 0.5, 0.5, 0.5,-0.5}}, {{ 0.5, 0.5,-0.5,-0.5}},
        {{ 0.5,-0.5, 0.5,-0.5}}, {{ 0.5,-0.5,-0.5,-0.5}} } }, //  ::
  {4, { {{ 0.5, 0.5, 0.5,-0.5}}, {{ 0.5, 0.5,-0.5,-0.5}},
        {{ 0.5,-0.5, 0.5,-0.5}}, {{-0.5, 0.5,-0.5,-0.5}} } }, // ':.
  {4, { {{ 0.5, 0.5, 0.5,-0.5}}, {{ 0.5, 0.5,-0.5,-0.5}},
        {{ 0.5,-0.5, 0.5,-0.5}}, {{-0.5, 0.5, 0.5,-0.5}} } }, // ,:.
};

/** probabilities of solids in different
    difficulty levels (easy, medium, hard) */
static const int engProbs[DIFFLEVELS][OBJECTTYPES] =
{{10, 5, 5, 1, 1, 1},
 { 5, 3, 2, 1, 1, 1},
 { 2, 1, 1, 1, 1, 1}};
  
/** Score points for filling a level in different difficulty levels */
static const int engScoreStep[DIFFLEVELS] = { 100, 200, 400};

/*------------------------------------------------------------------------------
   GLOBAL VARIABLES
------------------------------------------------------------------------------*/

/** Game engine container */
tEngGame engGE;

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static void engNewSolid(void);
static int engOverlapping(void);
static void engKillFullLevels(void);
static int engEqLevel(tEngLevel level1, tEngLevel level2);
static void engCopyLevel(tEngLevel target, tEngLevel source);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

/** Performing the queued transformation, return flag indicates if more
 *  call needed (1), or queue empty (0). */
int engAnimation(void)
{
  if (engGE.animation.num > 0)
  {
    engGE.object.axices = m4dMultiplyMM(engGE.animation.transform,
                                        engGE.object.axices);

    engGE.object.pos.c[eM4dAxisW] -= engGE.animation.posDecrease;

    engGE.animation.num--;
  }

  if (engGE.animation.num <= 0)
  {
    engGE.lock = 0;
    return(0);
  }
  else
  {
    return(1);
  }
}

/** Render/convert an object to gamespace array */
tEngSolid engObject2Solid(tEngObject object)
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
void engPrintSpace(void)
{
  int w, x, y, z;

  tEngSolid solid = engObject2Solid(engGE.object);

  for(y = YSIZE-1; y >= 0; y--)
  {
    for(z = ZSIZE-1; z >= 0 ; z--)
    {
      printf((z == 1) ? " " : "");
      for(w = 0; w < SPACELENGTH; w++)
      {
        for(x = 0; x < XSIZE; x++)
        {
          printf(engGE.space[w][x][y][z] ||
                 (      (w - engGE.object.pos.c[eM4dAxisW] >= 0)
                     && (w - engGE.object.pos.c[eM4dAxisW] < WSIZE)
                     && solid.c[w - lround(engGE.object.pos.c[eM4dAxisW])]
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

/** duplicates the game engine */
tEngGame engCopyGameEngine(tEngGame inGameEngine)
{
  int w;
  tEngGame result; // duplicated game engine;
  
  // Copy the input game engine to the result one.
  result = inGameEngine;
  
  // Copy the gamespace to the result.
  for (w = 0; w < SPACELENGTH; w++)
  {
    engCopyLevel(result.space[w], inGameEngine.space[w]);
  }
  
  // return with the result gamespace
  return result;
}

/** Reset game variables */
void engResetGame(void)
{
  int w;

  // for the every part of the space
  for (w = 0; w < SPACELENGTH; w++)
  {
    engClearLevel(engGE.space[w]);
  }

  // initialise the number of solids dropped
  engGE.solidnum = 0;

  // get new solid
  engNewSolid();
   
  // init score value
  engGE.score = 0;

  engGE.gameOver = 0;

  engGE.animation.num = 0;
  engGE.lock = 0;
  engGE.animation.posDecrease = 0;
  engGE.animation.transform   = m4dUnitMatrix();

}


/** initialize the game variables */
void engInitGame(void)
{
  // initialize random generator
  srand(time(NULL));

  // set options
  engGE.game_opts.diff = 2;
  engGE.animation.enable = 1;
  engGE.stereoEnable = 0; //MOD
  engGE.activeUser = 0;

  // reset parameters
  engResetGame();
}

/** get a new random solid */
static void engNewSolid(void)
{
   // probability
   int prob; 
   // sum of probabilities
   int sum = 0;  
   // loop counter
   int i;

   // for every solid type
   for (i = 0; i < OBJECTTYPES; i++)
   {
      // summarize the probability
      sum += engProbs[engGE.game_opts.diff][i];
   }
   
   // initialize loop counter
   i = 0; 
   
   // get a random probability
   prob = (long int) (sum * (rand() / (RAND_MAX + 1.0) ));
   
   int prb = engProbs[engGE.game_opts.diff][i];
   
   while (!(prob < prb))
   { 
     i++;
     prb += engProbs[engGE.game_opts.diff][i];
   }

   engGE.object.axices = m4dUnitMatrix();

   engGE.object.block = engObjects[i];

   // position the new solid to the
   // top 2 level of the space
   engGE.object.pos = m4dNullVector();
   engGE.object.pos.c[eM4dAxisW] = SPACELENGTH - WSIZE;
   
   // increase the number of the solid
   engGE.solidnum++;
}

/** check overlap between solid and gamespace
 *  \return overlapping detected flag */
static int engOverlapping(void)
{
  int w, x, y, z, pos;
  int overlap = 0;

  tEngSolid solid = engObject2Solid(engGE.object);

  for(w = 0; w < WSIZE; w++)
  for(x = 0; x < XSIZE; x++)
  for(y = 0; y < YSIZE; y++)
  for(z = 0; z < ZSIZE; z++)
  {
    pos = engGE.object.pos.c[eM4dAxisW];

    overlap |= (((pos + w) >= 0) ? engGE.space[pos + w][x][y][z] : 1)
               && solid.c[w][x][y][z];
  }

  return(overlap);

}//end of checkOverlap

/** deletes the full levels */
static void engKillFullLevels(void)
{
  // loop counter
  int t, tn;

  // for every level
  for(t = 0; t < SPACELENGTH; t++)
  {
    // if full level found
    if (engEqLevel(engGE.space[t], engFullLevel))
    {
      // increase score if user plays
      if (engGE.activeUser)
      {
        engGE.score += engScoreStep[engGE.game_opts.diff];
      }

      // step down every higher level
      for (tn = t+1; tn < SPACELENGTH; tn++)
      {
        // get the next level
        engCopyLevel(engGE.space[tn-1], engGE.space[tn]);
      } // end of every level
     // 0 on the top level
     engClearLevel(engGE.space[SPACELENGTH-1]);

     // step back with the loop counter to get the same level checked again
     t--;
    } // end of if full

  } // end of every level

} //end of checkFullLevels

/** lower the solid with one level
   \return false if invalid (end of game) */
int engLowerSolid(void)
{
  int w, x, y, z;
  int onFloor = 0;

  if (!engGE.lock)
  {
    engGE.object.pos.c[eM4dAxisW]--;
    if (engOverlapping())
    {
      engGE.object.pos.c[eM4dAxisW]++;
      onFloor = 1;
    }
    else
    {
      if (engGE.animation.enable)
      {
        engGE.object.pos.c[eM4dAxisW]++;

        engGE.lock = 1;
        engGE.animation.num  = 2.0;
        engGE.animation.posDecrease  = 1.0 / 2.0;
        engGE.animation.transform   = m4dUnitMatrix();
      }
    }

    // if reached the floor,
    if (onFloor)
    {
      tEngSolid solid = engObject2Solid(engGE.object);

      // put the solid to the space
      for(w = 0; w < WSIZE; w++)
      for(x = 0; x < XSIZE; x++)
      for(y = 0; y < YSIZE; y++)
      for(z = 0; z < ZSIZE; z++)
      {
        engGE.space[lround(engGE.object.pos.c[eM4dAxisW])+w][x][y][z] |= solid.c[w][x][y][z];
      }

      // delete the full levels
      engKillFullLevels();
      // get new solid
      engNewSolid();

      // check new solid already overlapped
      engGE.gameOver = engOverlapping();
      return (engGE.gameOver);
    }
  }

  return(0);
}

/** turns the solid from axis 1 to axis 2
   \return indicator of turn availability */
int engTurn(char ax1, char ax2)
{
  tEngObject obj;
  int result;

  // store object
  obj = engGE.object;

  // turn it
  engGE.object.axices = m4dMultiplyMM(m4dRotMatrix(ax1, ax2, M_PI / 2.0),
                                      engGE.object.axices);

  // Calc and set turn axis in scene drawing from axices defines the turning
  // plane. Fortunately this simple empiric equation does the trick
  scnAxle = abs(3 - ax1 - ax2);

  // if overlapped, invalid turn
  // get back the original
  if (engOverlapping())
  {
    engGE.object = obj;
    result = 0;
  }
  else
  {
    result = 1;

    if (engGE.animation.enable)
    {
      engGE.object = obj;
      if (!engGE.lock)
      {
        engGE.lock = 1;
        engGE.animation.num  = 5;
        engGE.animation.posDecrease  = 0.0;
        engGE.animation.transform  = m4dRotMatrix(ax1, ax2, M_PI / 2.0 / 5.0);
      }
    }
  }

  return(result);
}

