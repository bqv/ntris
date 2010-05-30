/**
 * \file 4dt_eng.h
 * \brief Header of the 4D-TRIS game engine
 */

#ifndef _4DT_ENG_H

#define _4DT_ENG_H

/*------------------------------------------------------------------------------
   INCLUDES
------------------------------------------------------------------------------*/

#include "4dt_m4d.h"

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/ 

/** length of the game space (num of levels) */
#define SPACELENGTH (12)

/** number of difficulty levels */
#define DIFFLEVELS (3)

/** Size of gamespace */
#define XSIZE 2
#define YSIZE 2
#define ZSIZE 2
#define WSIZE 2

#define MAXBLOCKNUM 4

/*------------------------------------------------------------------------------
   TYPE DEFINITIONS
------------------------------------------------------------------------------*/

/** one 3D level of the game space x, y, z */
typedef char tEngLevel[XSIZE][YSIZE][ZSIZE];

/** 2x2x2x2 supercube / container of a Solid */
typedef struct
{
  tEngLevel c[WSIZE];
} tEngSolid;

/** Container of block array */
typedef struct
{
  int        num;            /**< Number of blocks */
  tM4dVector c[MAXBLOCKNUM]; /** Array of block center position vectors */
} tEngBlocks;

/** Object container struct */
typedef struct
{
  tM4dVector pos;    /**< actual position of the object */
  tM4dMatrix axices; /**< Vectors of the 4 axices
                          builds the object's own coord.sys */
  tEngBlocks block;  /**< Vectors of each blocks center  in the
                          objects's own coordinate system */
} tEngObject;

/** game options */
typedef struct 
{
  /** difficulty level [0..2] */
  int diff;
}
tEngGameOptions;


/** sturct of the game variables */
typedef struct 
{
  /** the game space  */
  tEngLevel space[SPACELENGTH];
  /** actual object */
  tEngObject object;
  /** score collected in the actual game */
  int score;
  /** flag for indicate game over */
  int gameOver;
  /** counter for used objects */
  int solidnum;
  /** engine locked while animation running */
  int lock;

  /** animation related variables*/
  struct
  {
    int enable;   /** animation switch */
    int num;  /** number of transformation have to be performed */
    tM4dMatrix transform; /** transformation to be performed. */
    double posDecrease;   /** position decreasion to be performed */
  } animation;

  /** struct of game options */
  tEngGameOptions game_opts;

} tEngGame;

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern tEngGame engGE;

extern void engResetGame(void);
extern void engInitGame(void);
extern int engLowerSolid(void);
extern int engTurn(char ax1, char ax2);
extern tEngGame engCopyGameEngine(tEngGame inGameEngine);
extern void engPrintSpace(void);
extern tEngSolid engObject2Solid(tEngObject object);
extern int engAnimation(void);

/** time step, while the solid steps one level down in msec; */
static inline int engGetTimestep(void)
{
  // calculate timestep depending on actual score
  return(10000/(4+engGE.score/10000));
}

/** get the cell of the level at x, y, z from
    the game space empty or full */
static inline int engGetSpaceCell(int w, int x, int y, int z)
{
  return(engGE.space[w][x][y][z]);
}

#endif
