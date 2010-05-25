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
} tuiKeyCodes;


/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern void uiKeyPress(int key);

#endif /* 4DT_UI_H_ */
