/**
 * \file  4dt_menu.c
 * \brief Game menu handling and drawing module.
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>

#include "4dt_g3d.h"
#include "4dt_ai.h"
#include "4dt_eng.h"
#include "4dt_hst.h"

#include "4dt_menu.h"

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/** Maximal number of submenu items per menu item. */
#define MENUMAXITEM 10
/** Number of lines in some menu's multiline text */
#define TEXTLINENUM 8

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

/** String type for menu caption. */
typedef char* tMenuString;

/** Menu item descriptor struct */
typedef struct
{
  char        enabled;               /**< Flag for enable menu item */
  tMenuString caption;               /**< Caption of the menu item */
  void        (*function)(void);     /**< Callback celled when menu item
                                          activates */
  eMenuItem   parent;                /**< Parent menu item */
  eMenuItem   submenus[MENUMAXITEM]; /**< (last item must be 'eNull') */
} tMenuItem;

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static void menuNew(void);
static void menuSound(void);
static void menuMusic(void);
static void menuAuto(void);
static void menuLevel(void);
static void menuControls(void);
static void menuHelp(void);
static void menuScores(void);
static void menuAbout(void);
static void menuBack(void);
static void menuQuit(void);
static void menuAnimation(void);

/*------------------------------------------------------------------------------
   CONSTANTS
------------------------------------------------------------------------------*/

/** Menu item descriptor array. Must be same order then Menu item enums! */
static tMenuItem menuItems[eMenuItemNum] =
{
/* en, caption,        function,     parent,        submenus */
  {1, "",               NULL,         eMenuOFF,          {eMenuRoot, eMenuNull}},
  {1, "Main",           NULL,         eMenuOFF,          {eMenuNewGame, eMenuOptions, eMenuHelp, eMenuBackToGame, eMenuQuit, eMenuNull}},
  {1, "New Game",       menuNew,      eMenuRoot,         {eMenuNull} },
  {1, "Options",        NULL,         eMenuRoot,         {eMenuVideoOptions, eMenuAudioOptions, eMenuGameOptions, eMenuNull} },
  {1, "Video Options",  NULL,         eMenuOptions,      {eMenuAnimation, eMenuNull} },
  {1, "Animation - ON", menuAnimation,eMenuVideoOptions, {eMenuNull} },
  {0, "Audio Options",  NULL,         eMenuOptions,      {eMenuSound, eMenuMusic, eMenuNull} },
  {0, "Sound",          menuSound,    eMenuAudioOptions, {eMenuNull}  },
  {0, "Music",          menuMusic,    eMenuAudioOptions, {eMenuNull}  },
  {1, "Game Options",   NULL,         eMenuOptions,      {eMenuAutoPlayer, eMenuDiffLevel, eMenuControls, eMenuNull} },
  {1, "Auto Player - ON", menuAuto,   eMenuGameOptions,  {eMenuNull}  },
  {1, "Diff. Level - Hard", menuLevel, eMenuGameOptions, {eMenuNull}  },
  {0, "Controls",       menuControls, eMenuGameOptions,  {eMenuNull}  },
  {1, "Help",           NULL,         eMenuRoot,         {eMenuHelppage, eMenuHighScores, eMenuAbout, eMenuNull} },
  {1, "Help Page",      menuHelp,     eMenuHelp,         {eMenuNull}  },
  {1, "High Scores",    menuScores,   eMenuHelp,         {eMenuNull}  },
  {1, "About",          menuAbout,    eMenuHelp,         {eMenuNull}  },
  {0, "Back To Game",   menuBack,     eMenuRoot,         {eMenuNull}  },
  {1, "Quit",           menuQuit,     eMenuRoot,         {eMenuNull}  },
};

/** Text of help page */
tMenuString menuHelpText[TEXTLINENUM] =
{
  "Game Controls:",
  "  - arrow keys - rotate the view port",
  "  - z, x, c, v - turn the blocks around the 4 axis",
  "  - q          - exit",
  "  - 1, 2, 3    - difficulty levels",
  "  - a          - auto player on/off",
  "",
  ""
};

/** Text of help page */
tMenuString menuAboutText[TEXTLINENUM] =
{
  "4DTRIS",
  "Tetris like game in four dimension",
  "Copyright (C) 2008-2010 Simon, Laszlo",
  "<laszlo.simon@gmail.com>",
  "http://dtris.sourceforge.net/",
  "https://launchpad.net/4dtris",
  "",
  ""
};

/*------------------------------------------------------------------------------
   GLOBAL VARIABLES
------------------------------------------------------------------------------*/

static eMenuItem menuActItem = eMenuRoot; /**< Actual active menu item.           */
static int menuSelItems[eMenuItemNum];    /**< Last selected submenu in each menuitem */
static char **menuText = NULL;        /**< Actual menu text for display       */

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

/** Get function for query menu active state. */
int menuActived(void)
{
  return(menuActItem != eMenuOFF);
}

