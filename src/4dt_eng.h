/**
 * \file 4dt_eng.h
 * \brief Header of the 4D-TRIS game engine
 */

#ifndef _4DT_ENG_H

#define _4DT_ENG_H

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/ 

/** length of the game space (num of levels) */
#define SPACELENGTH (12)

/** number of difficulty levels */
#define DIFFLEVELS (3)

/*------------------------------------------------------------------------------
   TYPE DEFINITIONS
------------------------------------------------------------------------------*/

/** boolean type */
typedef unsigned char bool;

/** one 3D level of the game space x, y, z */
typedef unsigned char TLevel;

/** 2x2x2x2 supercube / container of a Solid */
typedef unsigned short int TSolid;

/** game options */
typedef struct 
{
      /** difficulty level [0..2] */
      int diff;

} t_game_opts;


/** sturct of the game variables */
typedef struct 
{
  /** the game space  */
  TLevel space[SPACELENGTH];
  /** actual solid */
  TSolid solid;
  /** position of actual solid
      0 if reached the floor */
  int pos;
  /** indicator of  */
  bool isnewsolid;
  /** score collected in the actual game */
  int score;
  /** flag for indicate game over */
  int gameOver;
  /** num of the solid */
  int solidnum;

  /** struct of game options */
  t_game_opts game_opts;

} t_game_Engine;

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern t_game_Engine engGE;

/** get the cell of the level at x, y, z from
    the game space empty or full */
static inline int engGetSpaceCell(int l, int x, int y, int z)
{
  return(engGE.space[l] & (0x01 << (x + y*2 + z*4)));
}

/** get the cell of the level at x, y, z from
    the game space empty or full */
static inline int engGetSolidCell(int l, int x, int y, int z)
{
  return (engGE.solid & (0x01 << (x + y*2 + z*4 + l*8)));
}

extern void engResetGame(void);
extern void engInitGame(void);
extern bool engLowerSolid(void);
extern bool engTurn(char ax1, char ax2);
extern t_game_Engine engCopyGameEngine(t_game_Engine in_game_Engine);

extern void engTrigger(float time);


#endif
