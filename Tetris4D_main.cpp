/*

 Project: Tetris4D
 Author: Simon, Laszlo 

 Short Description:
 This program is a tetris game uses 4D hypercubes.  
 This file is the main unit of the application.  

 Change Log:
 1.1 - Initial revision - 2008 jan 04

 ToDo: 

 */

#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>

#include "Tetris4D_engine.h"

// Variables from game engine:

// the game space
extern TLevel space[SPACELENGTH];
// actual solid
extern TSolid solid;
// position of actual solid
// 0 if reached the floor
extern char pos;
// score collected in the actual game
extern int score;

// Variables:

static double timestep = 1.0;

static double baseSize = 0.75;
static double maxSize = 2.0;

static double angleX = -60.0;
static double angleZ = 0.0; 
static double dangle = 10.0;


GLfloat level_colors[SPACELENGTH][4];

double perspFact(double level)
{
  double baseLevel, sizeFact, result;
  
  sizeFact = baseSize / maxSize;
  
  baseLevel = (double)SPACELENGTH * sizeFact / (1 - sizeFact);
    
  result = baseSize * (baseLevel + SPACELENGTH) / 
           (baseLevel + SPACELENGTH - level);
  
  return result;
}

void crossProduct(double *n, double v1[3], double v2[3])
{  
  n[0] = v1[1] * v2[2] - v2[1] * v1[2];
  n[1] = v1[2] * v2[0] - v2[2] * v1[0];
  n[2] = v1[0] * v2[1] - v2[0] * v1[1];
}

void normalise(double *v)
{
  double l;
  
  l = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

  for (int i = 0; i < 3; i++)        
  {
    v[i] /= l;
  }
}

void calcNormal(double *n, double v1[3], double v2[3])
{
  crossProduct(n, v1, v2);     
  normalise(n);
}

void renderBitmapString(float x, float y, float z, void *font, char *string) 
{  
  char *c;
  glRasterPos3f(x, y,z);
  for (c=string; *c != '\0'; c++) 
  {
    glutBitmapCharacter(font, *c);
  }
}

static void cube4D(double x, double y, double z, double l)
{
  double points[16][4] = 
  /*  x,   y,   z,   l */
  {{0.0, 0.0, 0.0, 0.0},
   {1.0, 0.0, 0.0, 0.0},
   {0.0, 1.0, 0.0, 0.0},
   {1.0, 1.0, 0.0, 0.0},
   {0.0, 0.0, 1.0, 0.0},
   {1.0, 0.0, 1.0, 0.0},
   {0.0, 1.0, 1.0, 0.0},
   {1.0, 1.0, 1.0, 0.0},
   {0.0, 0.0, 0.0, 1.0},
   {1.0, 0.0, 0.0, 1.0},
   {0.0, 1.0, 0.0, 1.0},
   {1.0, 1.0, 0.0, 1.0},
   {0.0, 0.0, 1.0, 1.0},
   {1.0, 0.0, 1.0, 1.0},
   {0.0, 1.0, 1.0, 1.0},
   {1.0, 1.0, 1.0, 1.0}};
     
  char faces[24][4] =
  {
// outer cube
{   0,   4,   6,   2}, {   1,   3,   7,   5}, {   0,   1,   5,   4}, 
{   2,   6,   7,   3}, {   0,   2,   3,   1}, {   4,   5,   7,   6}, 
// inner cube
{   8, 0xA, 0xE, 0xC}, {   9, 0xD, 0xF, 0xB}, {   8, 0xC, 0xD,   9},
{   9, 0xB, 0xF, 0xD}, {   8, 0xA, 0xB,   9}, { 0xC, 0xE, 0xF, 0xD},
// intermediate faces
{   0,   8, 0xC,   4}, {   0,   8, 0xA,   2}, { 0xA,   2,   6, 0xE}, { 0xE,   6,   4, 0xC},
{   1,   9, 0xB,   3}, { 0xB,   3,   7, 0xF}, {   7, 0xF, 0xD,   5}, { 0xD,   5,   1,   9}, 
{ 0xC, 0xD,   5,   4}, {   8,   9,   1,   0}, { 0xA, 0xB,   3,   2}, { 0xE, 0xF,   7,   6}
  };
  
  double norm[3];
  double v1[3];
  double v2[3];

  for (int n = 0; n < 16; n++)
  {
      points[n][0] = (points[n][0] + x) * perspFact(l - (n < 8)); 
      points[n][1] = (points[n][1] + y) * perspFact(l - (n < 8)); 
      points[n][2] = (points[n][2] + z) * perspFact(l - (n < 8));      
  }
  
  for (int i = 0; i < 24; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      for (int k = 0; k < 3; k++)
      {  
        v1[k] = points[faces[i][1]][k] - points[faces[i][0]][k];
        v2[k] = points[faces[i][3]][k] - points[faces[i][0]][k];
      } 
      
      calcNormal(norm, v1, v2);        
        
      glColor4d(0.4, 0.4, 0.6, 0.15);
        
      glBegin(GL_QUADS);
        glNormal3d(norm[0], norm[1], norm[2]); 
         
        for (int k = 0; k < 4; k++)
		glVertex3d(points[faces[i][k]][0], 
                   points[faces[i][k]][1], 
                   points[faces[i][k]][2]);
	  glEnd();

      glColor4d(0.4, 0.4, 0.6, 0.6);

      glBegin(GL_LINE_STRIP); 				
        for (int k = 0; k < 4; k++)
		glVertex3d(points[faces[i][k]][0], 
                   points[faces[i][k]][1], 
                   points[faces[i][k]][2]);
	  glEnd();
    }  
  } 
         
}


