#include<GL/glut.h>
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>

// Define a structure for bullets
typedef struct {
    GLfloat x;       // X-position
    GLfloat y;       // Y-position
    int active;      // Is bullet active? (1=yes, 0=no)
} Bullet;

#define MAX_BULLETS 10  // Maximum number of bullets allowed on screen
Bullet bullets[MAX_BULLETS];  // Array to store bullets
int activeBulletCount = 0;    // Track how many bullets are currently active

GLfloat br1=0,br2=0,br3=25,br4=50,br5=50;
// Removing single bullet variables as we're now using an array of bullets
GLint flag=0,flag2=0,flag1=0;
// Removing bulletActive as we now track each bullet's state individually
// Removing autoShootTimer as we don't need auto shooting
GLfloat b1x1=0,b1x2=50,b1x3=25,b1y1=575,b1y2=600;
GLfloat b2x1=500,b2x2=550,b2x3=525,b2y1=575,b2y2=600;
GLfloat b3x1=300,b3x2=350,b3x3=325,b3y1=575,b3y2=600;
GLfloat b4x1=200,b4x2=250,b4x3=225,b4y1=575,b4y2=600;

// Difficulty scaling variables
GLfloat baseSpeed = 0.12;    // Increased from 0.04 to 0.12 for faster initial speed
GLfloat speedFactor = 1.0;   // Speed multiplier that increases with score
int lastSpeedIncrease = 0;   // Tracks when speed was last increased
int speedIncreaseInterval = 5; // Increase speed every 5 points
int maxActiveBricks = 4;     // Initial number of active bricks
int brickActiveStatus[4] = {1, 1, 1, 1}; // Track which bricks are active (1=active, 0=inactive)
int respawnTimer = 0;        // Timer for respawning bricks
int respawnDelay = 100;      // Frames to wait before respawning

int count=0;
void live_score ();
void gamestatus();

// Function to randomize brick position
void randomize_brick_position(int brick_num) {
    int x_pos = (rand() % 11) * 50; // Random X position (0-550 in steps of 50)
    
    switch(brick_num) {
        case 1:
            b1x1 = x_pos;
            b1x2 = x_pos + 50;
            b1x3 = x_pos + 25;
            b1y1 = 575;
            b1y2 = 600;
            brickActiveStatus[0] = 1;
            break;
        case 2:
            b2x1 = x_pos;
            b2x2 = x_pos + 50;
            b2x3 = x_pos + 25;
            b2y1 = 575;
            b2y2 = 600;
            brickActiveStatus[1] = 1;
            break;
        case 3:
            b3x1 = x_pos;
            b3x2 = x_pos + 50;
            b3x3 = x_pos + 25;
            b3y1 = 575;
            b3y2 = 600;
            brickActiveStatus[2] = 1;
            break;
        case 4:
            b4x1 = x_pos;
            b4x2 = x_pos + 50;
            b4x3 = x_pos + 25;
            b4y1 = 575;
            b4y2 = 600;
            brickActiveStatus[3] = 1;
            break;
    }
}

