/**
 * \file  mou.c
 * \brief Module of mouse event handlers
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "m3d.h"
#include "m4d.h"
#include "g3d.h"
#include "g4d.h"
#include "eng.h"
#include "scn.h"
#include "menu.h"

#include "mou.h"

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
    eMouStateObjRotate,
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
static tM3dVector downedV;
static tMouState mouState = eMouStateNone;
int mouLastX  = 0;
int mouLastY  = 0;

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

void mouResize(int width, int height)
{
    mouWinWidth = width;
    mouWinHeight = height;
}


void mouMove(int x, int y, tEngGame *pEngGame)
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
        if (menuIsActived())
        {
            mouState = eMouStateMenu;
        }
        break;
    }
    case eMouStateObjRotate:
    {
        /*    extern tM3dVector cursor;
              int ax1 = m3dOrtho(downedV);
              int ax2 = m3dOrtho(cursor);

              if (ax1 != ax2)
              {
                engTurn(ax1, ax2,
                        downedV.c[ax1]>=0 ? 1 : -1,
                        cursor.c[ax2]>=0  ? 1 : -1, pEngGame);
                downedV = cursor;
              }
        */
        break;
    }
    default:
        break;
    }
    mouLastX = x;
    mouLastY = y;
}

void mouDown(int x, int y)
{
    mouDowned = 1;

    if (mouState == eMouStateNone)
    {
        mouState  = eMouStateObjRotate;
        /*mouState  = eMouStateViewChange;*/
    }

    mouLastX  = mouDownX = x;
    mouLastY  = mouDownY = y;

    /*  downedV = cursor;*/
}

void mouUp(int x, int y, tEngGame *pEngGame)
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
    default:
    {
        break;
    }
    }

    mouDowned = 0;
    mouState = eMouStateNone;

    mouLastX = x;
    mouLastY = y;
}
