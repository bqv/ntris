/*

 Project: Tetris4D
 Author: Simon, Laszlo 

 Short Description:
 This file is the Computer gamer's logic.

 Change Log:
 1.1 - Initial revision - simonl - 2008 jan 08

 ToDo: 
 
 */

// inculde headers
#include <stdlib.h>
#include "Tetris4D_engine.h"

// Variables from game engine:

// the game space;
extern TLevel space[SPACELENGTH];
// actual solid;
extern TSolid solid;
// position of actual solid
// (0 if reached the floor);
extern char pos;
// score collected in the actual game.
extern int score;

extern bool isnewsolid;

// Array contains the CenterOf Gravity of each situation.
double cog[4 * 4 * 4 * 4];
// Array contains the number of turns for each situation.
int    turns[4 * 4 * 4 * 4];


// Finds the best situation from all turn variation
// (the most effective one with fewest turn).
int findBestSolution(void);

// Calculate Cog of the situation.
double ProcessSitu(void);

// Search the best situation.
int SearchBestSitu(void);

// Finds the best situation from all turn variation
// (the most effective one with fewest turn).
int findBestSolution(void)
{
  // Loop counters. 
  int i, x1, x2, x3, x4;
    
  // For each turn number variation: 
  for (x1 = 0; x1 < 4; x1++)
    for (x2 = 0; x2 < 4; x2++)
      for (x3 = 0; x3 < 4; x3++)
        for (x4 = 0; x4 < 4; x4++)    
  {
    // Turn x1 times around first axle.
    for (i = 0; i < x1; i++) 
    {
      turn(0, 1);
    }      
     
    // Turn x2 times around first axle.
    for (i = 0; i < x2; i++) 
    {
      turn(1, 2);
    }
    
    // Turn x3 times around first axle.
    for (i = 0; i < x3; i++) 
    {
      turn(2, 0);
    }
    
    // Turn x4 times around first axle.
    for (i = 0; i < x4; i++) 
    {
      turn(0, 3);
    }    
     
    // While not reached the floor    
    while (!isnewsolid)
    {
      // lower the solid. 
      lowerSolid();      
    }
      
    // Calculate Cog of the situation.  
    cog[x1*64 + x2*16 + x3*4 + x4] = ProcessSitu(); 
    // Calculate number of turns made.
    turns[x1*64 + x2*16 + x3*4 + x4] = x1 + x2 + x3 + x4;
  }   
    
  // Return with the best of situations.   
  return SearchBestSitu();    
  
}  // End of function

// Calculate Cog of the situation.
double ProcessSitu(void)
{
  // Loop counters for axises.   
  int x, y, z, l;
  // Center of gravity on l axis, sum of full cells.
  double cog, sum = 0.0;
  
  // For each cell of gamespace,
  for (l = 0; l < SPACELENGTH; l++)
    for (x = 0; x < 4; x++)
      for (y = 0; y < 4; y++)
        for (z = 0; z < 4; z++)    
  {  
    // if the cell is full,    
    if (getSpaceCell(l,x,y,z))
    {
      // inrease summ and
      sum++;
      // add the position to Cog. 
      cog += l;
    }
  }  
  // 'Normalise' CoG.
  cog /= sum;
  
  return cog;

}  // End of function

// Search the best situation.
int SearchBestSitu(void)
{ 
  // Local variables:
  int i,       // loop counter; 
      cntCog,  // counter for;
      cntTurn; // counter for turns;
  int bestSitusCog[4 * 4 * 4 * 4];  // best situation container array;
  int bestSitusTurn[4 * 4 * 4 * 4]; // best situation container array;
    
  // lowest value of Cog;
  double lowerCog = SPACELENGTH;
  // lowest value of turn numbers.
  double fewerTurn = 4 * 4 * 4 * 4;
  
  // Find the minimal CoG result.
  
  // For each situation
  for (i = 0; i < 4 * 4 * 4 * 4; i++)
  {   
    // If actual CoG lower then lowest,
    if (cog[i] < lowerCog)
    {
      // the actual is the lowest.
      lowerCog = cog[i];         
    }  
  }
  
  // No best situation found.
  cntCog = 0;
  
  // For each situation
  for (i = 0; i < 4 * 4 * 4 * 4; i++)
  {
    // If the situations cog is the best
    if (cog[i] == lowerCog)
    {
      // the situ. is one of the best situs.
      bestSitusCog[cntCog] = i;
      // Increase counter.
      cntCog++;         
    }  
  }

  // Find the one with minimal turning number 
  // from best situations.
   
  // For each best situation
  for (i = 0; i < cntCog; i++)
  {  
    // if actual situ. turns num. fewer then lowest,
    if (turns[bestSitusCog[i]] < fewerTurn)
    {
      // the actual situs's turn num is the fewest.
      fewerTurn = turns[bestSitusCog[i]];         
    }  
  }
  
  // No best situation found regarding turn numbers.
  cntTurn = 0;
   
  // For the best situations
  for (i = 0; i < cntCog; i++)
  {
    // if the turn number match with the best,
    if (turns[bestSitusCog[i]] == fewerTurn)
    {
      // the situation is best.
      bestSitusTurn[cntTurn] = i;
      // Increase counter.
      cntTurn++;          
    }  
  }

  // Select one from the best situations and return  
  return bestSitusTurn[rand()*cntTurn/RAND_MAX];      

}  // End of function