// Function to update game difficulty based on score
void update_difficulty() {
    // Increase speed factor every few points
    if (count > lastSpeedIncrease + speedIncreaseInterval) {
        speedFactor += 0.2; // 20% speed increase
        lastSpeedIncrease = count;
        
        // If score is high enough, decrease respawn delay
        if (count > 20) {
            respawnDelay = 80;  // Faster respawn after score of 20
        }
        if (count > 40) {
            respawnDelay = 60;  // Even faster respawn after score of 40
        }
    }
}

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
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            if (bullets[i].y < 600)
                bullets[i].y += 5.000; // Keeping the faster bullet speed
            if (bullets[i].y >= 600) {
                bullets[i].y = 25;
                bullets[i].active = 0; // Reset bullet state
                activeBulletCount--;
            }
        }
    }
    
    glutPostRedisplay();
    
    // Rest of collision detection code remains the same
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            if (bullets[i].x == b1x3 && bullets[i].y > b1y1 && bullets[i].y < b1y2) {   
                if (b1x2 < 600) {
                    b1y1 = 675;
                    b1x1 = b1x1 + 50;
                    b1x2 = b1x2 + 50;
                    b1x3 = b1x3 + 50;
                    b1y2 = 700;
                    count++;
                } else {
                    b1x1 = 0;
                    b1x2 = 50;
                    b1x3 = 25;
                    b1y1 = 575;
                    b1y2 = 600;	
                }
                bullets[i].active = 0;
                activeBulletCount--;
                bullets[i].y = 25;
                live_score();
            }
            if (bullets[i].x == b2x3 && bullets[i].y > b2y1 && bullets[i].y < b2y2) {	
                if (b2x2 > 0) {
                    b2y1 = 675;
                    b2x1 = b2x1 - 50;
                    b2x2 = b2x2 - 50;
                    b2x3 = b2x3 - 50;
                    b2y2 = 700;
                    count++;
                } else {
                    b2x1 = 500;
                    b2x2 = 550;
                    b2x3 = 525;
                    b2y1 = 575;
                    b2y2 = 600;
                }
                bullets[i].active = 0;
                activeBulletCount--;
                bullets[i].y = 25;
                live_score();
            }
            if (bullets[i].x == b3x3 && bullets[i].y > b3y1 && bullets[i].y < b3y2) {	
                if (b3x2 > 0) {
                    b3y1 = 675;
                    b3x1 = b3x1 - 50;
                    b3x2 = b3x2 - 50;
                    b3x3 = b3x3 - 50;
                    b3y2 = 700;
                    count++;
                } else {
                    b3x1 = 300;
                    b3x2 = 350;
                    b3x3 = 325;
                    b3y1 = 575;
                    b3y2 = 600;
                }	
                bullets[i].active = 0;
                activeBulletCount--;
                bullets[i].y = 25;
                live_score();
            }
            if (bullets[i].x == b4x3 && bullets[i].y > b4y1 && bullets[i].y < b4y2) {
                if (b4x2 < 600) {
                    b4y1 = 675;
                    b4x1 = b4x1 + 50;
                    b4x2 = b4x2 + 50;
                    b4x3 = b4x3 + 50;
                    b4y2 = 700;
                    count++;
                } else {
                    b4x1 = 200;
                    b4x2 = 250;
                    b4x3 = 225;
                    b4y1 = 575;
                    b4y2 = 600;	
                }
                bullets[i].y = 25;
                bullets[i].active = 0;
                activeBulletCount--;
                live_score();
            }
        }
    }
}
void keyb(unsigned char key,int x,int y)
{	
    if(key=='f'||key=='F' || key==' ') 
    {  
        flag2=1;
        flag=1; 
        // Find an inactive bullet and activate it
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (!bullets[i].active) {
                bullets[i].x = br3; // Set bullet x-position to current ship position when fired
                bullets[i].y = 25;
                bullets[i].active = 1;
                activeBulletCount++;
                break;
            }
        }
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
    // Update difficulty based on score
    update_difficulty();
    
    // Brick 1 (Red)
    if (brickActiveStatus[0]) {
        glColor3f(1.0,0.0,0.0);
        glBegin(GL_POLYGON);
            glVertex2f(b1x1,b1y1);
            glVertex2f(b1x1,b1y2);
            glVertex2f(b1x2,b1y2);
            glVertex2f(b1x2,b1y1);	
            glVertex2f(b1x3,b1y1);
        glEnd();
        
        if(b1y1>0) {
            // Apply speed based on difficulty factor
            float currentSpeed = baseSpeed * speedFactor;
            b1y1 = b1y1 - currentSpeed;
            b1y2 = b1y2 - currentSpeed;
        } else {
            // Brick reached bottom
            brickActiveStatus[0] = 0; // Deactivate brick
            respawnTimer = 0;  // Start respawn timer
        }
    } else {
        // Handle respawning
        if (respawnTimer >= respawnDelay) {
            randomize_brick_position(1);
            respawnTimer = 0;
        } else {
            respawnTimer++;
        }
    }

    // Brick 2 (Green)
    if (brickActiveStatus[1]) {
        glColor3f(0.0,1.0,0.0);
        glBegin(GL_POLYGON);
            glVertex2f(b2x1,b2y1);
            glVertex2f(b2x1,b2y2);
            glVertex2f(b2x2,b2y2);
            glVertex2f(b2x2,b2y1);	
            glVertex2f(b2x3,b2y1);
        glEnd();
        
        if(b2y1>0) {
            // Apply speed based on difficulty factor
            float currentSpeed = baseSpeed * 0.8 * speedFactor; // Slightly slower
            b2y1 = b2y1 - currentSpeed;
            b2y2 = b2y2 - currentSpeed;
        } else {
            // Brick reached bottom
            brickActiveStatus[1] = 0; // Deactivate brick
        }
    } else {
        // Handle respawning with slight delay offset
        if (respawnTimer >= respawnDelay + 30) {
            randomize_brick_position(2);
        }
    }

    // Brick 3 (Blue)
    if (brickActiveStatus[2]) {
        glColor3f(0.0,0.0,1.0);
        glBegin(GL_POLYGON);
            glVertex2f(b3x1,b3y1);
            glVertex2f(b3x1,b3y2);
            glVertex2f(b3x2,b3y2);
            glVertex2f(b3x2,b3y1);	
            glVertex2f(b3x3,b3y1);
        glEnd();
        
        if(b3y1>0) {
            // Apply speed based on difficulty factor
            float currentSpeed = baseSpeed * 0.9 * speedFactor;
            b3y1 = b3y1 - currentSpeed;
            b3y2 = b3y2 - currentSpeed;
        } else {
            // Brick reached bottom
            brickActiveStatus[2] = 0; // Deactivate brick
        }
    } else {
        // Handle respawning with slight delay offset
        if (respawnTimer >= respawnDelay + 60) {
            randomize_brick_position(3);
        }
    }

    // Brick 4 (Yellow)
    if (brickActiveStatus[3]) {
        glColor3f(1.0,1.0,0.0);
        glBegin(GL_POLYGON);
            glVertex2f(b4x1,b4y1);
            glVertex2f(b4x1,b4y2);
            glVertex2f(b4x2,b4y2);
            glVertex2f(b4x2,b4y1);	
            glVertex2f(b4x3,b4y1);
        glEnd();
        
        if(b4y1>0) {
            // Apply speed based on difficulty factor
            float currentSpeed = baseSpeed * 1.1 * speedFactor; // Slightly faster
            b4y1 = b4y1 - currentSpeed;
            b4y2 = b4y2 - currentSpeed;
        } else {
            // Brick reached bottom
            brickActiveStatus[3] = 0; // Deactivate brick
            
            // Only trigger game over if all bricks have reached bottom
            if (b1y1 <= 0 && b2y1 <= 0 && b3y1 <= 0) {
                gamestatus();
            }
        }
    } else {
        // Handle respawning with slight delay offset
        if (respawnTimer >= respawnDelay + 90) {
            randomize_brick_position(4);
        }
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
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            glColor3f(0.0,0.0,0.0);
            glPointSize(7);
            glBegin(GL_POINTS);
                glVertex2f(bullets[i].x, bullets[i].y);
            glEnd();
        }
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
       
       // Initialize all bullets to inactive
       for (int i = 0; i < MAX_BULLETS; i++) {
           bullets[i].x = 0;
           bullets[i].y = 25;
           bullets[i].active = 0;
       }
       
       myinit();
       glutDisplayFunc(display);
       glutKeyboardFunc(keyb);
       glutMainLoop();
       return 0;
}