static void resize(int width, int height)
{
  const float ar = (float) width / (float) height;
    
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
    
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity() ;
}

static void display(void)
{
  double size, shift;
  char *text;
       
  static double time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
  
  const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    
  if (t - time > timestep) {
    lowerSolid();
    time = t;        
  }
  
  size = perspFact(-1);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glPushMatrix();

    glTranslated(0, 0, -6);
    glRotated(angleX, 1, 0, 0);
    glRotated(angleZ, 0, 0, 1);
     
    glTranslated(-0.5 * size, -0.5 * size, -0.5 * size); 

    for (int x = 0; x <= 1; x++)
    for (int y = 0; y <= 1; y++)
    for (int z = 0; z <= 1; z++)
    {  
      glTranslated(x * size, y * size, z * size);
      glColor4d(0.4, 0.4, 0.6, 0.2);
      glutSolidCube(size);
      glColor4d(0.4, 0.4, 0.6, 0.6);
      glutWireCube(size * 1.01);
      glTranslated(-x * size, -y * size, -z * size);
    }
    glTranslated(0.5 * size, 0.5 * size, 0.5 * size);
           
    for (int l = 0; l < SPACELENGTH; l++)
    {
      size = perspFact(l);

      glColor4d(level_colors[l][0],
                level_colors[l][1],
                level_colors[l][2],
                level_colors[l][3]);
    
      shift = (0.5 - 0.01 * size) * size;

      glTranslated(-shift, -shift, -shift);

      for (int x = 0; x <= 1; x++)
      for (int y = 0; y <= 1; y++)
      for (int z = 0; z <= 1; z++)
      {  
        if (!!getSpaceCell(l, x, y, z))
        {
          glTranslated(x * 2 * shift, y * 2 * shift, z * 2 * shift);
          glutSolidCube(size);
          glTranslated(-x * 2 * shift, -y * 2 * shift, -z * 2 * shift);
        }
      }
      
      glTranslated(shift, shift, shift);
    }
    
    size = baseSize;
    glColor4d(0.4, 0.4, 0.6, 0.2);
      
    for (int l = 0; l < 2; l++)
    {      
      for (int x = 0; x <= 1; x++)
      for (int y = 0; y <= 1; y++)
      for (int z = 0; z <= 1; z++)
      {  
        if (!!getSolidCell(l, x, y, z))
        {
          cube4D(x - 1, y - 1, z - 1, pos + l); 
        }
      }

    }
    
  glPopMatrix();

  glutSwapBuffers();
}

static void key(unsigned char key, int x, int y)
{
  switch (key) 
  {
    case 27 : 
    case 'q':
      exit(0);
    break;
    
    case 'z':
      turn(0, 1);
    break;
    
    case 'x':
      turn(1, 2);
    break;
    
    case 'c':
      turn(2, 0);
    break;
    
    case 'v':
      turn(0, 3);
    break;
    
    case ' ':
      lowerSolid();
    break;
  }

  glutPostRedisplay();
}

static void special(int key, int x, int y)
{
  switch (key) 
  {
    case GLUT_KEY_UP:
      angleX += dangle;
    break;
    case GLUT_KEY_LEFT:
      angleZ += dangle;
    break;
    case GLUT_KEY_DOWN:
      angleX -= dangle;
    break;
    case GLUT_KEY_RIGHT:
      angleZ -= dangle;
    break;
  }

  glutPostRedisplay();
}


static void idle(void)
{
  glutPostRedisplay();
}

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

int main(int argc, char *argv[])
{
  t_game_opts in_game_opts;
  
  for (int i = 0; i < SPACELENGTH; i++)
  {
    for (int j = 0; j < 3; j++)
    {     
    level_colors[i][j] = (double)rand() / RAND_MAX;
    }
    level_colors[i][3] = 0.95;
  }
  
  in_game_opts.diff = 1;
  
  initGame(in_game_opts);

  glutInit(&argc, argv);
  glutInitWindowSize(640,480);
  glutInitWindowPosition(50,50);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

  glutCreateWindow("Tetris 4D");

  glutReshapeFunc(resize);
  glutDisplayFunc(display);
  glutKeyboardFunc(key);
  glutSpecialFunc(special);
  glutIdleFunc(idle);

  glClearColor(1.0,1.0,1.0,1.0);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);

  glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glEnable(GL_AUTO_NORMAL);
  glEnable(GL_NORMALIZE);
//  glDisable(GL_DEPTH_TEST); 
  
  glutMainLoop();

  return EXIT_SUCCESS;
}
