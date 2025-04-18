#include<GL/glut.h>
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>
#include<time.h> // Adding time.h header for the time() function

// Define a structure for bullets
typedef struct {
    GLfloat x;       // X-position
    GLfloat y;       // Y-position
    int active;      // Is bullet active? (1=yes, 0=no)
} Bullet;

// Define a structure for stars in the background
typedef struct {
    GLfloat x;       // X-position
    GLfloat y;       // Y-position
    GLfloat size;    // Star size
    GLfloat brightness; // Star brightness (for twinkling effect)
    GLfloat twinkleSpeed; // How fast the star twinkles
} Star;

#define MAX_BULLETS 10    // Maximum number of bullets allowed on screen
#define MAX_STARS 100     // Number of stars in the background
Bullet bullets[MAX_BULLETS]; // Array to store bullets
Star stars[MAX_STARS];    // Array to store background stars
int activeBulletCount = 0;    // Track how many bullets are currently active

// Space theme colors
GLfloat spaceBlue[] = {0.0, 0.05, 0.2}; // Dark blue space background
GLfloat spaceshipColor[] = {0.8, 0.8, 0.9}; // Silvery spaceship
GLfloat laserColor[] = {1.0, 0.4, 0.2}; // Orange-red laser beams
GLfloat alienColors[4][3] = {
    {0.2, 0.8, 0.2}, // Green alien
    {0.8, 0.2, 0.8}, // Purple alien  
    {0.0, 0.7, 0.8}, // Cyan alien
    {0.9, 0.6, 0.1}  // Orange-gold alien
};
// Trail effect for spaceship
GLfloat engineGlowTimer = 0;

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
int minRequiredBricks = 2;   // Minimum number of bricks that must be active
int lowBrickTimer = 0;       // Timer to track how long we've been below min brick count
int lowBrickThreshold = 200; // How long we can stay below min brick count before game over
int gameOver = 0;            // Flag to track if the game is over

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

// Function to count active bricks and ensure minimum brick count
int count_active_bricks() {
    int active_count = 0;
    for (int i = 0; i < 4; i++) {
        if (brickActiveStatus[i]) {
            active_count++;
        }
    }
    return active_count;
}

