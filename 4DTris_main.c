/*

 Project: 4D-TRIS
 Author: Simon, Laszlo 

 Short Description:
 This program is a tetris game uses 4D hypercubes.  
 This file is the main unit of the application.  

 Change Log:
 1.4 - C compatible - simonl - 2008 jan 14.
       Code converted to c from c++.

 1.3 - Autoplayer added - simonl - 2008 jan 13
       Auto player connected. 
       Some Global variable moved to the begining.        
        
 1.2 - Functions added - simonl - 2008 jan 06
       Added score print out.
       Added comments.
        
 1.1 - Initial revision - simonl - 2008 jan 04

 ToDo: 

 */

/*
--------------------------------------------------------------------------------
   INCLUDES
--------------------------------------------------------------------------------
*/

// Include Glut library.
#include <GL/glut.h>

// Include standard library.
#include <stdlib.h>

// Include math library.
#include <math.h>

// Include the computer gamer header.
#include "4DTris_AI.h"

// Include the game engine.
#include "4DTris_engine.h"

/*
--------------------------------------------------------------------------------
   GLOBAL VARIABLES
--------------------------------------------------------------------------------
*/

// Struct of game engine:
extern t_game_Engine ge;

// Variables:

// flag for auto gamer
static int autoGamer_ON = 0;

// time ellapsed while the the solid steps one level down;
static double timestep = 1.5;

// time ellapsed while the AI steps the next turn;
static double timestepAI = 0.15;

// size of cube represent the lowest level;
static double baseSize = 0.75;
// size of cube represent the highest level;
static double maxSize = 2.0;

// initial direction of the viewPort (deg);
static double angleX = -60.0;
static double angleZ = 10.0;

// changes of the viewPort direction by one keypress (deg);
static double dangle = 10.0;

// array of the colors of game space levels.
GLfloat level_colors[SPACELENGTH][4];

// Constant light parameters
const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

// Constant material parameters
const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

// Background color.
const GLfloat bg_color[]   = { 1.0f, 1.0f, 1.0f, 1.0f };

// Color of the wireframe 4D cube.
const GLfloat wire4d_color[] = {0.4, 0.4, 0.6, 0.6};

// Color of the 4D cube.
const GLfloat cube4d_color[] = {0.4, 0.4, 0.6, 0.15};

/*
--------------------------------------------------------------------------------
   FUNCTIONS
--------------------------------------------------------------------------------
*/


/*------------------------------------------------------------------------------
    VISUALISATION
*/
// Calculates perspective projection factor of the 'level' 
// (game space 4th axis).
double perspFact(double level)
{ 
  // Local variables:
  double baseLevel, // lowest level if the viewpoint is at zero level;
         sizeFact,  // factor of highest and lowest level projected size; 
         result;    // return value.
  
  // Calculate size factor.
  sizeFact = baseSize / maxSize;
  
  // Calculate viewpoint distance from lowest level.
  baseLevel = (double)SPACELENGTH * sizeFact / (1 - sizeFact);
    
  // Calculate perspective projection.
  result = baseSize * (baseLevel + SPACELENGTH) / 
           (baseLevel + SPACELENGTH - level);
  
  // Return with the result value.
  return result;
}

// Calculates the crossProduct of v1 and v2 vector where
// 'n' the result vector.
void crossProduct(double *n, double v1[3], double v2[3])
{  
  // Calculate crossproduct by coordinates.
  n[0] = v1[1] * v2[2] - v2[1] * v1[2];
  n[1] = v1[2] * v2[0] - v2[2] * v1[0];
  n[2] = v1[0] * v2[1] - v2[0] * v1[1];
}

// Normalise a vector. 
void normalise(double *v)
{
  // Local variables:  
  int i; // loop counter;
  double l; // length of the vector.
  
  // Calculate length of the vector.
  l = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

  // For each coordinate
  for (i = 0; i < 3; i++)        
  {
    // normalise the coordinate.
    v[i] /= l;
  }
}

