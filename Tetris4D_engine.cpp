/*

 Project: Tetris4D
 Author: Simon, Laszlo 

 Short Description:
 This file is the game engine of the Tetris 4D.
 Game space and element storage and handling.

 Change Log:
 1.1 - Initial revision - 2008 jan 05

 ToDo: 
 
 */
 /*
HE[h]: 10
*/
 
// interface

#include <stdlib.h>

#include "Tetris4D_engine.h"

// TYPE DEFINITIONS

// number of type of solids
#define SOLIDTYPES (6)

// number of difficulty levels
#define DIFFLEVELS (3)

// CONSTANTS

// defined solids
static TSolid solids[SOLIDTYPES] =
/* 1 cube */  {0x0001, 
/* 2 cube */   0x0003,
/* 3 cube L */ 0x0007,
/* 4 cube */   0x000f, 
/* 4 cube */   0x0027,
/* 4 cube */   0x0017};

// probabilities of solids in different
// difficulty levels (easy, medium, hard)
static int probs[DIFFLEVELS][SOLIDTYPES] =
{{1, 1, 1, 0, 0, 0},
 {1, 2, 2, 0, 0, 0},
 {1, 2, 2, 1, 1, 1}};

// GLOBAL VARIABLES

// the game space
TLevel space[SPACELENGTH];
// actual solid
TSolid solid;
// position of actual solid
// 0 if reached the floor
char pos;
// score collected in the actual game
int score;

t_game_opts game_opts;

// GLOBAL FUNCTIONS

// initialize the game variables
extern void initGame(void);

// lower the solid with one level
// result indicates that it invalid (end of game)
extern bool lowerSolid(void);

// LOCAL FUNCTIONS

// get a new random solid
extern void NewSolid(void);

// check overlap between solid and gamespace
extern bool checkOverlap(void);

// deletes the full levels
extern void killFullLevels(void);

// turns the solid from axis1 to axis 2
// returns indicator of turn availability
extern bool turn(char ax1, char ax2);

// implementation


// initialize the game variables
void initGame(t_game_opts in_game_opts)
{
   // for the every part of the space
   for (char t = 0; t < SPACELENGTH; t++)
      space[t] = 0x00;

   // get new solid
   NewSolid();
   
   // set options
   game_opts = in_game_opts;
   
   // init score value
   score = 0;
}

// get a new random solid
void NewSolid()
{
   // probability
   int prob; 
   // sum of probabilities
   int sum = 0;  

   // for every solid type
   for (int i = 0; i < SOLIDTYPES; i++)
   {
      // summarize the probability
      sum += probs[game_opts.diff][i];
   }
   
   int i = 0; 
   // get a random probability
   prob = (long int) rand() * sum / RAND_MAX;
   
   int prb = probs[game_opts.diff][i];
   
   while (!(prob < prb))
   { 
     i++;
     prb += probs[game_opts.diff][i];
   }

   solid = solids[i];

   // position the new solid to the
   // top 2 level of the space
   pos = SPACELENGTH - 2;
}

// check overlap between solid and gamespace
bool checkOverlap(void)
{
   return !!( ( (TSolid)space[pos+1] << 8 | space[pos]) & solid); 

}//end of checkOverlap

// deletes the full levels
void killFullLevels(void)
{
   // for every level
   for(int t = 0; t < SPACELENGTH; t++)
   {
      // if full level found
      if (space[t] == 0xff) 
      {
         // step down every higher level
         for (int tn = t+1; tn <= SPACELENGTH; tn++)
         {
            // get the next level or 0 on the top level
            space[tn-1] = space[tn] * 
                         (tn < SPACELENGTH);

         } // end of every level

      } // end of if full

   } // end of every level

} //end of checkFullLevels

// lower the solid with one level
// result> false if invalid (end of game)
bool lowerSolid(void)
{
   // solid reached the floor flag
   bool onFloor = (pos <= 0) || 
                  ( (pos-- || 1) &&
                    checkOverlap() && 
                    (pos++ || 1) );

   // if reached the floor,
   if (onFloor)
   {
      // put the solid to the space
      space[pos]   |= solid;
      space[pos+1] |= solid >> 8;
               
      // delete the full levels
      killFullLevels();
      // get new solid
      NewSolid();

      // check new solid already overlapped
      return (!checkOverlap());
   }
}

// turns the solid from axis 1 to axis 2
// returns indicator of turn availability
bool turn(char ax1, char ax2)
{
   // loop cnt, temp var
   int n, n2, i, tmp;
   // point
   int p[4];

   // remove the solid to a temp solid
   TSolid tempSolid = solid;
   solid = 0x0000;

 // turn it

   // for every cell
   for (n = 0; n < 16; n++)
   {
     // the bits of the number of the element are 
     // the coords of the element - so the num = pos vector
     
     // turn the vector (replace the two bit specified by axis num,
     //    and negate the second)
     n2 = n & (~(0x01 << ax2)) 
            & (~(0x01 << ax1)) 
            | (   ((n >> ax1) & 0x01) << ax2) 
            | ( (!((n >> ax2) & 0x01)) << ax1);

      // get the element to the new turned place
      solid |= ((tempSolid >> n) & 0x01) << n2;
   }

   // put to the lower level of the solid
   // if lower level empty then
   if (solid && 0x00ff == 0x00)
      // move from the higher level to the lower
      solid = solid >> 8;

   // if overlapped, invalid turn
   // get back the original
   return !(checkOverlap() && (solid = tempSolid)); 
 
}


