#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

GLfloat br1 = 0, br2 = 0, br3 = 25, br4 = 50, br5 = 50;
GLfloat bul = 25;
GLint flag = 0, flag2 = 0, flag1 = 0;
GLfloat b1x1 = 0, b1x2 = 50, b1x3 = 25, b1y1 = 575, b1y2 = 600;
GLfloat b2x1 = 500, b2x2 = 550, b2x3 = 525, b2y1 = 575, b2y2 = 600;
GLfloat b3x1 = 300, b3x2 = 350, b3x3 = 325, b3y1 = 575, b3y2 = 600;
GLfloat b4x1 = 200, b4x2 = 250, b4x3 = 225, b4y1 = 575, b4y2 = 600;

int count = 0;
void live_score();
void gamestatus();

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.7, 0.2, 0.8, 1.0);
    if (flag2 == 0)
    {
        screen();
    }

    if (flag2 == 1)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.7, 0.2, 0.8, 1.0);
        if (flag1 == 0)
        {
            nextscreen();
        }
        if (flag1 == 1)
        {

            glColor3f(1.0, 0.6, 0.1);
            glBegin(GL_POLYGON);
            glVertex2f(br1, 0);
            glVertex2f(br2, 20);
            glVertex2f(br3, 25);
            glVertex2f(br4, 20);
            glVertex2f(br5, 0);
            glEnd();

            glColor3f(0.0, 0.0, 0.0);
            glPointSize(7);
            glBegin(GL_POINTS);
            glVertex2f(br3, bul);
            glEnd();

            bricks();
            live_score();

            glFlush();
        }
    }
}
void myinit()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 600, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.0, 0.0, 0.2, 1.0);
}
void Write(char *string)
{
    while (*string)
    {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *string++);
    }
}

void gamestatus()
{
    char tmp_str[100];

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0, 0, 0);
    glRasterPos2f(50, 400);
    sprintf(tmp_str, "GAME OVER !!!!!!  Points: %d", count);

    Write(tmp_str);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("brickbreaker");
    myinit();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyb);
    glutMainLoop();
    return 0;
}
