/*

 Project: Tetris4D
 Author: Simon, Laszlo

 Short Description:
 Header of the Tetris 4D game engine

 Change Log:
 1.1 - Initial revision - simonl - 2008 jan 05

 ToDo:

 */

#ifndef TETRIS4D_ENGINE_H

#define TETRIS4D_ENGINE_H

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

// boolean type
#define bool unsigned char

// one 3D level of the game space x, y, z
#define TLevel unsigned char

// 2x2x2x2 supercube / container of a Solid
#define TSolid unsigned short int


// length of the game space (num of levels)
#define SPACELENGTH (12)

// get the cell of the level at x, y, z from
// the game space empty or full
#define getSpaceCell(l,x,y,z) \
  ((space[l]) & (0x01 << ((x)+(y)*2+(z)*4)))

// get the cell of the level at x, y, z from
// the game space empty or full
#define getSolidCell(l,x,y,z) \
  ((solid) & (0x01 << ((x)+(y)*2+(z)*4+(l)*8)))

/*------------------------------------------------------------------------------
   TYPE DEFINITIONS
------------------------------------------------------------------------------*/

// game options
typedef struct
{
      // difficulty level [0..2]
      char diff;

} t_game_opts;

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

// initialize the game variables
extern void initGame(t_game_opts in_game_opts);

// lower the solid with one level
// result indicates that it invalid (end of game)
extern bool lowerSolid(void);

// turns the solid from axis1 to axis 2
// returns indicator of turn availability
extern bool turn(char ax1, char ax2);



#endif
