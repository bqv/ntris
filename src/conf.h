/**
    \file  4dt_conf.h
    \brief Header of the 4D-TRIS configuration file handling.
 */

#ifndef _CONF_H_

#define _CONF_H_

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern char *confUserFilename(char *name);
extern void confLoad(char *filename);
extern void confSave(char *filename);
extern void confSetVar(char *name, double value);
extern double confGetVar(char *name, int *exists);

#endif
