/* ------------------------------------------------------------------------------
   Short Description:

   ToDo:

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>

#include "4dt_g3d.h"

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   CONSTANTS
------------------------------------------------------------------------------*/

/** Constant light parameters */
static const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
static const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
static const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

/** Constant material parameters */
static const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
static const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
static const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
static const GLfloat high_shininess[] = { 100.0f };

/** Background color. */
static const GLfloat bg_color[]   = { 1.0f, 1.0f, 1.0f, 1.0f };

/*------------------------------------------------------------------------------
   GLOBAL VARIABLES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static void resize(int width, int height);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

/** \brief Resize function. */
static void resize(int width, int height)
{
  // Calculate the factor of the window edges.
  const float ar = (float) width / (float) height;

  // Set the viewPort.
  glViewport(0, 0, width, height);
  // Select perspective projection.
  glMatrixMode(GL_PROJECTION);

  glLoadIdentity();
  glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

  // Set Matrixmode.
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity() ;
}

/** \brief Initialise 3D drawing module. */
void g3dInit(int argc, char *argv[])
{
  // Set the size of the window.
  glutInitWindowSize(640,480);

  // Set the position of the window's top left corner.
  glutInitWindowPosition(50,50);

  // Initialise Glut.
  glutInit(&argc, argv);

  // Set Glut display mode.
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

  // Create the window with the specified title.
  glutCreateWindow("4DTris");

  // Set the reshape function.
  glutReshapeFunc(resize);

  // Set background color.
  glClearColor(bg_color[0], bg_color[1], bg_color[2], bg_color[3]);

  // Enable cull face.
  glEnable(GL_CULL_FACE);
  // Set Cull face.
  glCullFace(GL_BACK);

  // Enable z buffer.
  glEnable(GL_DEPTH_TEST);
  // Set depth function.
  glDepthFunc(GL_LESS);

  // Enable various openGL features.
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);

  // Set up the light.
  glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  // Set up the material.
  glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

  // Enable alpha blend.
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
