#include<GL/glut.h>
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>

GLfloat br1=0,br2=0,br3=25,br4=50,br5=50;
GLfloat bul=25;
GLfloat bulX=25; // New variable to store bullet's x-position
GLint flag=0,flag2=0,flag1=0;
GLint bulletActive = 0; // Track if bullet is currently active
// Removing autoShootTimer as we don't need auto shooting
GLfloat b1x1=0,b1x2=50,b1x3=25,b1y1=575,b1y2=600;
GLfloat b2x1=500,b2x2=550,b2x3=525,b2y1=575,b2y2=600;
GLfloat b3x1=300,b3x2=350,b3x3=325,b3y1=575,b3y2=600;
GLfloat b4x1=200,b4x2=250,b4x3=225,b4y1=575,b4y2=600;

int count=0;
void live_score ();
void gamestatus();

void drawstring(float x,float y,char *string)
{
char *c;
 glRasterPos2f(x,y);
 for(c=string;*c!='\0';c++)
 {
  glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*c);
 }
}
void screen()
{
    glColor3f(1.0,1.0,1.0);
    drawstring(100,550,"COMPUTER GRAPHICS MINI PROJECT");
    drawstring(75,450,"Team Name:");
    drawstring(250,450,"Spirit");
    drawstring(75,500,"Game Name:");
    drawstring(250,500,"Bricks Breaker");
    drawstring(75,400,"By:");
    drawstring(50,350,"Your Nane");
    drawstring(150,250,"GUIDE:");
    drawstring(150,200,"Guide Name");
    drawstring(150,60,"Academic Year");
    drawstring(250,20,"press 'f' or space to continue");
glFlush();
}
void nextscreen()
{
    glColor3f(1.0,1.0,1.0);
    drawstring(250,550,"INSTRUCTIONS");
    drawstring(100,450,"press A to move left side");
    drawstring(100,400,"press D to move left side");
    drawstring(100,350,"press F or SPACE to fire bullet");
    drawstring(100,300,"press Q to exit");
    drawstring(150,20,"press 'n' to continue");
glFlush();
}

void live_score ()
{
    int len,i;
  char message[100] = {0};
  glPushMatrix();
    glLoadIdentity();
    glRasterPos2f(10, 550);
    glColor3f(0.0,0.5,0.1);
    sprintf(message,"Score: %d",count);
    len = (int)strlen(message);
    for (i = 0;i < len;i++) 
    {
      glutBitmapCharacter(GLUT_BITMAP_8_BY_13, message[i]);
    }
  glPopMatrix();
}


