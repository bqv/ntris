/**
 * \file 4dt_g3d.c
 * \brief 3D OpenGL scene rendering modul.
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>

#include "4dt_m3d.h"

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
static const GLfloat g3dLightAmbient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
static const GLfloat g3dLightDiffuse[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
static const GLfloat g3dLightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static const GLfloat g3dLightPosition[] = { 2.0f, -5.0f, 2.5f, 1.0f };

/** Constant material parameters */
static const GLfloat g3dMatAmbient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
static const GLfloat g3dMatDiffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
static const GLfloat g3dMatSpecular[]   = { 0.6f, 0.6f, 0.5f, 1.0f };
static const GLfloat g3dHighShininess[] = { 30.0f };

/** Background color. */
static const GLfloat g3dBgColor[]   = { 1.0f, 1.0f, 1.0f, 1.0f };


/*------------------------------------------------------------------------------
   GLOBAL VARIABLES
------------------------------------------------------------------------------*/


/** initial direction of the viewPort (deg); */
double g3dAngleX = -75.0;
double g3dAngleZ = 20.0;

/** changes of the viewPort direction by one keypress (deg); */
double g3dDAngle = 10.0;

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static void g3dResize(int width, int height);
static void g3dSwitchTo2D(void);
static void g3dSwitchTo3D(void);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

/** \brief Starts the actual frame drawing */
void g3dBeginPreDraw(void)
{
  // Clear the display buffer.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/** Switch the projection to 2D mode for window coordinate draw
 *  (coordinate interval: 0, 0 - 1, 1) */
static void g3dSwitchTo2D(void)
{
  // switch off light and Z testing
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  // store matrices
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode (GL_MODELVIEW);
  glPushMatrix();

  // set 2D coord.sys.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, 1, 0, 1);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
}

/** Switch back the projection mode from 2D mode */
static void g3dSwitchTo3D(void)
{
  // restore matrices
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode (GL_MODELVIEW);
  glPopMatrix();

  // reenable depth test and light
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
}

/** Draws a rectangle filled with gradient color */
void g3dDrawRectangle(float x0, float y0, float x1, float y1,
                      float color1[4], float color2[4])
{
  g3dSwitchTo2D();

  glBegin(GL_QUADS);
  {
    glColor4f(color1[0], color1[1], color1[2], color1[3]);

    glVertex2f(x0, y0);
    glVertex2f(x1, y0);

    glColor4f(color2[0], color2[1], color2[2], color2[3]);

    glVertex2f(x1, y1);
    glVertex2f(x0, y1);
  }
  glEnd();

  g3dSwitchTo3D();
}

/** todo: temporary function, should be merged to g3dBeginPreDraw */
void g3dBeginDraw(void)
{
  glPushMatrix();
  glLoadIdentity();

  // Place and orient the viewport.
  glTranslated(0, 0, -6);
  glRotated(g3dAngleX, 1, 0, 0);
  glRotated(g3dAngleZ, 0, 0, 1);
}


/** \brief Close the actual frame drawing */
void g3dEndDraw(void)
{
  glPopMatrix();

  // Swap the buffers.
  glutSwapBuffers();
}


/** Draw bitmap text to the specified coordinates and font. */
void g3dRenderString(double x, double y,
                    float color[4],
                    char *string)
{
  // Local variables:
  char *c; // actual character.

  void *font =  GLUT_BITMAP_HELVETICA_18;

  g3dSwitchTo2D();

  // Set color for the text.
  glColor4d(color[0], color[1], color[2], color[3]);

  // Set position.
  glRasterPos2f(x, y);


  // For each character of the string
  for (c=string; *c != '\0'; c++)
  {
    // draw the character.
    glutBitmapCharacter(font, *c);
  }

  g3dSwitchTo3D();
}

/** \brief Draw quad with given coordinates, color, style. */
void g3dDrawPoly(float points[4][4],
                 float color[4],
                 int mode /**< 0: fill, 1: fill & wire, 2: wire */)
{
  // loop counters
  int k;
  int transparent;
  // Normal vector of the actual facet.
  double norm[3];
  // Vectors of two edge of the facet.
  double v1[3];
  double v2[3];

  // For each coordinate
  for (k = 0; k < 3; k++)
  {
    // calculate two vector of two edge from the points.
    v1[k] = points[1][k] - points[0][k];
    v2[k] = points[3][k] - points[0][k];
  }

  // Calculate normal vector.
  m3dCalcNormal(norm, v1, v2);

  // Set the color of the facet.
  glColor4d(color[0], color[1], color[2], color[3]);

  transparent = (color[3] < 1.0);

  if (transparent)
  {
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
  }

  if (mode < 2)
  {
    // Start draw a quad.
    glBegin(GL_QUADS);
    // Set the normal vector.
    glNormal3d(norm[0], norm[1], norm[2]);

    // For each point of the facet
    for (k = 0; k < 4; k++)
    {
      // set the points of the quad.
      glVertex3d(points[k][0],
                 points[k][1],
                 points[k][2]);
    }
    // Finish drawing.
    glEnd();
  }

  // if enabled wire draw
  if (mode > 0)
  {
    // Start draw a line strip.
    glBegin(GL_LINE_STRIP);

    // For each point of the facet
    for (k = 0; k < 4; k++)
    {
      // set the points of the strip.
      glVertex3d(points[k][0],
                 points[k][1],
                 points[k][2]);
    }

    // Close the strip.
    glVertex3d(points[0][0],
               points[0][1],
               points[0][2]);

    // Finish drawing.
    glEnd();
  }

  if (transparent)
  {
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
  }
}

/** \brief Resize function. */
static void g3dResize(int width, int height)
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
  glutReshapeFunc(g3dResize);

  // Set background color.
  glClearColor(g3dBgColor[0], g3dBgColor[1], g3dBgColor[2], g3dBgColor[3]);

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
  glLightfv(GL_LIGHT0, GL_AMBIENT,  g3dLightAmbient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  g3dLightDiffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, g3dLightSpecular);
  glLightfv(GL_LIGHT0, GL_POSITION, g3dLightPosition);

  // Set up the material.
  glMaterialfv(GL_FRONT, GL_AMBIENT,   g3dMatAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   g3dMatDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  g3dMatSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, g3dHighShininess);

  // Enable alpha blend.
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
