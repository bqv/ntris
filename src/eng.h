/**
 * \file eng.h
 * \brief Header of the 4D-TRIS game engine
 */

#ifndef _ENG_H_

#define _ENG_H_

/*------------------------------------------------------------------------------
   INCLUDES
------------------------------------------------------------------------------*/

#include "m4d.h"

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/** length of the game space (num of levels) */
#define SPACELENGTH 20
/** Size of gamespace */
#define SPACESIZE 4

/** number of dimensions */
#define DIMENSIONS 7

/** number of difficulty levels */
#define DIFFLEVELS 3

/** Number of blocks in an object */
#define MAXBLOCKNUM 4

/*------------------------------------------------------------------------------
   TYPE DEFINITIONS
------------------------------------------------------------------------------*/

/** one 3D level of the game space x, y, z */
typedef char tEngLevel[SPACESIZE][SPACESIZE][SPACESIZE];

/** 2x2x2x2 supercube / container of a Solid */
typedef struct
{
    tEngLevel c[SPACELENGTH];
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

typedef struct sEngGame tEngGame;

typedef void (*tEngGameEvent)(tEngGame *pEngGame);

/** struct of the game variables */
struct sEngGame
{
    /** the game space  */
    tEngLevel space[SPACELENGTH];
    /** actual object */
    tEngObject object;
    /** game dimension */
    int dimensions;
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
    /** engine suspended while menu on (no lowering) */
    int suspended;
    /** levels of gamespace */
    int spaceLength;
    /** game space level sizes (x, y, z) */
    int size[6]; // Upper limit: 7D
    /** animation related variables */
    struct
    {
        int enable;             /**< animation switch */
        int num;                /**< number of transformation have to be performed */
        tM4dMatrix transform;   /**< transformation to be performed. */
        tM4dVector translation; /**< translation vector */
    } animation;

    /** id of drop down timer */
    int *fnID_dropdown;
    /** id of lowering timer */
    int *fnID_lower;

    /** struct of game options */
    tEngGameOptions game_opts;

    /** Event handler call back for game over */
    tEngGameEvent onGameOver;
};

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern void engResetGame(tEngGame *pEngGame);
extern void engInitGame(tEngGame *pEngGame, tEngGameEvent onGameOver);
extern int engLowerSolid(tEngGame *pEngGame);
extern void engDropSolid(tEngGame *pEngGame);
extern int engTurn(char ax1, char ax2, char sign1, char sign2, tEngGame *pEngGame);
extern int engMove(char axle, int direction, tEngGame *pEngGame);
extern void engPrintSpace(tEngGame *pEngGame);
extern int engGetSpaceCell(int w, int x, int y, int z, tEngGame *pEngGame);

#endif