// Calculate normal vector of the plane of v1, v2 vector.
void calcNormal(double *n, double v1[3], double v2[3])
{
  // Calculate normal by crossproducting v1 and v2.
  crossProduct(n, v1, v2);     
  // Normalise the normal vector.
  normalise(n);
}

// Draw bitmap text to the specified coordinates and font.
void renderString(double x, double y, double z, void *font, char *string) 
{  
  // Local variables: 
  char *c; // actual character.
  
  // Set position in 3D.
  glRasterPos3d(x, y, z);
  
  // For each character of the string
  for (c=string; *c != '\0'; c++) 
  {
    // draw the character.  
    glutBitmapCharacter(font, *c);
  }
}

// Write out the score to the game window.
static void writeScore(void)
{
  // Local variables: 
  char text[20];     // buffer for the full text;
  char textbuff[10]; // buffer for the score value;
  int i;             // loop counter;
  int score;         // score value. 

  // Set color for the text.
  glColor4d(0.2, 0.2, 0.4, 0.8);
  // Convert the score value to character array.
  itoa(ge.score, textbuff, 10);
  
  // Set the buffer's each character to zero.
  for (i = 0; i < 20; i++) { text[i] = 0; }
  // Add the string to the text buffer.
  strcat(text, "Score: ");
  // Add the score value to the buffer.
  strcat(text, textbuff);
  // Render the text.
  renderString(-3.5, -2.5, -6.0, GLUT_BITMAP_9_BY_15, text);
}   

// Draw a 4D cube to the specified place of the game space.
static void cube4D(double x, double y, double z, double l)
{
  // Array contains the points of a 4D hypercube.
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
 
  // Array contains the faces (specified with num. of the points) 
  // of a 4D hypercube.    
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
  
  // Normal vector of the actual facet.
  double norm[3];
  // Vectors of two edge of the facet.
  double v1[3];
  double v2[3];

  // Loop counter.
  int n, i, j, k;

  // For each point of the hypercube
  for (n = 0; n < 16; n++)
  {
      // correct the coordinates with the perspective torsion to 
      // the direction of the 4th axis.
      points[n][0] = (points[n][0] + x) * perspFact(l - (n < 8)); 
      points[n][1] = (points[n][1] + y) * perspFact(l - (n < 8)); 
      points[n][2] = (points[n][2] + z) * perspFact(l - (n < 8));      
  }
  
  // For each facet and
  for (i = 0; i < 24; i++)
  {
    // for each point do: 
    for (j = 0; j < 4; j++)
    {
      // For each coordinate
      for (k = 0; k < 3; k++)
      { 
        // calculate two vector of two edge from the points.
        v1[k] = points[faces[i][1]][k] - points[faces[i][0]][k];
        v2[k] = points[faces[i][3]][k] - points[faces[i][0]][k];
      } 
      
      // Calculate normal vector.
      calcNormal(norm, v1, v2);        
        
      // Set the color of the facet.
      glColor4d(cube4d_color[0], cube4d_color[1], cube4d_color[2], cube4d_color[3]);
        
      // Start draw a quad.
      glBegin(GL_QUADS);
        // Set the normal vector.
        glNormal3d(norm[0], norm[1], norm[2]); 
         
        // For each point of the facet 
        for (k = 0; k < 4; k++)
        // set the points of the quad.
		glVertex3d(points[faces[i][k]][0], 
                   points[faces[i][k]][1], 
                   points[faces[i][k]][2]);
      // Finish drawing.
	  glEnd();

      // Set the color of the facet's wireframe.
      glColor4d(wire4d_color[0], wire4d_color[1], wire4d_color[2], wire4d_color[3]);

      // Start draw a line strip.
      glBegin(GL_LINE_STRIP);
        // For each point of the facet  				
        for (k = 0; k < 4; k++)
        // set the points of the strip.
		glVertex3d(points[faces[i][k]][0], 
                   points[faces[i][k]][1], 
                   points[faces[i][k]][2]);
      // Finish drawing.
	  glEnd();
    }  
  } 
         
}

// Resize function.
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

