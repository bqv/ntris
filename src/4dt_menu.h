/**
 * \file  4dt_menu.h
 * \brief Header of game menu handling and drawing module.
 */

/* ------------------------------------------------------------------------------
   Short Description:
------------------------------------------------------------------------------*/
 
#ifndef _4DT_MENU_H_
#define _4DT_MENU_H_

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

/** Enums for each menu item */
typedef enum
{
eMenuNull,

eMenuOFF,
  eMenuRoot,
    eMenuNewGame,
    eMenuOptions,
      eMenuVideoOptions,
        eMenuAnimation,
        eMenuDrawmode,
        eMenuGrid,
        eMenuStereo,
      eMenuAudioOptions,
        eMenuSound,
        eMenuMusic,
      eMenuGameOptions,
        eMenuAutoPlayer,
        eMenuDiffLevel,
        eMenuControls,
    eMenuHelp,
      eMenuHelppage,
      eMenuHighScores,
      eMenuAbout,
    eMenuBackToGame,
    eMenuQuit,
    eMenuGameOver,
    eMenuItemNum
}
eMenuItem;

/** Enums for menu controlling events*/
typedef enum
{
  eMenuUp,
  eMenuDown,
  eMenuForward,
  eMenuBack
}
eMenuEvent;

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern int  menuIsActived(void);
extern void menuDraw(void);
extern void menuNavigate(eMenuEvent event);
extern void menuGotoItem(eMenuItem menuItem);

#endif /* _4DT_MENU_H_ */