// Function to spawn a new brick if needed
void ensure_minimum_bricks() {
    int active_bricks = count_active_bricks();
    
    // If less than 3 bricks are active, try to spawn new ones
    if (active_bricks < 3) {
        // First check which bricks are inactive
        for (int i = 0; i < 4; i++) {
            if (!brickActiveStatus[i]) {
                // Immediately spawn a new brick at this position
                randomize_brick_position(i + 1);
                
                // If we now have at least 2-3 bricks, we can stop
                if (count_active_bricks() >= 3) {
                    break;
                }
            }
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
    // Move all active bullets upward
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            if (bullets[i].y < 600)
                bullets[i].y += 5.000; // Laser beam speed
            if (bullets[i].y >= 600) {
                bullets[i].y = 25;
                bullets[i].active = 0;
                activeBulletCount--;
            }
        }
    }
    
    glutPostRedisplay();
    
    // Improved collision detection with better hitbox for alien ships
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            // Alien 1 collision - expanded hitbox for better gameplay
            if (bullets[i].x >= b1x1 && bullets[i].x <= b1x2 && 
                bullets[i].y >= b1y1 && bullets[i].y <= b1y2 && brickActiveStatus[0]) {
                
                // Visual explosion effect will be added in draw_aliens
                
                // Reset alien position after hit
                if (b1x2 < 600) {
                    b1y1 = 575;
                    b1x1 = (rand() % 11) * 50;
                    b1x2 = b1x1 + 50;
                    b1x3 = b1x1 + 25;
                    b1y2 = 600;
                    count++;
                } else {
                    b1x1 = 0;
                    b1x2 = 50;
                    b1x3 = 25;
                    b1y1 = 575;
                    b1y2 = 600;	
                }
                
                // Reset bullet
                bullets[i].active = 0;
                activeBulletCount--;
                bullets[i].y = 25;
                live_score();
            }
            
            // Alien 2 collision
            if (bullets[i].x >= b2x1 && bullets[i].x <= b2x2 && 
                bullets[i].y >= b2y1 && bullets[i].y <= b2y2 && brickActiveStatus[1]) {
                
                // Reset alien position after hit
                if (b2x2 > 0) {
                    b2y1 = 575;
                    b2x1 = (rand() % 11) * 50;
                    b2x2 = b2x1 + 50;
                    b2x3 = b2x1 + 25;
                    b2y2 = 600;
                    count++;
                } else {
                    b2x1 = 500;
                    b2x2 = 550;
                    b2x3 = 525;
                    b2y1 = 575;
                    b2y2 = 600;
                }
                
                // Reset bullet
                bullets[i].active = 0;
                activeBulletCount--;
                bullets[i].y = 25;
                live_score();
            }
            
            // Alien 3 collision
            if (bullets[i].x >= b3x1 && bullets[i].x <= b3x2 && 
                bullets[i].y >= b3y1 && bullets[i].y <= b3y2 && brickActiveStatus[2]) {
                
                // Reset alien position after hit
                if (b3x2 > 0) {
                    b3y1 = 575;
                    b3x1 = (rand() % 11) * 50;
                    b3x2 = b3x1 + 50;
                    b3x3 = b3x1 + 25;
                    b3y2 = 600;
                    count++;
                } else {
                    b3x1 = 300;
                    b3x2 = 350;
                    b3x3 = 325;
                    b3y1 = 575;
                    b3y2 = 600;
                }	
                
                // Reset bullet
                bullets[i].active = 0;
                activeBulletCount--;
                bullets[i].y = 25;
                live_score();
            }
            
            // Alien 4 collision
            if (bullets[i].x >= b4x1 && bullets[i].x <= b4x2 && 
                bullets[i].y >= b4y1 && bullets[i].y <= b4y2 && brickActiveStatus[3]) {
                
                // Reset alien position after hit
                if (b4x2 < 600) {
                    b4y1 = 575;
                    b4x1 = (rand() % 11) * 50;
                    b4x2 = b4x1 + 50;
                    b4x3 = b4x1 + 25;
                    b4y2 = 600;
                    count++;
                } else {
                    b4x1 = 200;
                    b4x2 = 250;
                    b4x3 = 225;
                    b4y1 = 575;
                    b4y2 = 600;	
                }
                
                // Reset bullet
                bullets[i].active = 0;
                activeBulletCount--;
                bullets[i].y = 25;
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

// Function to draw and animate stars
void draw_stars() {
    // Update star twinkle effect
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].brightness += stars[i].twinkleSpeed;
        if (stars[i].brightness > 1.0) {
            stars[i].brightness = 0.0;
            // Occasionally move stars for subtle movement effect
            if (rand() % 20 == 0) {
                stars[i].y -= 0.5;
                if (stars[i].y < 0) {
                    stars[i].y = 600; // Wrap around to top
                    stars[i].x = rand() % 600;
                }
            }
        }
        
        // Draw star with current brightness
        float brightness = 0.5 + stars[i].brightness * 0.5; // Range from 0.5 to 1.0
        glColor3f(brightness, brightness, brightness);
        glPointSize(stars[i].size);
        glBegin(GL_POINTS);
            glVertex2f(stars[i].x, stars[i].y);
        glEnd();
    }
}