// Main drawing function.
static void display(void)
{
  // Local variables:
  int l, x, y, z;        // loop counter;      
  double size,  // actual size of a cube;
         shift; // actual shift of a cube.
       
  // Time storage for lower down the solid and AI step
  // initialised with ellapsed time since program started.
  static double timeLower = 0.0;
  static double timeAI = 0.0; 

  // Time ellapsed since program started. 
  const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    
  // If time ellapsed since last storage larger the time of step down,
  if (t - timeLower > timestep) {
    // lower the solid and    
    lowerSolid();
    // store the actual time.
    timeLower = t;        
  }  
  
  // If time ellapsed since last storage larger the time of AI step,
  // and auto player is switched on,
  if ((t - timeAI > timestepAI) && (autoGamer_ON)) {
    // make the AI step and     
    dostep();
    // store the actual time.
    timeAI = t;        
  }  
  
  // Get perspective ratio of the -1st level.
  size = perspFact(-1);

  // Clear the display buffer.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
  glPushMatrix();

    // Write out the game score.
    writeScore();
  
    // Place and orient the viewport.
    glTranslated(0, 0, -6);
    glRotated(angleX, 1, 0, 0);
    glRotated(angleZ, 0, 0, 1);
     
    // Draw the lower level.
     
    glTranslated(-0.5 * size, -0.5 * size, -0.5 * size); 

    // For each cell of the level do:
    for (x = 0; x <= 1; x++)
    for (y = 0; y <= 1; y++)
    for (z = 0; z <= 1; z++)
    {  
      glTranslated(x * size, y * size, z * size);
      
      // Set the color of the lower level,
      glColor4d(cube4d_color[0], cube4d_color[1], cube4d_color[2], cube4d_color[3]);
      // Draw the cube.
      glutSolidCube(size);
      
      // Set the color of the lower level's wireframe.
      glColor4d(wire4d_color[0], wire4d_color[1], wire4d_color[2], wire4d_color[3]);
      // Draw the cube's wireframe.
      glutWireCube(size * 1.01);

      glTranslated(-x * size, -y * size, -z * size);
    }
    
    glTranslated(0.5 * size, 0.5 * size, 0.5 * size);
           
    // Draw the gamespace.
           
    // For each level
    for (l = 0; l < SPACELENGTH; l++)
    {
      // Get perspective ratio of the actual level.
      size = perspFact(l);

      // Set the color of the actual level.
      glColor4d(level_colors[l][0],
                level_colors[l][1],
                level_colors[l][2],
                level_colors[l][3]);
    
      shift = (0.5 - 0.01 * size) * size;

      glTranslated(-shift, -shift, -shift);

      // For each cell of the level
      for (x = 0; x <= 1; x++)
      for (y = 0; y <= 1; y++)
      for (z = 0; z <= 1; z++)
      {  
        // if the cell is not empty then   
        if (!!getSpaceCell(l, x, y, z))
        {
          // draw the cube.
          glTranslated(x * 2 * shift, y * 2 * shift, z * 2 * shift);
          glutSolidCube(size);
          glTranslated(-x * 2 * shift, -y * 2 * shift, -z * 2 * shift);
        }
      }
      
      glTranslated(shift, shift, shift);
    }
    
    // Draw the actual solid.
    
    // Set actual size to base value.
    size = baseSize;
    
    // Set the color of the solid.
    glColor4d(0.4, 0.4, 0.6, 0.2);
      
    // For each cell
    for (l = 0; l < 2; l++)
    {      
      for (x = 0; x <= 1; x++)
      for (y = 0; y <= 1; y++)
      for (z = 0; z <= 1; z++)
      {  
        // if the cell is not empty then  
        if (!!getSolidCell(l, x, y, z))
        {
          // draw the hypercube.
          cube4D(x - 1, y - 1, z - 1, ge.pos + l); 
        }
      }

    }
    
  glPopMatrix();

  // Swap the buffers.
  glutSwapBuffers();
}


