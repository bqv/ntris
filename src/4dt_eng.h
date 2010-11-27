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
  /** flag for indicate real user gameing (or autoplay) */
  int activeUser;
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

  /** id of drop down timer */
  int fnID_dropdown;

  /** struct of game options */
  tEngGameOptions game_opts;

} tEngGame;

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern void engResetGame(tEngGame *pEngGame);
extern void engInitGame(tEngGame *pEngGame);
extern int engLowerSolid(tEngGame *pEngGame);
extern void engDropSolid(tEngGame *pEngGame);
extern int engTurn(char ax1, char ax2, tEngGame *pEngGame);
extern void engPrintSpace(tEngGame *pEngGame);
extern int engGetSpaceCell(int w, int x, int y, int z, tEngGame *pEngGame);

#endif