// Function to draw spaceship with engine glow
void draw_spaceship() {
    // Update engine glow effect
    engineGlowTimer += 0.05;
    if (engineGlowTimer > 1.0) engineGlowTimer = 0.0;
    
    // Engine glow effect (changes with time) - draw this FIRST so it appears behind the ship
    float glowIntensity = 0.5 + 0.5 * sin(engineGlowTimer * 10.0);
    glColor3f(1.0, 0.4 * glowIntensity, 0.1 * glowIntensity);
    glBegin(GL_TRIANGLES);
        glVertex2f(br3 - 15, 5);
        glVertex2f(br3, -15 - (glowIntensity * 10)); // Larger engine flame
        glVertex2f(br3 + 15, 5);
    glEnd();
    
    // Main spaceship body - more futuristic design
    glColor3f(spaceshipColor[0], spaceshipColor[1], spaceshipColor[2]);
    
    // Main hull
    glBegin(GL_POLYGON);
       glVertex2f(br1 + 5, 0);	
       glVertex2f(br2 - 5, 20);
       glVertex2f(br3, 30); // Taller ship
       glVertex2f(br4 + 5, 20);
       glVertex2f(br5 - 5, 0);
    glEnd();
    
    // Side wings
    glColor3f(0.7, 0.7, 0.9);
    glBegin(GL_TRIANGLES);
        // Left wing
        glVertex2f(br1, 0);
        glVertex2f(br1 + 10, 10);
        glVertex2f(br3 - 10, 0);
        // Right wing
        glVertex2f(br5, 0);
        glVertex2f(br5 - 10, 10);
        glVertex2f(br3 + 10, 0);
    glEnd();
    
    // Cockpit - glowing blue
    glColor3f(0.2, 0.4, 1.0);
    glBegin(GL_POLYGON);
        glVertex2f(br3 - 8, 15);
        glVertex2f(br3 - 5, 25);
        glVertex2f(br3 + 5, 25);
        glVertex2f(br3 + 8, 15);
    glEnd();
    
    // Weapon mounts
    glColor3f(0.5, 0.5, 0.5);
    glPointSize(5.0);
    glBegin(GL_POINTS);
        glVertex2f(br3 - 15, 10);
        glVertex2f(br3 + 15, 10);
    glEnd();
    
    // Add glowing effect for shield or energy field
    if (rand() % 20 == 0) {  // Occasionally show shield flicker
        glColor4f(0.2, 0.7, 1.0, 0.3);
        glBegin(GL_LINE_LOOP);
            for (int i = 0; i < 20; i++) {
                float angle = i * 18.0 * 3.14159 / 180.0;
                glVertex2f(br3 + cos(angle) * 35, 15 + sin(angle) * 35);
            }
        glEnd();
    }
}