/** Counts sub menu items of a menu item. */
static int menuSubNum(eMenuItem item)
{
  int i = 0;

  while( (menuItems[item].submenus[i] != eMenuNull) && (i < MENUMAXITEM) )
  {
    i++;
  }

  return(i);
}

/** Set menu item active */
void menuGotoItem(eMenuItem menuItem)
{
  menuActItem = menuItem;
   if (NULL != menuItems[menuActItem].function)
   {
     (*menuItems[menuActItem].function)();
   }
}

/** Menu state machine. */
void menuNavigate(eMenuEvent event)
{
  int subMenuNum;    // number of submenus in actual menu item
  eMenuItem subItem; // actual selected menu item

  subMenuNum = menuSubNum(menuActItem);
  subItem = menuItems[menuActItem].submenus[menuSelItems[menuActItem]];

  switch(event)
  {
    case eMenuUp:
      // Go up in the submenu list
      if (subMenuNum != 0)
      {
        menuSelItems[menuActItem] = (menuSelItems[menuActItem] + subMenuNum - 1)
                                    % subMenuNum;
      }
      break;
    case eMenuDown:
      // Go down in the submenu list
      if (subMenuNum != 0)
      {
        menuSelItems[menuActItem] = (menuSelItems[menuActItem] + 1)
                                    % subMenuNum;
      }
      break;
    case eMenuForward:
      // Step in to a menu item
      if (1 == menuItems[subItem].enabled)
      {
        menuGotoItem(subItem);
      }
      break;
    case eMenuBack:
      // Step out from a menu item
      menuActItem = menuItems[menuActItem].parent;
      menuText = NULL;
      break;
    default:
      exit(1); // Control should not reach this point
  }
}

/** Menu drawing procedure */
void menuDraw(void)
{
  int i = 0;
  eMenuItem subItem;

  float linespace = 0.1; // portion of vert. display size between lines

  // colors for different menu item states
  float colorDisable[4] = {0.6, 0.6, 0.8, 0.2};
  float colorNormal[4]  = {0.2, 0.2, 0.4, 0.8};
  float colorSelect[4]  = {1.0, 0.2, 0.4, 0.8};

  // for each submenu in active menu item
  for (i = 0; i < menuSubNum(menuActItem); i++)
  {
    subItem = menuItems[menuActItem].submenus[i];

    // Render the menu caption.
    g3dRenderString(0.1, (1-linespace) - i * linespace,
                    (i == menuSelItems[menuActItem])
                    ? colorSelect
                    : (0 == menuItems[subItem].enabled)
                      ? colorDisable
                      : colorNormal,
                    menuItems[subItem].caption);
  }

  // If multiline text selected to display
  if (menuText != NULL)
  {
    // display the text
    g3dRenderText(0.1, 1-linespace, colorNormal,
                  menuText, TEXTLINENUM, linespace);
  }

  return;
}

/*------------------------------------------------------------------------------
   MENUITEM CALLBACK FUNCTIONS
------------------------------------------------------------------------------*/

static void menuNew(void)
{
  // reset game engine
  engResetGame();

  aiAutoGamerON = 0;

  menuItems[eMenuAutoPlayer].caption = "Auto Player - OFF";

  menuItems[eMenuBackToGame].enabled = 1;

  menuNavigate(eMenuBack);
  menuNavigate(eMenuBack);
}

static void menuSound(void)
{
  // Not implemented yet
}

static void menuMusic(void)
{
  // Not implemented yet
}

static void menuAuto(void)
{
  aiAutoGamerON = !aiAutoGamerON;

  menuItems[eMenuAutoPlayer].caption = (aiAutoGamerON)
                                   ? "Auto Player - ON"
                                   : "Auto Player - OFF";
  menuNavigate(eMenuBack);
}

static void menuLevel(void)
{
  const tMenuString captions[DIFFLEVELS] =
  {
    "Diff. Level - Easy",
    "Diff. Level - Medium",
    "Diff. Level - Hard"
  };

  engGE.game_opts.diff = (engGE.game_opts.diff+1) % DIFFLEVELS;

  menuItems[eMenuDiffLevel].caption = captions[engGE.game_opts.diff];

  menuNavigate(eMenuBack);
}

static void menuControls(void)
{
  // Not implemented yet
}

static void menuHelp(void)
{
  menuText = menuHelpText;
}

static void menuScores(void)
{
  hstAddScore(engGE.score);
  hstPrintScoreTab(&menuText, TEXTLINENUM);
}

static void menuAbout(void)
{
  menuText = menuAboutText;
}

static void menuBack(void)
{
  menuNavigate(eMenuBack);
  menuNavigate(eMenuBack);
}

static void menuQuit(void)
{
  exit(EXIT_SUCCESS);
}

static void menuAnimation(void)
{
  engGE.animation.enable = !engGE.animation.enable;

  menuItems[eMenuAnimation].caption = (engGE.animation.enable)
                                   ? "Animation - ON"
                                   : "Animation - OFF";
  menuNavigate(eMenuBack);
}
