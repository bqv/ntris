/**
 *  \file 4dt_conf.c
 *  \brief Configuration file handling
 *
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "4dt_conf.h"

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   GLOBAL VARIABLES
------------------------------------------------------------------------------*/

static char**  confKeys = NULL;
static double* confVals = NULL;
static int     confNum = 0;

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static void confAddVar(char *name, double value);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

/** returns the user config file name. parameter: application name */
char *confUserFilename(char *name)
{
  static char *filename;
  char *home = getenv("HOME");

  if (home == NULL) { home = "."; }

  filename = malloc(strlen(home) + 2 + strlen(name) + 1);

  strcpy(filename, home);
  strcat(filename, "/.");
  strcat(filename, name);

  return(filename);
}

void confLoad(char *filename)
{
  FILE *file = NULL;
  char name[256];
  char line[256];
  double value;

  file = fopen(filename, "r");

  if (file != NULL)
  {
    while(fgets(line, sizeof(line), file) != NULL)
    {
      if (sscanf(line, "%s = %lf", name, &value) == 2)
      {
        confSetVar((char *)name, value);
      }
    }
    fclose(file);
  }
}

double confGetVar(char *name, int *exists)
{
  int i;
  *exists = 0;

  for(i = 0; i < confNum; i++)
  {
    if (strcmp(confKeys[i], name) == 0)
    {
      *exists = 1;
      return(confVals[i]);
    }
  }
  return(0);
}

void confSetVar(char *name, double value)
{
  int i;
  int exists = 0;

  for(i = 0; i < confNum; i++)
  {
    if (strcmp(confKeys[i], name) == 0)
    {
      confVals[i] = value;
      exists = 1;
    }
  }

  if (!exists)
  {
    confAddVar(name, value);
  }
}

static void confAddVar(char *name, double value)
{
  char** tempConfKeys = malloc((confNum+1) * sizeof(char*));
  double* tempConfVals = malloc((confNum+1) * sizeof(double));

  if(confNum > 0)
  {
    memcpy(tempConfKeys, confKeys, confNum * sizeof(char*));
    memcpy(tempConfVals, confVals, confNum * sizeof(double));
    free(confKeys);
    free(confVals);
  }
  confNum++;

  confKeys = tempConfKeys;
  confVals = tempConfVals;
  confKeys[confNum-1] = malloc(strlen(name)+1);
  strcpy(confKeys[confNum-1], name);
  confVals[confNum-1] = value;
}

void confSave(char *filename)
{
  FILE *file;
  int i;

  file = fopen(filename, "w");

  if (file != NULL)
  {
    for(i = 0; i < confNum; i++)
    {
      fprintf(file, "%s = %lf\n",confKeys[i], confVals[i]);
    }
    fclose(file);
  }
  else
  {
    fprintf(stderr, "Error: can not save config file: %s\n", filename);
  }
}
