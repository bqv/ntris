/**
 * \file  4dt_mou.c
 * \brief Module of mouse event handlers
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <math.h>


#include "4dt_m4d.h"
#include "4dt_g4d.h"
#include "4dt_eng.h"
#include "4dt_scn.h"
#include "4dt_menu.h"

#include "4dt_mou.h"

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

typedef enum
{
  eMouStateNone,
  eMouStateViewChange,
  eMouStateMenu
} tMouState;

/*------------------------------------------------------------------------------
   CONSTANTS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   GLOBAL VARIABLES
------------------------------------------------------------------------------*/

static int mouWinWidth  = 1280;
static int mouWinHeight = 800;
static int mouDowned = 0;
static int mouDownX  = 0;
static int mouDownY  = 0;
static int mouLastX  = 0;
static int mouLastY  = 0;

static tMouState mouState = eMouStateNone;

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/


void mouMove(int x, int y)
{
  switch (mouState)
  {
    case eMouStateViewChange:
    {
      g4dRotateViewport(eM4dAxisY, eM4dAxisZ, 5.0 * (1.0*y - mouLastY)/mouWinHeight);
      g4dRotateViewport(eM4dAxisX, eM4dAxisY, 5.0 * (1.0*x - mouLastX)/mouWinWidth);
      break;
    }
    case eMouStateMenu:
    {
      menuNavigateTo( ((1.0 * MENUMAXITEM * y) / mouWinHeight) - 0.5 );
      break;
    }
    case eMouStateNone:
    {
      if (menuIsActived()) { mouState = eMouStateMenu; }
      break;
    }
    default: break;
  }

  mouLastX = x;
  mouLastY = y;
}

void mouDown(int x, int y)
{
  mouDowned = 1;

  if (mouState == eMouStateNone)
  {
    mouState  = eMouStateViewChange;
  }

  mouLastX  = mouDownX = x;
  mouLastY  = mouDownY = y;
}

void mouUp(int x, int y)
{
  switch (mouState)
  {
    case eMouStateMenu:
    {
      menuNavigate(eMenuForward);
      break;
    }
    case eMouStateViewChange:
    case eMouStateNone:
    default: break;
  }

  mouDowned = 0;
  mouState = eMouStateNone;

  mouLastX = x;
  mouLastY = y;
}