// Function to draw aliens with more distinctive space theme
void draw_aliens() {
    // Update difficulty based on score
    update_difficulty();
    
    // Check if game is already over
    if (gameOver) {
        gamestatus();
        return;
    }
    
    // Count how many bricks are currently active
    int activeBrickCount = count_active_bricks();
    
    // Check if we need to take action based on brick count
    if (activeBrickCount < minRequiredBricks) {
        lowBrickTimer++; 
        if (lowBrickTimer > 30) { 
            ensure_minimum_bricks();
            lowBrickTimer = 0; 
        }
        if (lowBrickTimer > lowBrickThreshold) {
            gameOver = 1;
            gamestatus();
            return;
        }
    } else {
        lowBrickTimer = 0;
    }
    
    // ALIEN 1: Insectoid alien ship with pulsing eyes
    if (brickActiveStatus[0]) {
        // Pulsing effect for alien tech
        float pulse = 0.7 + 0.3 * sin(engineGlowTimer * 15.0);
        
        // Main body - bright green, larger
        glColor3f(alienColors[0][0] * pulse, alienColors[0][1] * pulse, alienColors[0][2] * pulse);
        glBegin(GL_POLYGON);
            glVertex2f(b1x1 + 5, b1y1);
            glVertex2f(b1x1, b1y1 + 15);
            glVertex2f(b1x3 - 10, b1y2);
            glVertex2f(b1x3 + 10, b1y2);
            glVertex2f(b1x2, b1y1 + 15);
            glVertex2f(b1x2 - 5, b1y1);
        glEnd();
        
        // Insectoid head
        glBegin(GL_TRIANGLES);
            glVertex2f(b1x3 - 10, b1y1 + 15);
            glVertex2f(b1x3, b1y1 + 30); // Tall pointy head
            glVertex2f(b1x3 + 10, b1y1 + 15);
        glEnd();
        
        // Alien mandibles
        glLineWidth(2.0);
        glBegin(GL_LINES);
            glVertex2f(b1x3 - 10, b1y1 + 15);
            glVertex2f(b1x3 - 20, b1y1 + 20);
            glVertex2f(b1x3 + 10, b1y1 + 15);
            glVertex2f(b1x3 + 20, b1y1 + 20);
        glEnd();
        
        // Glowing eyes - pulsing red
        glColor3f(1.0 * pulse, 0.0, 0.0); 
        glPointSize(6.0); // Larger eyes
        glBegin(GL_POINTS);
            glVertex2f(b1x3 - 6, b1y1 + 20);
            glVertex2f(b1x3 + 6, b1y1 + 20);
        glEnd();
        
        // Movement
        if(b1y1 > 0) {
            float currentSpeed = baseSpeed * speedFactor;
            b1y1 = b1y1 - currentSpeed;
            b1y2 = b1y2 - currentSpeed;
        } else {
            // Alien reached bottom - GAME OVER!
            gameOver = 1;
            gamestatus();
            return;
        }
    } else {
        if (respawnTimer >= respawnDelay) {
            randomize_brick_position(1);
            respawnTimer = 0;
        } else {
            respawnTimer++;
        }
    }

    // ALIEN 2: Crystalline space entity
    if (brickActiveStatus[1]) {
        // Draw crystalline entity
        glColor3f(alienColors[1][0], alienColors[1][1], alienColors[1][2]);
        
        // Crystal body
        glBegin(GL_POLYGON);
            glVertex2f(b2x3, b2y1); // Bottom point
            glVertex2f(b2x1 + 10, b2y1 + 15);
            glVertex2f(b2x3 - 10, b2y2);
            glVertex2f(b2x3 + 10, b2y2);
            glVertex2f(b2x2 - 10, b2y1 + 15);
        glEnd();
        
        // Crystal spikes
        glBegin(GL_TRIANGLES);
            // Left spike
            glVertex2f(b2x1 + 10, b2y1 + 15);
            glVertex2f(b2x1, b2y1 + 25);
            glVertex2f(b2x3 - 10, b2y2);
            // Right spike
            glVertex2f(b2x2 - 10, b2y1 + 15);
            glVertex2f(b2x2, b2y1 + 25);
            glVertex2f(b2x3 + 10, b2y2);
            // Top spike
            glVertex2f(b2x3 - 10, b2y2);
            glVertex2f(b2x3, b2y2 + 15);
            glVertex2f(b2x3 + 10, b2y2);
        glEnd();
        
        // Energy core - pulsing white center
        float pulse = 0.5 + 0.5 * sin(engineGlowTimer * 20.0);
        glColor3f(pulse, pulse, pulse);
        glBegin(GL_POLYGON);
            for (int i = 0; i < 8; i++) {
                float angle = i * 45.0 * 3.14159 / 180.0;
                glVertex2f(b2x3 + cos(angle) * 8, b2y1 + 15 + sin(angle) * 8);
            }
        glEnd();
        
        // Movement
        if(b2y1 > 0) {
            float currentSpeed = baseSpeed * 0.8 * speedFactor;
            b2y1 = b2y1 - currentSpeed;
            b2y2 = b2y2 - currentSpeed;
        } else {
            // Alien reached bottom - GAME OVER!
            gameOver = 1;
            gamestatus();
            return;
        }
    } else {
        if (respawnTimer >= respawnDelay/2) {
            randomize_brick_position(2);
        }
    }

    // ALIEN 3: Classic UFO with enhanced lighting
    if (brickActiveStatus[2]) {
        // UFO saucer shape - bigger
        glColor3f(alienColors[2][0], alienColors[2][1], alienColors[2][2]);
        glBegin(GL_POLYGON);
            for (int i = 0; i < 12; i++) {
                float angle = i * 30.0 * 3.14159 / 180.0;
                if (angle > 3.14159) { // Bottom half flatter
                    glVertex2f(b3x3 + cos(angle) * 30, b3y1 + 15 + sin(angle) * 10);
                } else { // Top half rounder
                    glVertex2f(b3x3 + cos(angle) * 30, b3y1 + 15 + sin(angle) * 20);
                }
            }
        glEnd();
        
        // Top dome - transparent
        glColor4f(0.8, 0.8, 1.0, 0.6);
        glBegin(GL_POLYGON);
            for (int i = 0; i < 10; i++) {
                float angle = i * 36.0 * 3.14159 / 180.0;
                glVertex2f(b3x3 + cos(angle) * 15, b3y1 + 25 + sin(angle) * 15);
            }
        glEnd();
        
        // Running lights - rotating
        float lightPos = engineGlowTimer * 12.0;
        glPointSize(5.0);
        for (int i = 0; i < 8; i++) {
            float angle = (i * 45.0 + lightPos) * 3.14159 / 180.0;
            float r = 0.0, g = 0.0, b = 0.0;
            // Alternate colors
            switch(i % 4) {
                case 0: r = 1.0; break; // Red
                case 1: g = 1.0; break; // Green
                case 2: b = 1.0; break; // Blue
                case 3: r = 1.0; g = 1.0; break; // Yellow
            }
            glColor3f(r, g, b);
            glBegin(GL_POINTS);
                glVertex2f(b3x3 + cos(angle) * 25, b3y1 + 15 + sin(angle) * 12);
            glEnd();
        }
        
        // Tractor beam - occasional
        if (rand() % 30 == 0) {
            glColor4f(0.5, 1.0, 0.5, 0.3);
            glBegin(GL_TRIANGLES);
                glVertex2f(b3x3, b3y1 + 5);
                glVertex2f(b3x3 - 20, b3y1 - 40);
                glVertex2f(b3x3 + 20, b3y1 - 40);
            glEnd();
        }
        
        // Movement
        if(b3y1 > 0) {
            float currentSpeed = baseSpeed * 0.9 * speedFactor;
            b3y1 = b3y1 - currentSpeed;
            b3y2 = b3y2 - currentSpeed;
        } else {
            // Alien reached bottom - GAME OVER!
            gameOver = 1;
            gamestatus();
            return;
        }
    } else {
        if (respawnTimer >= respawnDelay/2 + 30) {
            randomize_brick_position(3);
        }
    }

    // ALIEN 4: Large battlecruiser
    if (brickActiveStatus[3]) {
        // Main hull - larger
        glColor3f(alienColors[3][0], alienColors[3][1], alienColors[3][2]);
        glBegin(GL_POLYGON);
            glVertex2f(b4x1, b4y1 + 15);
            glVertex2f(b4x1 + 10, b4y1 + 25);
            glVertex2f(b4x2 - 10, b4y1 + 25);
            glVertex2f(b4x2, b4y1 + 15);
        glEnd();
        
        // Command tower
        glBegin(GL_POLYGON);
            glVertex2f(b4x3 - 10, b4y1 + 25);
            glVertex2f(b4x3 - 5, b4y2 + 10); // Taller
            glVertex2f(b4x3 + 5, b4y2 + 10);
            glVertex2f(b4x3 + 10, b4y1 + 25);
        glEnd();
        
        // Side wings
        glBegin(GL_TRIANGLES);
            // Left wing
            glVertex2f(b4x1, b4y1 + 15);
            glVertex2f(b4x1 - 15, b4y1 + 5);
            glVertex2f(b4x1 + 15, b4y1 + 15);
            // Right wing
            glVertex2f(b4x2, b4y1 + 15);
            glVertex2f(b4x2 + 15, b4y1 + 5);
            glVertex2f(b4x2 - 15, b4y1 + 15);
        glEnd();
        
        // Engine glow - pulsing
        float pulse = 0.5 + 0.5 * sin(engineGlowTimer * 8.0);
        glColor3f(1.0, 0.4 * pulse, 0.0);
        glBegin(GL_QUADS);
            glVertex2f(b4x3 - 20, b4y1);
            glVertex2f(b4x3 - 15, b4y1 + 5);
            glVertex2f(b4x3 - 10, b4y1 + 5);
            glVertex2f(b4x3 - 5, b4y1);
            
            glVertex2f(b4x3 + 5, b4y1);
            glVertex2f(b4x3 + 10, b4y1 + 5);
            glVertex2f(b4x3 + 15, b4y1 + 5);
            glVertex2f(b4x3 + 20, b4y1);
        glEnd();
        
        // Weapon beams - charging and firing effect
        float beamCharge = engineGlowTimer * 2.0;
        if (beamCharge > 1.0) beamCharge = 2.0 - beamCharge; // oscillate between 0-1
        
        if (beamCharge > 0.7) { // Only show beam when highly charged
            glColor4f(1.0, 0.0, 0.0, beamCharge);
            glLineWidth(3.0 * beamCharge);
            glBegin(GL_LINES);
                // Left cannon
                glVertex2f(b4x3 - 15, b4y1 + 15);
                glVertex2f(b4x3 - 15, b4y1 - 30 * beamCharge);
                // Right cannon
                glVertex2f(b4x3 + 15, b4y1 + 15);
                glVertex2f(b4x3 + 15, b4y1 - 30 * beamCharge);
            glEnd();
        }
        
        // Bridge windows
        glColor3f(0.8, 0.8, 0.2); // Yellow windows
        glPointSize(2.0);
        glBegin(GL_POINTS);
            for (int i = -3; i <= 3; i++) {
                glVertex2f(b4x3 + i*3, b4y1 + 30);
            }
            for (int i = -2; i <= 2; i++) {
                glVertex2f(b4x3 + i*3, b4y1 + 35);
            }
        glEnd();
        
        // Movement
        if(b4y1 > 0) {
            float currentSpeed = baseSpeed * 1.1 * speedFactor;
            b4y1 = b4y1 - currentSpeed;
            b4y2 = b4y2 - currentSpeed;
        } else {
            // Alien reached bottom - GAME OVER!
            gameOver = 1;
            gamestatus();
            return;
        }
    } else {
        if (respawnTimer >= respawnDelay/2 + 60) {
            randomize_brick_position(4);
        }
    }
    
    // Ensure minimum brick count
    if (count_active_bricks() < minRequiredBricks) {
        for (int i = 0; i < 4; i++) {
            if (!brickActiveStatus[i]) {
                randomize_brick_position(i + 1);
                break;
            }
        }
    }
    
    glutPostRedisplay();
}

