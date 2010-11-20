/**
 * \file  4dt_ui.h
 * \brief Header for module of user interface event handlers
 */

#ifndef _4DT_UI_H_
#define _4DT_UI_H_

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

/** Enum for special keys */
enum
{
  UI_KEY_BACKSPACE = 8,
  UI_KEY_TAB       = 9,
  UI_KEY_ENTER     = 13,
  UI_KEY_ESC       = 27,
  UI_KEY_UP        = 0x0100,
  UI_KEY_LEFT      = 0x0200,
  UI_KEY_DOWN      = 0x0300,
  UI_KEY_RIGHT     = 0x0400,
  UI_KEY_PGUP      = 0x0500,
  UI_KEY_PGDOWN    = 0x0600,
  UI_KEY_HOME      = 0x0700,
  UI_KEY_END       = 0x0800,
  UI_KEY_DEL       = 0x0900,
  UI_KEY_INS       = 0x0A00,
  UI_KEY_F1        = 0x1100,
  UI_KEY_F2        = 0x1200,
  UI_KEY_F10       = 0x1A00
} tuiKeyCodes;


/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern void uiKeyPress(int key, tEngGame *pEngGame, tScnSet *pScnSet);

#endif /* 4DT_UI_H_ */