void idel()
{	
    // Auto-shooting logic has been removed, keeping only the bullet movement
    if(flag==1)
    {
        if(bul<600)
            bul=bul+5.000; // Keeping the faster bullet speed
        if(bul>=600) {
            bul=25;
            flag=0; // Reset bullet state
            bulletActive = 0;
        }
    }
    
    glutPostRedisplay();
    
    // Rest of collision detection code remains the same
    if(bulX==b1x3&&bul>b1y1&&bul<b1y2)
    {   
        if(b1x2<600)
        {
            b1y1=675;
            b1x1=b1x1+50;
            b1x2=b1x2+50;
            b1x3=b1x3+50;
            b1y2=700;
            count++;
        }
        else
        {
            b1x1=0;
            b1x2=50;
            b1x3=25;
            b1y1=575;
            b1y2=600;	
        }
        flag=0;
        bulletActive = 0;
        bul=25;
        live_score();
    }
    if(bulX==b2x3&&bul>b2y1&&bul<b2y2)
    {	
        if(b2x2>0)
        {
        b2y1=675;
        b2x1=b2x1-50;
        b2x2=b2x2-50;
        b2x3=b2x3-50;
        b2y2=700;
        count++;
        }
        else
        {
        b2x1=500;
        b2x2=550;
        b2x3=525;
        b2y1=575;
        b2y2=600;
        }
        flag=0;
        bulletActive = 0;
        bul=25;
        live_score();
    }
    if(bulX==b3x3&&bul>b3y1&&bul<b3y2)
    {	
        if(b3x2>0)
        {
        b3y1=675;
        b3x1=b3x1-50;
        b3x2=b3x2-50;
        b3x3=b3x3-50;
        b3y2=700;
        count++;
        }
        else
        {
            b3x1=300;
            b3x2=350;
            b3x3=325;
            b3y1=575;
            b3y2=600;
        }	
        flag=0;
        bulletActive = 0;
        bul=25;
        live_score();
    }
    if(bulX==b4x3&&bul>b4y1&&bul<b4y2)
    {
        if(b4x2<600)
        {
        b4y1=675;
        b4x1=b4x1+50;
        b4x2=b4x2+50;
        b4x3=b4x3+50;
        b4y2=700;
        count++;
        }
        else
        {
        b4x1=200;
        b4x2=250;
        b4x3=225;
        b4y1=575;
        b4y2=600;	
        }
        bul=25;
        flag=0;
        bulletActive = 0;
        live_score();
    }
}
void keyb(unsigned char key,int x,int y)
{	
    if(key=='f'||key=='F' || key==' ') 
    {  
        flag2=1;
        flag=1; 
        bulX = br3; // Set bullet x-position to current ship position when fired
        bulletActive = 1;
        glutIdleFunc(idel);
    }
    
    if(key=='n'||key=='N')
    {  
        flag1=1;
        flag=1; 
        glutIdleFunc(idel);
    }
    
    if(key=='d'||key=='D')
    {	
        if(br5<600)
        {
            br1=br1+50;
            br2=br2+50;
            br3=br3+50;
            br4=br4+50;
            br5=br5+50;
        }
    }
    
    if(key=='a'||key=='A')
    {	
        if(br1>0)
        {
            br1=br1-50;
            br2=br2-50;
            br3=br3-50;
            br4=br4-50;
            br5=br5-50;
        }
    }
    
    if(key=='q')
        exit(0);
}
void bricks()
{
    glColor3f(1.0,0.0,0.0);
    glBegin(GL_POLYGON);
        glVertex2f(b1x1,b1y1);
        glVertex2f(b1x1,b1y2);
        glVertex2f(b1x2,b1y2);
        glVertex2f(b1x2,b1y1);	
        glVertex2f(b1x3,b1y1);
    glEnd();
    glColor3f(0.0,1.0,0.0);
    glBegin(GL_POLYGON);
        glVertex2f(b2x1,b2y1);
        glVertex2f(b2x1,b2y2);
        glVertex2f(b2x2,b2y2);
        glVertex2f(b2x2,b2y1);	
        glVertex2f(b2x3,b2y1);
    glEnd();
    glColor3f(0.0,0.0,1.0);
    glBegin(GL_POLYGON);
        glVertex2f(b3x1,b3y1);
        glVertex2f(b3x1,b3y2);
        glVertex2f(b3x2,b3y2);
        glVertex2f(b3x2,b3y1);	
        glVertex2f(b3x3,b3y1);
    glEnd();
    glColor3f(1.0,1.0,0.0);
    glBegin(GL_POLYGON);
        glVertex2f(b4x1,b4y1);
        glVertex2f(b4x1,b4y2);
        glVertex2f(b4x2,b4y2);
        glVertex2f(b4x2,b4y1);	
        glVertex2f(b4x3,b4y1);
    glEnd();
    if(b1y1>0)
    {
    b1y1=b1y1-0.04;
    b1y2=b1y2-0.04;
    }
     else
    {
        b1y1==-600;
     
    gamestatus();
    }

    
    if(b2y1>0)
    {
    b2y1=b2y1-0.02;
    b2y2=b2y2-0.02;
    }
 else
    {
        b2y1==-600;
     
    gamestatus();
    }
    if(b3y1>0)
    {
    b3y1=b3y1-0.02;
    b3y2=b3y2-0.02;
    }
else
    {
        b3y1==-600;
     
    gamestatus();
    }
    if(b4y1>0)
    {
    b4y1=b4y1-0.04;
    b4y2=b4y2-0.04;
    }
else
    {
        b4y1==-600;
     
    gamestatus();
    }
    
    
    glutPostRedisplay();
    
}
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.7,0.2,0.8,1.0);
    if(flag2==0)
    {
    screen();
    }
    
    if(flag2==1)
    {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.7,0.2,0.8,1.0);
    if(flag1==0)
    {
    nextscreen();
        }
    if(flag1==1)
    {
    
    glColor3f(1.0,0.6,0.1);
    glBegin(GL_POLYGON);
       glVertex2f(br1,0);	
       glVertex2f(br2,20);
       glVertex2f(br3,25);
       glVertex2f(br4,20);
       glVertex2f(br5,0);
    glEnd();
    
    // Only draw the bullet if it's active
    if (flag == 1) {
        glColor3f(0.0,0.0,0.0);
        glPointSize(7);
        glBegin(GL_POINTS);
            glVertex2f(bulX,bul);
        glEnd();
    }
    
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
    gluOrtho2D(0,600,0,600);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.0,0.0,0.2,1.0);
}
void Write(char *string)
{ 
  while(*string)
  {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *string++);
  }
}

void gamestatus()
{
  char tmp_str[100];

  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(0,0,0);
  glRasterPos2f(50, 400);
  sprintf(tmp_str, "GAME OVER !!!!!!  Points: %d",count);

  Write(tmp_str);
}


int main(int argc,char** argv)
{
       glutInit(&argc,argv);
       glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
       glutInitWindowSize(600,600);
       glutInitWindowPosition(0,0);
       glutCreateWindow("brickbreaker");
       myinit();
       glutDisplayFunc(display);
       glutKeyboardFunc(keyb);
       glutMainLoop();
       return 0;
}