/**
    \file  4dt_conf.h
    \brief Header of the 4D-TRIS configuration file handling.
 */

#ifndef _4DT_CONF_H

#define _4DT_CONF_H

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern char *confUserFilename(char *name);
extern void confLoad(char *filename);
extern void confSave(char *filename);
extern void confSetVar(char *name, double value);
extern double confGetVar(char *name, int *exists);

#endif
