/**
 * \file 4dt_g3d.c
 * \brief 3D OpenGL scene rendering modul.
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "4dt_m.h"
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
static const GLfloat g3dLightAmbient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
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

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static void g3dSwitchTo2D(void);
static void g3dSwitchTo3D(void);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

/** Converts 2D window coordinates to 3D object coordinates or vice versa*/
tM3dVector g3dTransformTo(tG3dSystem target, tM3dVector v)
{
    int   viewport[4];
    double modelview[16];
    double projection[16];
    double x, y, z;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    v.c[eM3dAxisY] = (float)viewport[3] - (float)v.c[eM3dAxisY];

    if (target == eG3dWorld3D)
    {
        glReadPixels((int)v.c[eM3dAxisX], (int)v.c[eM3dAxisY],
                     1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &(v.c[eM3dAxisZ]));

        gluUnProject(v.c[eM3dAxisX], v.c[eM3dAxisY], v.c[eM3dAxisZ],
                     modelview, projection, viewport,
                     &x, &y, &z);
    }
    else
    {
        gluProject(v.c[eM3dAxisX], v.c[eM3dAxisY], v.c[eM3dAxisZ],
                   modelview, projection, viewport,
                   &x, &y, &z);
    }

    return(m3dVector(x, y, z));
}

/** Sets/unsets transparent mode: Blend + no Depthmask */
void g3dSetTransparentMode(int enable)
{
    if (enable)
    {
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
    }
    else
    {
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
}


/** \brief Starts the actual frame drawing */
void g3dBeginDraw(int x, int y, int z, int picnum, int anaglyph)
{
    if ((picnum == 0) || (anaglyph == 1))
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    if (anaglyph == 1)
    {
        if (picnum == 0)
        {
            glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
        }
        else
        {
            glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
        }
    }

    glPushMatrix();
    glLoadIdentity();

    /*  Place and orient the viewport. */
    glTranslated(-x, -y, -z);

    glRotated(-75.0, 1, 0, 0);
    glRotated(20.0, 0, 0, 1);

    if (anaglyph == 1)
    {
        glRotated((picnum == 0) ? 0 : -4, 0, 0, 1);
    }
}

/** Switch the projection to 2D mode for window coordinate draw
 *  (coordinate interval: 0, 0 - 1, 1) */
static void g3dSwitchTo2D(void)
{
    /*  switch off light and Z testing */
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);

    /*  store matrices */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix();

    /*  set 2D coord.sys. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,1,0,1,-1,1);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
}

/** Switch back the projection mode from 2D mode */
static void g3dSwitchTo3D(void)
{
    /*  restore matrices */
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode (GL_MODELVIEW);
    glPopMatrix();

    /*  reenable depth test and light */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
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

/** \brief Close the actual frame part drawing */
void g3dEndDrawPic(void)
{
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glPopMatrix();
}

/** \brief Close the actual frame drawing */
void g3dEndDraw(void)
{
    /*  Swap the buffers. */
    SDL_GL_SwapBuffers();
}

/** Draws 3D line */
void g3dDrawLine(tM3dVector point0,
                 tM3dVector point1,
                 float color0[4],
                 float color1[4],
                 float linewidth)
{
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);

    glLineWidth(linewidth);

    /*  Start draw a line. */
    glBegin(GL_LINES);

    glColor4f(color0[0], color0[1], color0[2], color0[3]);
    glVertex3d(point0.c[0], point0.c[1], point0.c[2]);

    glColor4f(color1[0], color1[1], color1[2], color1[3]);
    glVertex3f(point1.c[0], point1.c[1], point1.c[2]);

    /*  Finish drawing. */
    glEnd();

    glLineWidth(1.0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

/** Draws a cylinder */
void g3dDrawCylinder(tM3dVector v1,
                     tM3dVector v2,
                     float radius)
{
    int resolution = 5;
    GLUquadricObj *quadric = gluNewQuadric();
    tM3dVector v12, z0, z1, rotaxle;
    double angle, length, sin, cos;

    z0      = m3dVector(0.0, 0.0, 1.0);
    v12     = m3dSub(v2,v1);
    z1      = m3dNormalise(v12);
    length  = m3dAbs(v12);
    rotaxle = m3dCrossProduct(z0, z1);
    cos     = m3dMultiplyVV(z0, z1);
    sin     = m3dAbs(rotaxle);
    angle   = atan2(sin, cos) * 180 / M_PI;

    if (fabs(sin) == 0.0)
    {
        rotaxle.c[0] = 1;
    }
    rotaxle = m3dNormalise(rotaxle);

    gluQuadricNormals(quadric, GLU_SMOOTH);
    glPushMatrix();
    glTranslatef(v1.c[0], v1.c[1], v1.c[2]);
    glRotatef(angle, rotaxle.c[0], rotaxle.c[1], rotaxle.c[2]);
    gluQuadricOrientation(quadric,GLU_OUTSIDE);
    gluCylinder(quadric, radius, radius, length, resolution, 1);
    glPopMatrix();
    gluDeleteQuadric(quadric);
}

/** Draws a sphere */
void g3dDrawSphere(tM3dVector o, double radius)
{
    int resolution = 5;

    GLUquadricObj *quadric=gluNewQuadric();

    gluQuadricNormals(quadric, GLU_SMOOTH);
    glPushMatrix();
    glTranslatef(o.c[0], o.c[1], o.c[2]);
    gluSphere(quadric, radius, resolution, resolution);
    glPopMatrix();
    gluDeleteQuadric(quadric);
}

/** \brief Draw quadratic polygon wire with given coordinates, color. */
void g3dDrawPolyWire(tM3dVector points[4],
                     float color[4],
                     int sideVisible[4])
{
    int k;

    /*  Set the color of the facet. */
    glColor4d(color[0], color[1], color[2], color[3]);

    /*  For each point of the facet */
    for (k = 0; k < 4; k++)
    {
        if ((sideVisible == NULL) || (sideVisible[k] == 1))
        {
            glBegin(GL_LINE);

            glVertex3d(points[k].c[0],
                       points[k].c[1],
                       points[k].c[2]);

            glVertex3d(points[(k+1) % 4].c[0],
                       points[(k+1) % 4].c[1],
                       points[(k+1) % 4].c[2]);
            glEnd();
        }
    }
}

/** Draw quadratic polygon wire from tubes with given coordinates, color. */
void g3dDrawPolyTube(tM3dVector points[4],
                     float color[4],
                     int sideVisible[4])
{
    const double lineWidth = 0.04;

    int k;

    /*  Set the color of the facet. */
    glColor4d(color[0], color[1], color[2], color[3]);

    /*  For each point of the facet */
    for (k = 0; k < 4; k++)
    {
        if ((sideVisible == NULL) || (sideVisible[k] == 1))
        {
            g3dDrawSphere(points[k], lineWidth/2.0);

            g3dDrawCylinder(points[k],
                            points[(k+1) % 4],
                            lineWidth/2.0);
        }
    }
}

/** \brief Draw filled quadratic polygon with given coordinates, color. */
void g3dDrawPolyFill(tM3dVector points[4],
                     float color[4])
{
    int vertex;

    /*  calculate two vector of two edge from the points. */
    tM3dVector v1 = m3dSub(points[1], points[0]);
    tM3dVector v2 = m3dSub(points[3], points[0]);

    /*  Calculate normal vector. */
    tM3dVector norm = m3dCalcNormal(v1, v2);

    glColor4d(color[0], color[1], color[2], color[3]);

    glBegin(GL_QUADS);

    glNormal3d(norm.c[0], norm.c[1], norm.c[2]);

    for (vertex = 0; vertex < 4; vertex++)
    {
        glVertex3d(points[vertex].c[0],
                   points[vertex].c[1],
                   points[vertex].c[2]);
    }

    glEnd();
}

/** \brief Resize function. */
void g3dResize(int width, int height)
{
    /*  Calculate the factor of the window edges. */
    const float ar = (float) width / (float) height;

    /*  Set the viewPort. */
    glViewport(0, 0, width, height);
    /*  Select perspective projection. */
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 16.0);

    /*  Set Matrixmode. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

/** \brief Initialise 3D drawing module. */
void g3dInit(void)
{
    /*  Set background color. */
    glClearColor(g3dBgColor[0], g3dBgColor[1], g3dBgColor[2], g3dBgColor[3]);

    /*  Enable cull face. */
    /*  glEnable(GL_CULL_FACE); */
    /*  Set Cull face. */
    glCullFace(GL_BACK);

    /*  Enable z buffer. */
    glEnable(GL_DEPTH_TEST);
    /*  Set depth function. */
    glDepthFunc(GL_LESS);

    /*  Enable various openGL features. */
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    /*  Set up the light. */
    glLightfv(GL_LIGHT0, GL_AMBIENT,  g3dLightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  g3dLightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, g3dLightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, g3dLightPosition);

    /*  Set up the material. */
    glMaterialfv(GL_FRONT, GL_AMBIENT,   g3dMatAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   g3dMatDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  g3dMatSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, g3dHighShininess);

    /*  Enable alpha blend. */
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
