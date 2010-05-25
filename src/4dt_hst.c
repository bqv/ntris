/**
 *  \file 4dt_hst.c
 *  \brief High score table handling.
 *
 *  Todo: use dinamic memory allocation for strings.
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#include "4dt_hst.h"

/*------------------------------------------------------------------------------
   MACROES
------------------------------------------------------------------------------*/

/** Number of scores in high score table */
#define SCORENUM 7

/** Length of username string */
#define USRNAMELEN 64
/** Length of month string */
#define MONTHLEN 32
/** Length of filename string */
#define FILENAMELEN 32

// I hate to do this, but
/* creates char array initialiser from macro */
#define stringize(s)  #s
#define string(s) stringize(s)

/*------------------------------------------------------------------------------
   TYPEDEFS
------------------------------------------------------------------------------*/

/** Struct for high score items */
typedef struct
{
  int  score;
  char user[USRNAMELEN];
  int  year;
  char month[MONTHLEN];
  int  day;
  int  hour;
  int  minu;
}
tHstScore;

/*------------------------------------------------------------------------------
   CONSTANTS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   GLOBALS
------------------------------------------------------------------------------*/

/** Array of Scores */
static tHstScore hstScores[SCORENUM];

/** Name of high score table file */
static char hstScoreFile[FILENAMELEN];

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static void hstFReadScoreTab(tHstScore scores[], char *filename);
static void hstFWriteScoreTab(tHstScore scores[SCORENUM], char *filename);
static tHstScore hstCreateScore(int score);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

/** Print score table to string array. */
void hstPrintScoreTab(char ***textTab, int scoreNum)
{
  int n;
  char str[256];
  int  length;

  (*textTab) = malloc(scoreNum * sizeof(char*));

  for (n = 0; n < scoreNum; n++)
  {
    if (n < SCORENUM)
    {
      length = sprintf(str,
               "%6d - %-8s - %04d % 4s %2d, %2d:%02d",
               hstScores[n].score, hstScores[n].user,
               hstScores[n].year, hstScores[n].month, hstScores[n].day,
               hstScores[n].hour, hstScores[n].minu);
    }
    else
    {
      length = sprintf(str,  "");
    }
    // TODO: mem free missing
    (*textTab)[n] = malloc(length+1);
    strcpy((*textTab)[n], str);
  }
}

/** Add Score to high score table
 *  Sorts in the score to the sorted (decreasing) high score table. */
void hstAddScore(int score)
{
  int n;
  tHstScore temp;
  tHstScore line;

  line = hstCreateScore(score);

  for (n = 0; n < SCORENUM; n++)
  {
    if (line.score > hstScores[n].score)
    {
      temp = hstScores[n];
      hstScores[n] = line;
      line = temp;
    }
  }

  hstFWriteScoreTab(hstScores, hstScoreFile);
}

/** Initialise High Score table handler.
 *  Reads score table from file. If missing, then creates an empty one. */
void hstInit(void)
{
  FILE* fp = NULL;
  char *home = getenv("HOME");

  // concat score table file name
  strcpy(hstScoreFile, (home != NULL) ? home : ".");
  strcat(hstScoreFile, "/.4dtris-scores");

  fp = fopen(hstScoreFile,"r");

  // if file exists
  if (fp != NULL)
  {
    fclose(fp);
    // read score table file
    hstFReadScoreTab(hstScores, hstScoreFile);
  }
  else
  {
    fprintf(stderr, __FILE__ ":info: No %s found, it will be created.\n", hstScoreFile);
    // Create score table file
    hstFWriteScoreTab(hstScores, hstScoreFile);
  }
}

/** Read score table from (comma separated text) file.  */
static void hstFReadScoreTab(tHstScore scores[SCORENUM], char *filename)
{
  FILE* fp = NULL;
  int n = 0;

  fp = fopen(filename,"r");

  // if file exists
  if (fp != NULL)
  {
    // while line founds with score and not found enough read score
    while((fscanf(fp, "%d,%" string(USRNAMELEN)
                     "[^,],%d,%" string(MONTHLEN)
                     "[^,],%d,%d,%d\n",
                 &scores[n].score, scores[n].user,
                 &scores[n].year, scores[n].month, &scores[n].day,
                 &scores[n].hour, &scores[n].minu
                 ) == 7
          ) && (n < SCORENUM))
    {
      n++;
    }
    fclose(fp);
  }
  else
  {
    fprintf(stderr, __FILE__ ":warning: Can not read file %s\n", filename);
  }
}

/** Writes score table to (comma separated text) file */
static void hstFWriteScoreTab(tHstScore scores[SCORENUM], char *filename)
{
  FILE* fp = NULL;
  int n;

  fp = fopen(filename,"w");

  // if file exists
  if (fp != NULL)
  {
    // write out each score struct
    for (n = 0; n < SCORENUM; n++)
    {
     fprintf(fp, "%d,%s,%d,%s,%d,%d,%d\n",
             scores[n].score, scores[n].user,
             scores[n].year, scores[n].month, scores[n].day,
             scores[n].hour, scores[n].minu);
    }
    fclose(fp);
  }
  else
  {
    fprintf(stderr, __FILE__
      ":Warning: Can not write file %s, data won't be stored.\n", filename);
  }
}

/** Create score struct for the given score value.
 *  Adds actual user and timestamp. */
static tHstScore hstCreateScore(int score)
{
  time_t curtime;
  tHstScore res;
  struct tm *lctime;
  char * user = getenv("USER");

  // get actual local time
  curtime = time(NULL);
  lctime = localtime(&curtime);

  // add timestamp to score struct
  res.score = score;
  res.year = 1900 + (*lctime).tm_year;
  res.day = (*lctime).tm_mday;
  res.hour = (*lctime).tm_hour;
  res.minu = (*lctime).tm_min;
  strftime(res.month, MONTHLEN, "%B", lctime);

  // add actual user
  strncpy(res.user, (user != NULL) ? user : "Me", USRNAMELEN-1);
  res.user[USRNAMELEN-1] = '\0';

  return(res);
}
