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

extern int  menuActive(void);
extern void menuDraw(void);
extern void menuNavigate(eMenuEvent event);

#endif /* _4DT_MENU_H_ */
