/**
 * \file 4dt_eng.h
 * \brief Header of the 4D-TRIS game engine
 */

#ifndef _4DT_ENG_H

#define _4DT_ENG_H

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/** boolean type */
#define bool unsigned char

/** one 3D level of the game space x, y, z */
#define TLevel unsigned char

/** 2x2x2x2 supercube / container of a Solid */
#define TSolid unsigned short int


/** length of the game space (num of levels) */
#define SPACELENGTH (12)

/** get the cell of the level at x, y, z from
    the game space empty or full */
#define getSpaceCell(l,x,y,z) \
  ((ge.space[l]) & (0x01 << ((x)+(y)*2+(z)*4)))

/** get the cell of the level at x, y, z from
    the game space empty or full */
#define getSolidCell(l,x,y,z) \
  ((ge.solid) & (0x01 << ((x)+(y)*2+(z)*4+(l)*8)))

/*------------------------------------------------------------------------------
   TYPE DEFINITIONS
------------------------------------------------------------------------------*/

/** game options */
typedef struct
{
      /** difficulty level [0..2] */
      char diff;

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
  char pos;
  /** indicator of  */
  bool isnewsolid;
  /** score collected in the actual game */
  int score;
  /** num of the solid */
  int solidnum;

  /** struct of game options */
  t_game_opts game_opts;

} t_game_Engine;

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern t_game_Engine ge;
extern void initGame(void);
extern bool lowerSolid(void);
extern bool turn(char ax1, char ax2);
extern t_game_Engine copyGameEngine(t_game_Engine in_game_Engine);

extern void engTrigger(float time);


#endif