// Function to draw laser beams (bullets)
void draw_lasers() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            // Draw laser beam
            glColor3f(laserColor[0], laserColor[1], laserColor[2]);
            
            // Draw a line for the laser beam
            glLineWidth(2.0);
            glBegin(GL_LINES);
                glVertex2f(bullets[i].x, bullets[i].y);
                glVertex2f(bullets[i].x, bullets[i].y + 10);
            glEnd();
            
            // Add a glow effect around the laser
            glPointSize(5.0);
            glColor4f(laserColor[0], laserColor[1], laserColor[2], 0.5);
            glBegin(GL_POINTS);
                glVertex2f(bullets[i].x, bullets[i].y + 5);
            glEnd();
        }
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    if(flag2==0) {
        // Space-themed start screen
        glClearColor(spaceBlue[0], spaceBlue[1], spaceBlue[2], 1.0);
        
        // Draw some stars in the background
        for (int i = 0; i < MAX_STARS/2; i++) {
            float brightness = 0.5 + 0.5 * ((float)rand() / RAND_MAX);
            glColor3f(brightness, brightness, brightness);
            glPointSize(0.5 + 2.0 * ((float)rand() / RAND_MAX));
            glBegin(GL_POINTS);
                glVertex2f(rand() % 600, rand() % 600);
            glEnd();
        }
        
        // Draw a colorful space nebula in the background
        glColor4f(0.5, 0.0, 0.5, 0.2);
        glBegin(GL_POLYGON);
            glVertex2f(0, 0);
            glVertex2f(200, 100);
            glVertex2f(400, 50);
            glVertex2f(600, 200);
            glVertex2f(600, 0);
        glEnd();
        
        glColor3f(1.0, 1.0, 1.0);
        drawstring(100, 550, "COMPUTER GRAPHICS MINI PROJECT");
        drawstring(75, 450, "Team Name:");
        drawstring(250, 450, "Spirit");
        drawstring(75, 500, "Game Name:");
        drawstring(250, 500, "Space Invaders");
        drawstring(75, 400, "By:");
        drawstring(50, 350, "Your Name");
        drawstring(150, 250, "GUIDE:");
        drawstring(150, 200, "Guide Name");
        drawstring(150, 60, "Academic Year");
        drawstring(250, 20, "press 'f' or space to continue");
        glFlush();
    }
    
    else if(flag2==1) {
        if(flag1==0) {
            // Space-themed instructions screen
            glClearColor(spaceBlue[0], spaceBlue[1], spaceBlue[2], 1.0);
            
            // More stars
            for (int i = 0; i < MAX_STARS/2; i++) {
                float brightness = 0.5 + 0.5 * ((float)rand() / RAND_MAX);
                glColor3f(brightness, brightness, brightness);
                glPointSize(0.5 + 2.0 * ((float)rand() / RAND_MAX));
                glBegin(GL_POINTS);
                    glVertex2f(rand() % 600, rand() % 600);
                glEnd();
            }
            
            glColor3f(1.0, 1.0, 1.0);
            drawstring(250, 550, "INSTRUCTIONS");
            drawstring(100, 450, "press A to move left");
            drawstring(100, 400, "press D to move right");
            drawstring(100, 350, "press F or SPACE to fire lasers");
            drawstring(100, 300, "press Q to exit");
            drawstring(150, 20, "press 'n' to continue");
            glFlush();
        }
        
        else if(flag1==1) {
            // Main gameplay screen
            glClearColor(spaceBlue[0], spaceBlue[1], spaceBlue[2], 1.0);
            
            // Draw stars
            draw_stars();
            
            // Draw spaceship
            draw_spaceship();
            
            // Draw laser beams
            draw_lasers();
            
            // Draw aliens (formerly bricks)
            draw_aliens();
            
            // Show score with space theme
            glPushMatrix();
            glLoadIdentity();
            glRasterPos2f(10, 550);
            glColor3f(0.7, 0.9, 0.7); // Light green futuristic text
            char message[100] = {0};
            sprintf(message, "ALIEN KILLS: %d", count);
            int len = (int)strlen(message);
            for (int i = 0; i < len; i++) {
                glutBitmapCharacter(GLUT_BITMAP_8_BY_13, message[i]);
            }
            glPopMatrix();
            
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
  glClearColor(spaceBlue[0], spaceBlue[1], spaceBlue[2], 1.0);
  
  // Draw stars in background of game over screen
  for (int i = 0; i < MAX_STARS; i++) {
      float brightness = 0.5 + 0.5 * ((float)rand() / RAND_MAX);
      glColor3f(brightness, brightness, brightness);
      glPointSize(0.5 + 2.0 * ((float)rand() / RAND_MAX));
      glBegin(GL_POINTS);
          glVertex2f(rand() % 600, rand() % 600);
      glEnd();
  }
  
  // Draw large exploding spaceship
  float centerX = 300;
  float centerY = 300;
  
  // Draw explosion rays
  for (int i = 0; i < 12; i++) {
      float angle = i * 30.0 * 3.14159 / 180.0;
      float length = 50.0 + (rand() % 50);
      
      glColor3f(1.0, 0.5, 0.0); // Orange-red explosion color
      glLineWidth(2.0 + (rand() % 3));
      glBegin(GL_LINES);
          glVertex2f(centerX, centerY);
          glVertex2f(centerX + cos(angle) * length, centerY + sin(angle) * length);
      glEnd();
  }
  
  // Draw explosion core
  glColor3f(1.0, 0.8, 0.2);
  glBegin(GL_POLYGON);
      for (int i = 0; i < 16; i++) {
          float angle = i * 22.5 * 3.14159 / 180.0;
          glVertex2f(centerX + cos(angle) * 30, centerY + sin(angle) * 30);
      }
  glEnd();
  
  // Game over text with space theme
  glColor3f(1.0, 0.0, 0.0);
  drawstring(150, 480, "MISSION FAILED");
  
  glColor3f(1.0, 1.0, 1.0);
  glRasterPos2f(180, 400);
  sprintf(tmp_str, "ALIEN KILLS: %d", count);
  Write(tmp_str);
  
  glColor3f(0.7, 0.7, 1.0);
  drawstring(120, 200, "Earth has been invaded!");
  
  glColor3f(1.0, 1.0, 0.0);
  drawstring(150, 100, "Press Q to exit");
}

int main(int argc,char** argv)
{
       glutInit(&argc,argv);
       glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
       glutInitWindowSize(600,600);
       glutInitWindowPosition(0,0);
       glutCreateWindow("Space Invaders");
       
       // Initialize random seed
       srand(time(NULL));
       
       // Initialize all bullets to inactive
       for (int i = 0; i < MAX_BULLETS; i++) {
           bullets[i].x = 0;
           bullets[i].y = 25;
           bullets[i].active = 0;
       }
       
       // Initialize stars with random positions and properties
       for (int i = 0; i < MAX_STARS; i++) {
           stars[i].x = rand() % 600;
           stars[i].y = rand() % 600;
           stars[i].size = 0.5 + (rand() % 20) / 10.0; // Random size between 0.5 and 2.5
           stars[i].brightness = (rand() % 100) / 100.0; // Random initial brightness
           stars[i].twinkleSpeed = 0.005 + (rand() % 10) / 1000.0; // Random twinkle speed
       }
       
       myinit();
       glutDisplayFunc(display);
       glutKeyboardFunc(keyb);
       glutMainLoop();
       return 0;
}