/*------------------------------------------------------------------------------
    EVENT HANDLING
*/
// Eventhandler of key pressing.
static void key(unsigned char key, int x, int y)
{
  // Check which key pressed.
  switch (key) 
  {
    // If 'Esc' or 'q' then
    case 27 : 
    case 'q':
      // exit from the software
      exit(0);
    break;
    
    // If 'z' then 
    case 'z':
      // turn the solid 
      // around axis 2.   
      turn(0, 1);
    break;
    
    // If 'x' then 
    case 'x':
      // turn the solid 
      // around axis 0.  
      turn(1, 2);
    break;
    
    // If 'c' then 
    case 'c':
      // turn the solid 
      // around axis 1.  
      turn(2, 0);
    break;

    // If 'v' then     
    case 'v':
      // turn the solid 
      // around axis 4.  
      turn(0, 3);
    break;
    
    // If space then  
    case ' ':
      // step down the solid.   
      lowerSolid();
    break;
    
    case '1':
      // Set game difficulty to easy.
      ge.game_opts.diff = 0;
    break;
    
    case '2':
      // Set game difficulty to medium.
      ge.game_opts.diff = 1;
    break;
    
    case '3':
      // Set game difficulty to hard.
      ge.game_opts.diff = 2;
    break;

    case 'a':
      // Switch the autoplayer on.
      autoGamer_ON = !autoGamer_ON;
    break;

  }


  // Refresh the display.
  glutPostRedisplay();
}

// Eventhandler of special key pressing.
static void special(int key, int x, int y)
{
  // Check which key pressed.
  switch (key) 
  {
    // If up arrow then  
    case GLUT_KEY_UP:
      // rotate viewport around x axis 
      // to positive direction.  
      angleX += dangle;
    break;
    // If up arrow then 
    case GLUT_KEY_LEFT:
      // rotate viewport around z axis 
      // to negative direction.  
      angleZ += dangle;
    break;
    // If up arrow then 
    case GLUT_KEY_DOWN:
      // rotate viewport around x axis 
      // to positive direction.  
      angleX -= dangle;
    break;
    // If up arrow then 
    case GLUT_KEY_RIGHT:
      // rotate viewport around z axis 
      // to negative direction.  
      angleZ -= dangle;
    break;
  }

  // Refresh the display.
  glutPostRedisplay();
}

// Idle process.
static void idle(void)
{
  // Refresh the display.
  glutPostRedisplay();
}

/*------------------------------------------------------------------------------
    INITIALISATION
*/
// Main function of the software
int main(int argc, char *argv[])
{
  // Loop counters.
  int i, j;

  // Game option structure for initialize game engine.
  t_game_opts in_game_opts;
  
  // For each level of the game space,
  for (i = 0; i < SPACELENGTH; i++)
  {
    // for each color component
    for (j = 0; j < 3; j++)
    {     
      // create a random value.    
      level_colors[i][j] = (double)rand() / RAND_MAX;
    }
    // Set the color's alpha component.
    level_colors[i][3] = 0.95;
  }
  
  // Set game difficulty parameter.
  in_game_opts.diff = 2;
  
  // Initialize the game engine.
  initGame(in_game_opts);

  // Initialise Glut.
  glutInit(&argc, argv);
  // Set the size of the window.
  glutInitWindowSize(640,480);
  // Set the position of the window's top left corner.
  glutInitWindowPosition(50,50);
  // Set Glut display mode.
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

  // Create the window with the specified title.
  glutCreateWindow("4D-TRIS");

  // Set the reshape function.
  glutReshapeFunc(resize);
  // Set the display function.
  glutDisplayFunc(display);  
  // Set the keypress event handler function.
  glutKeyboardFunc(key);
  // Set the special keypress event handler function.
  glutSpecialFunc(special);
  // Set the idle function.
  glutIdleFunc(idle);

  // Set background color.
  glClearColor(bg_color[0],bg_color[1],bg_color[2],bg_color[3]);

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
  glEnable(GL_BLEND);
  
  // Start glut's main loop.
  glutMainLoop();

  // Return with successed exit constant.
  return EXIT_SUCCESS;
}
