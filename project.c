#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define M_PI 3.14159265358979323846
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct {
GLfloat r, g, b;
} Color;

Color COLOR_RED = {1.0f, 0.0f, 0.0f};
Color COLOR_GREEN = {0.0f, 1.0f, 0.0f};
Color COLOR_BLUE = {0.0f, 0.0f, 1.0f};
Color COLOR_YELLOW = {1.0f, 1.0f, 0.0f};
Color COLOR_PURPLE = {0.8f, 0.2f, 0.8f};
Color COLOR_CYAN = {0.0f, 1.0f, 1.0f};
Color COLOR_ORANGE = {1.0f, 0.6f, 0.0f};
Color COLOR_WHITE = {1.0f, 1.0f, 1.0f};
Color COLOR_BLACK = {0.0f, 0.0f, 0.0f};

float randomFloat(float min, float max) {
return min + ((float)rand() / RAND_MAX) * (max - min);
}

void setColor(Color color) {
glColor3f(color.r, color.g, color.b);
}

// Draw string at specified position
void drawString(float x, float y, const char* string, void* font) {
    glRasterPos2f(x, y);
    for (const char* c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#include "stb_image.h"
#define STBI_rgb_alpha 4

// Game states
#define SPLASH_STATE -1
#define MENU_STATE 0
#define INSTRUCTIONS_STATE 1
#define PLAYING_STATE 2
#define GAME_OVER_STATE 3
#define LEVEL_COMPLETE_STATE 4

typedef struct {
    GLfloat x, y;
    GLfloat width, height;
    GLfloat speed;
    Color color;
    int active;
    int health;
} GameObject;

typedef struct {
    GameObject base;
    int type;  // 0: normal, 1: double damage, 2: fast
    float angle;
} Bullet;

typedef struct {
    GameObject base;
    int type;  // 0: standard, 1: armored, 2: boss
    float movePattern;
    int points;
} Enemy;

typedef struct {
    GameObject base;
    int type;  // 0: extra life, 1: shield, 2: rapid fire, 3: multi-shot
    float rotationAngle;
} PowerUp;

// Game variables
int gameState = MENU_STATE;
int score = 0;
int lives = 3;
int level = 1;
int maxLevel = 20;
int enemiesKilled = 0;
int enemiesRequired = 0;
float difficultyMultiplier = 1.0f;
int isPaused = 0;

// Player variables
GameObject player;
int isMovingLeft = 0;
int isMovingRight = 0;
int isShooting = 0;
int hasShield = 0;
int hasRapidFire = 0;
int hasMultiShot = 0;
int shootCooldown = 0;
int invincibilityFrames = 0;

#define MAX_BULLETS 50
Bullet bullets[MAX_BULLETS];

#define MAX_ENEMIES 30
Enemy enemies[MAX_ENEMIES];

#define MAX_POWERUPS 5
PowerUp powerUps[MAX_POWERUPS];

#define MAX_STARS 100
typedef struct {
    float x, y;
    float brightness;
    float speed;
} Star;
Star stars[MAX_STARS];

int lastFrameTime = 0;
float deltaTime = 0.0f;

GLuint splashTexture;

void drawPlayer();
void drawBullets();
void drawEnemies();
void drawPowerUps();
void drawStars();
void drawHUD();
void drawLives();
void drawExplosion(float x, float y, float size, Color color);
void spawnEnemy();
void spawnPowerUp(float x, float y);
void checkCollisions();
void updateGameState();
void resetGame();
void setupLevel(int level);
void drawMenu();
void drawInstructions();
void drawGameOver();
void drawLevelComplete();
void drawSplashText();

GLuint loadTexture(const char* filename) {
    GLuint texture;
    int width, height, channels;
    
    unsigned char* image = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
    if (!image) {
        printf("Failed to load texture: %s\n", filename);
        return 0;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);
    return texture;
}

void initGame() {
    srand(time(NULL));
    
    player.x = WINDOW_WIDTH / 2;
    player.y = 50;
    player.width = 40;
    player.height = 30;
    player.speed = 300.0f;
    player.color = COLOR_CYAN;
    player.active = 1;
    player.health = 100;
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].base.active = 0;
    }
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].base.active = 0;
    }
    
    for (int i = 0; i < MAX_POWERUPS; i++) {
        powerUps[i].base.active = 0;
    }
    
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].x = randomFloat(0, WINDOW_WIDTH);
        stars[i].y = randomFloat(0, WINDOW_HEIGHT);
        stars[i].brightness = randomFloat(0.2f, 1.0f);
        stars[i].speed = randomFloat(10.0f, 50.0f);
    }
    
    setupLevel(level);
}

void setupLevel(int level) {
    enemiesKilled = 0;
    enemiesRequired = 10 + (level * 5);
    difficultyMultiplier = 1.0f + (level * 0.1f);
    
    // Clear all enemies and powerups
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].base.active = 0;
    }
    
    for (int i = 0; i < MAX_POWERUPS; i++) {
        powerUps[i].base.active = 0;
    }
    
    // Spawn initial enemies for this level
    for (int i = 0; i < 5 + level; i++) {
        spawnEnemy();
    }
}

void resetGame() {
    score = 0;
    lives = 3;
    level = 1;
    hasShield = 0;
    hasRapidFire = 0;
    hasMultiShot = 0;
    invincibilityFrames = 0;
    
    initGame();
}

void spawnEnemy() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].base.active) {
            enemies[i].base.active = 1;
            enemies[i].base.x = randomFloat(40, WINDOW_WIDTH - 40);
            enemies[i].base.y = WINDOW_HEIGHT + randomFloat(10, 50);
            
            // Determine enemy type with increasing difficulty
            float typeRoll = randomFloat(0, 1);
            if (typeRoll < 0.1f * level) {
                // Boss type
                enemies[i].type = 2;
                enemies[i].base.width = 60;
                enemies[i].base.height = 60;
                enemies[i].base.speed = 30.0f * difficultyMultiplier;
                enemies[i].base.health = 5;
                enemies[i].base.color = COLOR_RED;
                enemies[i].points = 230;
            } else if (typeRoll < 0.3f * level) {
                // Armored type
                enemies[i].type = 1;
                enemies[i].base.width = 40;
                enemies[i].base.height = 40;
                enemies[i].base.speed = 50.0f * difficultyMultiplier;
                enemies[i].base.health = 3;
                enemies[i].base.color = COLOR_ORANGE;
                enemies[i].points = 100;
            } else {
                // Standard type
                enemies[i].type = 0;
                enemies[i].base.width = 30;
                enemies[i].base.height = 30;
                enemies[i].base.speed = 70.0f * difficultyMultiplier;
                enemies[i].base.health = 1;
                enemies[i].base.color = COLOR_GREEN;
                enemies[i].points = 50;
            }
            
            enemies[i].movePattern = randomFloat(0, 2*M_PI);
            return;
        }
    }
}

void spawnPowerUp(float x, float y) {
    if (randomFloat(0, 1) > 0.3f) return;
    
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerUps[i].base.active) {
            powerUps[i].base.active = 1;
            powerUps[i].base.x = x;
            powerUps[i].base.y = y;
            powerUps[i].base.width = 20;
            powerUps[i].base.height = 20;
            powerUps[i].base.speed = 80.0f;
            
            powerUps[i].type = rand() % 4;
            
            switch (powerUps[i].type) {
                case 0: // Extra life
                    powerUps[i].base.color = COLOR_RED;
                    break;
                case 1: // Shield
                    powerUps[i].base.color = COLOR_BLUE;
                    break;
                case 2: // Rapid fire
                    powerUps[i].base.color = COLOR_YELLOW;
                    break;
                case 3: // Multi-shot
                    powerUps[i].base.color = COLOR_PURPLE;
                    break;
            }
            
            powerUps[i].rotationAngle = 0;
            return;
        }
    }
}

void fireBullet(int type, float angleOffset) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].base.active) {
            bullets[i].base.active = 1;
            bullets[i].base.x = player.x;
            bullets[i].base.y = player.y + player.height / 2;
            bullets[i].base.width = 5;
            bullets[i].base.height = 15;
            bullets[i].type = type;
            bullets[i].angle = angleOffset;
            
            switch (type) {
                case 0: // Normal bullet
                    bullets[i].base.speed = 400.0f;
                    bullets[i].base.color = COLOR_WHITE;
                    break;
                case 1: // Double damage bullet
                    bullets[i].base.speed = 400.0f;
                    bullets[i].base.color = COLOR_RED;
                    bullets[i].base.width = 8;
                    break;
                case 2: // Fast bullet
                    bullets[i].base.speed = 600.0f;
                    bullets[i].base.color = COLOR_YELLOW;
                    break;
            }
            return;
        }
    }
}

void drawLevelComplete() {
    drawStars();
    setColor(COLOR_GREEN);
    drawString(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT - 200, "LEVEL COMPLETE!", GLUT_BITMAP_TIMES_ROMAN_24);
    
    // Draw level info
    setColor(COLOR_WHITE);
    char buffer[64];
    sprintf(buffer, "Level %d Completed!", level);
    drawString(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 30, buffer, GLUT_BITMAP_HELVETICA_18);
    
    sprintf(buffer, "Current Score: %d", score);
    drawString(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2, buffer, GLUT_BITMAP_HELVETICA_18);
    
    sprintf(buffer, "Next Level: %d", level + 1);
    drawString(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 30, buffer, GLUT_BITMAP_HELVETICA_18);
    
    setColor(COLOR_YELLOW);
    drawString(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 80, "Press SPACE to continue", GLUT_BITMAP_HELVETICA_18);
}

void updateGameState() {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    deltaTime = (currentTime - lastFrameTime) / 1000.0f;
    lastFrameTime = currentTime;
    
    if (isPaused) return;
    
    if (isMovingLeft && player.x > player.width / 2) {
        player.x -= player.speed * deltaTime;
    }
    if (isMovingRight && player.x < WINDOW_WIDTH - player.width / 2) {
        player.x += player.speed * deltaTime;
    }
    
    if (isShooting && shootCooldown <= 0) {
        if (hasMultiShot) {
            fireBullet(0, -0.2f);
            fireBullet(0, 0.0f);
            fireBullet(0, 0.2f);
        } else {
            fireBullet(hasRapidFire ? 2 : 0, 0.0f);
        }
        
        shootCooldown = hasRapidFire ? 10 : 20;
    }
    
    if (shootCooldown > 0) {
        shootCooldown--;
    }
    
    if (invincibilityFrames > 0) {
        invincibilityFrames--;
    }
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].base.active) {
            float vx = sin(bullets[i].angle) * bullets[i].base.speed * deltaTime;
            float vy = cos(bullets[i].angle) * bullets[i].base.speed * deltaTime;
            
            bullets[i].base.x += vx;
            bullets[i].base.y += vy;
            
            // Deactivate bullets that go off-screen
            if (bullets[i].base.y > WINDOW_HEIGHT + 10) {
                bullets[i].base.active = 0;
            }
        }
    }
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].base.active) {
            // Move enemy down with some horizontal movement based on pattern
            enemies[i].base.y -= enemies[i].base.speed * deltaTime;
            enemies[i].base.x += sin(enemies[i].movePattern + enemies[i].base.y * 0.01) * 2.0f;
            enemies[i].base.x = fmaxf(30, fminf(WINDOW_WIDTH - enemies[i].base.width - 10, enemies[i].base.x));
            
            // Deactivate enemies that go off-screen
            if (enemies[i].base.y < -enemies[i].base.height) {
                enemies[i].base.active = 0;
                spawnEnemy(); 
            }
        }
    }
    
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerUps[i].base.active) {
            powerUps[i].base.y -= powerUps[i].base.speed * deltaTime;
            powerUps[i].rotationAngle += 2.0f;
            
            // Deactivate power-ups that go off-screen
            if (powerUps[i].base.y < -powerUps[i].base.height) {
                powerUps[i].base.active = 0;
            }
        }
    }
    
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].y -= stars[i].speed * deltaTime;
        if (stars[i].y < 0) {
            stars[i].y = WINDOW_HEIGHT;
            stars[i].x = randomFloat(0, WINDOW_WIDTH);
        }
    }
    
    checkCollisions();
    
    int activeEnemies = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].base.active) {
            activeEnemies++;
        }
    }
    
    if (activeEnemies < 5 + level) {
        spawnEnemy();
    }
    
    // Check for level completion
    if (enemiesKilled >= enemiesRequired) {
        if (level < maxLevel) {
            // level++;
            gameState = LEVEL_COMPLETE_STATE; 
            return;  
        } else {
            gameState = GAME_OVER_STATE;
        }
    }
}

void checkCollisions() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].base.active) {
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (enemies[j].base.active) {
                    float dx = bullets[i].base.x - enemies[j].base.x;
                    float dy = bullets[i].base.y - enemies[j].base.y;
                    float distance = sqrt(dx * dx + dy * dy);
                    
                    if (distance < (bullets[i].base.width + enemies[j].base.width) / 2) {
                        // Hit! Reduce enemy health
                        int damage = (bullets[i].type == 1) ? 2 : 1;
                        enemies[j].base.health -= damage;
                        
                        if (enemies[j].base.health <= 0) {
                            // Enemy destroyed
                            score += enemies[j].points;
                            enemiesKilled++;
                            
                            // Spawn power-up from destroyed enemy
                            spawnPowerUp(enemies[j].base.x, enemies[j].base.y);
                            
                            enemies[j].base.active = 0;
                        }
                        
                        bullets[i].base.active = 0;
                        break;
                    }
                }
            }
        }
    }
    
    // Player-enemy collisions
    if (invincibilityFrames <= 0) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (enemies[i].base.active) {
                float dx = player.x - enemies[i].base.x;
                float dy = player.y - enemies[i].base.y;
                float distance = sqrt(dx * dx + dy * dy);
                
                if (distance < (player.width + enemies[i].base.width) / 2) {
                    // Collision! Handle damage
                    if (hasShield) {
                        // Shield absorbs the hit
                        hasShield = 0;
                    } else {
                        lives--;
                        if (lives <= 0) {
                            gameState = GAME_OVER_STATE;
                        }
                    }
                    
                    // Give temporary invincibility
                    invincibilityFrames = 120;
                    
                    // Destroy the enemy
                    enemies[i].base.active = 0;
                    break;
                }
            }
        }
    }
    
    // Player-powerup collisions
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerUps[i].base.active) {
            float dx = player.x - powerUps[i].base.x;
            float dy = player.y - powerUps[i].base.y;
            float distance = sqrt(dx * dx + dy * dy);
            
            if (distance < (player.width + powerUps[i].base.width) / 2) {
                // Collect power-up
                switch (powerUps[i].type) {
                    case 0: // Extra life
                        lives++;
                        if (lives > 5) lives = 5; 
                        break;
                    case 1: // Shield
                        hasShield = 1;
                        break;
                    case 2: // Rapid fire
                        hasRapidFire = 200; 
                        break;
                    case 3: // Multi-shot
                        hasMultiShot = 200; 
                        break;
                }
                
                powerUps[i].base.active = 0;
            }
        }
    }
    
    // Reduce power-up durations
    if (hasRapidFire > 0) hasRapidFire--;
    if (hasMultiShot > 0) hasMultiShot--;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    switch (gameState) {
        case SPLASH_STATE:
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, splashTexture);
            
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(WINDOW_WIDTH, 0);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(0, WINDOW_HEIGHT);
            glEnd();
            
            glDisable(GL_TEXTURE_2D);
            
            drawSplashText();
            
            glColor3f(1.0, 1.0, 1.0);  
            drawString(WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 200, 
                "Press SPACE to continue", 
                GLUT_BITMAP_HELVETICA_18);
            break;
            
        case MENU_STATE:
            drawMenu();
            break;
            
        case INSTRUCTIONS_STATE:
            drawInstructions();
            break;
            
        case PLAYING_STATE:
            // Draw game elements
            drawStars();
            
            if (gameState == PLAYING_STATE) {
                drawPlayer();
                drawBullets();
                drawEnemies();
                drawPowerUps();
                drawHUD();
                drawLives();
            }
            break;
            
        case GAME_OVER_STATE:
            drawGameOver();
            break;
            
        case LEVEL_COMPLETE_STATE:
            drawLevelComplete();
            break;
    }
    
    glutSwapBuffers();
}

void drawPlayer() {
    // Don't draw during invincibility frames (flashing effect)
    if (invincibilityFrames > 0 && invincibilityFrames % 10 < 5) {
        return;
    }

    // Draw the main body of the ship (Star Wars X-Wing style)
    setColor(player.color);
    glBegin(GL_QUADS);
        glVertex2f(player.x - player.width / 4, player.y); // Left side
        glVertex2f(player.x + player.width / 4, player.y); // Right side
        glVertex2f(player.x + player.width / 6, player.y + player.height); // Top right
        glVertex2f(player.x - player.width / 6, player.y + player.height); // Top left
    glEnd();

    // Draw the wings
    setColor(COLOR_WHITE);
    glBegin(GL_TRIANGLES);
        // Left wing
        glVertex2f(player.x - player.width / 2, player.y + player.height / 3);
        glVertex2f(player.x - player.width / 4, player.y + player.height / 3);
        glVertex2f(player.x - player.width / 2, player.y - player.height / 3);

        // Right wing
        glVertex2f(player.x + player.width / 2, player.y + player.height / 3);
        glVertex2f(player.x + player.width / 4, player.y + player.height / 3);
        glVertex2f(player.x + player.width / 2, player.y - player.height / 3);
    glEnd();

    // Draw the cockpit
    setColor(COLOR_BLACK);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 20; i++) {
        float angle = 2.0f * M_PI * i / 20;
        float x = player.x + cos(angle) * (player.width / 8);
        float y = player.y + player.height / 1.5 + sin(angle) * (player.width / 8);
        glVertex2f(x, y);
    }
    glEnd();

    // Draw engine flames
    setColor(COLOR_ORANGE);
    glBegin(GL_TRIANGLES);
        glVertex2f(player.x - player.width / 6, player.y - player.height / 2);
        glVertex2f(player.x - player.width / 12, player.y);
        glVertex2f(player.x - player.width / 4, player.y - player.height / 3);

        glVertex2f(player.x + player.width / 6, player.y - player.height / 2);
        glVertex2f(player.x + player.width / 12, player.y);
        glVertex2f(player.x + player.width / 4, player.y - player.height / 3);
    glEnd();

    // Draw shield if active
    if (hasShield) {
        setColor(COLOR_BLUE);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 20; i++) {
            float angle = 2.0f * M_PI * i / 20;
            float x = player.x + cos(angle) * (player.width / 2 + 10);
            float y = player.y + sin(angle) * (player.width / 2 + 10);
            glVertex2f(x, y);
        }
        glEnd();
    }
}

void drawBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].base.active) {
            setColor(bullets[i].base.color);
            glBegin(GL_QUADS);
                glVertex2f(bullets[i].base.x - bullets[i].base.width / 2, bullets[i].base.y - bullets[i].base.height / 2);
                glVertex2f(bullets[i].base.x + bullets[i].base.width / 2, bullets[i].base.y - bullets[i].base.height / 2);
                glVertex2f(bullets[i].base.x + bullets[i].base.width / 2, bullets[i].base.y + bullets[i].base.height / 2);
                glVertex2f(bullets[i].base.x - bullets[i].base.width / 2, bullets[i].base.y + bullets[i].base.height / 2);
            glEnd();
            
            // Draw trail effect
            glBegin(GL_TRIANGLES);
                glVertex2f(bullets[i].base.x - bullets[i].base.width / 2, bullets[i].base.y - bullets[i].base.height / 2);
                glVertex2f(bullets[i].base.x + bullets[i].base.width / 2, bullets[i].base.y - bullets[i].base.height / 2);
                glVertex2f(bullets[i].base.x, bullets[i].base.y - bullets[i].base.height);
                glEnd();
            }
        }
    }
    
    void drawEnemies() {
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (enemies[i].base.active) {
                setColor(enemies[i].base.color);

                switch (enemies[i].type) {
                    case 0: // Standard enemy (TIE Fighter style)
                        // Draw hexagonal wing panels
                        glBegin(GL_POLYGON);
                        for (int j = 0; j < 6; j++) {
                            float angle = 2.0f * M_PI * j / 6;
                            float x = enemies[i].base.x - enemies[i].base.width / 2 + cos(angle) * enemies[i].base.width / 2;
                            float y = enemies[i].base.y + sin(angle) * enemies[i].base.height / 2;
                            glVertex2f(x, y);
                        }
                        glEnd();
                        
                        glBegin(GL_POLYGON);
                        for (int j = 0; j < 6; j++) {
                            float angle = 2.0f * M_PI * j / 6;
                            float x = enemies[i].base.x + enemies[i].base.width / 2 + cos(angle) * enemies[i].base.width / 2;
                            float y = enemies[i].base.y + sin(angle) * enemies[i].base.height / 2;
                            glVertex2f(x, y);
                        }
                        glEnd();

                        // Draw central cockpit
                        setColor(COLOR_BLACK);
                        glBegin(GL_QUADS);
                        glVertex2f(enemies[i].base.x - enemies[i].base.width / 10, enemies[i].base.y - enemies[i].base.height / 4);
                        glVertex2f(enemies[i].base.x + enemies[i].base.width / 10, enemies[i].base.y - enemies[i].base.height / 4);
                        glVertex2f(enemies[i].base.x + enemies[i].base.width / 10, enemies[i].base.y + enemies[i].base.height / 4);
                        glVertex2f(enemies[i].base.x - enemies[i].base.width / 10, enemies[i].base.y + enemies[i].base.height / 4);
                        glEnd();
                        
                        // Draw connecting arms
                        setColor(enemies[i].base.color);
                        glBegin(GL_LINES);
                        glVertex2f(enemies[i].base.x - enemies[i].base.width / 10, enemies[i].base.y);
                        glVertex2f(enemies[i].base.x - enemies[i].base.width / 2, enemies[i].base.y);
                        
                        glVertex2f(enemies[i].base.x + enemies[i].base.width / 10, enemies[i].base.y);
                        glVertex2f(enemies[i].base.x + enemies[i].base.width / 2, enemies[i].base.y);
                        glEnd();
                        break;

                    case 1: // Armored enemy (Star Destroyer style)
                        // Draw triangular body
                        glBegin(GL_TRIANGLES);
                        glVertex2f(enemies[i].base.x, enemies[i].base.y + enemies[i].base.height / 2);
                        glVertex2f(enemies[i].base.x - enemies[i].base.width / 2, enemies[i].base.y - enemies[i].base.height / 2);
                        glVertex2f(enemies[i].base.x + enemies[i].base.width / 2, enemies[i].base.y - enemies[i].base.height / 2);
                        glEnd();

                        // Draw command bridge
                        setColor(COLOR_WHITE);
                        glBegin(GL_QUADS);
                        glVertex2f(enemies[i].base.x - enemies[i].base.width / 6, enemies[i].base.y);
                        glVertex2f(enemies[i].base.x + enemies[i].base.width / 6, enemies[i].base.y);
                        glVertex2f(enemies[i].base.x + enemies[i].base.width / 8, enemies[i].base.y + enemies[i].base.height / 6);
                        glVertex2f(enemies[i].base.x - enemies[i].base.width / 8, enemies[i].base.y + enemies[i].base.height / 6);
                        glEnd();
                        
                        // Draw engine glow
                        setColor(COLOR_CYAN);
                        glBegin(GL_QUADS);
                        glVertex2f(enemies[i].base.x - enemies[i].base.width / 4, enemies[i].base.y - enemies[i].base.height / 2);
                        glVertex2f(enemies[i].base.x + enemies[i].base.width / 4, enemies[i].base.y - enemies[i].base.height / 2);
                        glVertex2f(enemies[i].base.x + enemies[i].base.width / 5, enemies[i].base.y - enemies[i].base.height / 2.5);
                        glVertex2f(enemies[i].base.x - enemies[i].base.width / 5, enemies[i].base.y - enemies[i].base.height / 2.5);
                        glEnd();
                        break;

                    case 2: // Boss enemy (Death Star style)
                        // Draw main spherical body
                        glBegin(GL_POLYGON);
                        for (int j = 0; j < 20; j++) {
                            float angle = 2.0f * M_PI * j / 20;
                            float x = enemies[i].base.x + cos(angle) * enemies[i].base.width / 2;
                            float y = enemies[i].base.y + sin(angle) * enemies[i].base.height / 2;
                            glVertex2f(x, y);
                        }
                        glEnd();

                        // Draw equatorial trench
                        setColor(COLOR_BLACK);
                        glLineWidth(2.0);
                        glBegin(GL_LINES);
                        glVertex2f(enemies[i].base.x - enemies[i].base.width / 2, enemies[i].base.y);
                        glVertex2f(enemies[i].base.x + enemies[i].base.width / 2, enemies[i].base.y);
                        glEnd();
                        glLineWidth(1.0);
                        
                        // Draw superlaser dish
                        setColor(COLOR_GREEN);
                        glBegin(GL_POLYGON);
                        for (int j = 0; j < 10; j++) {
                            float angle = 2.0f * M_PI * j / 10;
                            float x = enemies[i].base.x + enemies[i].base.width / 4 + cos(angle) * enemies[i].base.width / 8;
                            float y = enemies[i].base.y - enemies[i].base.height / 4 + sin(angle) * enemies[i].base.height / 8;
                            glVertex2f(x, y);
                        }
                        glEnd();
                        break;
                }
            }
        }
    }
        void drawPowerUps() {
            for (int i = 0; i < MAX_POWERUPS; i++) {
                if (powerUps[i].base.active) {
                    setColor(powerUps[i].base.color);
                    
                    // Draw rotating diamond shape
                    glPushMatrix();
                    glTranslatef(powerUps[i].base.x, powerUps[i].base.y, 0);
                    glRotatef(powerUps[i].rotationAngle, 0, 0, 1);
                    
                    glBegin(GL_QUADS);
                    glVertex2f(0, powerUps[i].base.height / 2);
                    glVertex2f(powerUps[i].base.width / 2, 0);
                    glVertex2f(0, -powerUps[i].base.height / 2);
                    glVertex2f(-powerUps[i].base.width / 2, 0);
                    glEnd();
                    
                    setColor(COLOR_WHITE);
                    switch (powerUps[i].type) {
                        case 0: // Extra life (heart shape)
                        glBegin(GL_TRIANGLES);
                        glVertex2f(-5, 0);
                        glVertex2f(0, 5);
                        glVertex2f(5, 0);
                    glEnd();
                    glBegin(GL_TRIANGLES);
                    glVertex2f(-5, 0);
                    glVertex2f(0, -5);
                    glVertex2f(5, 0);
                    glEnd();
                    break;
                    
                    case 1: // Shield (circle)
                    glBegin(GL_LINE_LOOP);
                    for (int j = 0; j < 10; j++) {
                        float angle = 2.0f * M_PI * j / 10;
                        float x = cos(angle) * 5;
                        float y = sin(angle) * 5;
                        glVertex2f(x, y);
                    }
                    glEnd();
                    break;
                    
                    case 2: // Rapid fire (lightning bolt)
                    glBegin(GL_LINES);
                    glVertex2f(-3, 5);
                    glVertex2f(0, 0);
                    glVertex2f(0, 0);
                    glVertex2f(3, -5);
                    glEnd();
                    break;
                    
                    case 3: // Multi-shot (three dots)
                    glPointSize(3.0f);
                    glBegin(GL_POINTS);
                    glVertex2f(-4, 0);
                    glVertex2f(0, 0);
                    glVertex2f(4, 0);
                    glEnd();
                    break;
                }
                
                glPopMatrix();
            }
        }
    }
    
    // Draw starfield background
    void drawStars() {
        glPointSize(2.0f);
        glBegin(GL_POINTS);
        for (int i = 0; i < MAX_STARS; i++) {
            glColor3f(stars[i].brightness, stars[i].brightness, stars[i].brightness);
            glVertex2f(stars[i].x, stars[i].y);
        }
        glEnd();
}

void drawHUD() {
    char buffer[64];
    
    setColor(COLOR_WHITE);
    sprintf(buffer, "Score: %d", score);
    drawString(10, WINDOW_HEIGHT - 20, buffer, GLUT_BITMAP_HELVETICA_18);
    
    sprintf(buffer, "Level: %d/%d", level, maxLevel);
    drawString(WINDOW_WIDTH - 150, WINDOW_HEIGHT - 20, buffer, GLUT_BITMAP_HELVETICA_18);
    
    sprintf(buffer, "Enemies killed: %d/%d", enemiesKilled, enemiesRequired);
    drawString(WINDOW_WIDTH / 2 - 70, WINDOW_HEIGHT - 20, buffer, GLUT_BITMAP_HELVETICA_18);
    
    if (hasRapidFire) {
        setColor(COLOR_YELLOW);
        drawString(10, WINDOW_HEIGHT - 40, "Rapid Fire", GLUT_BITMAP_HELVETICA_12);
    }
    
    if (hasMultiShot) {
        setColor(COLOR_PURPLE);
        drawString(120, WINDOW_HEIGHT - 40, "Multi-Shot", GLUT_BITMAP_HELVETICA_12);
    }
    
    if (hasShield) {
        setColor(COLOR_BLUE);
        drawString(220, WINDOW_HEIGHT - 40, "Shield", GLUT_BITMAP_HELVETICA_12);
    }
    
    setColor(COLOR_WHITE);
    sprintf(buffer, "FPS: %.1f", 1.0f / deltaTime);
    drawString(WINDOW_WIDTH - 100, 20, buffer, GLUT_BITMAP_HELVETICA_12);
}

void drawLives() {
    setColor(COLOR_RED);
    for (int i = 0; i < lives; i++) {
        glBegin(GL_TRIANGLES);
            glVertex2f(20 + i * 25, 20);
            glVertex2f(10 + i * 25, 40);
            glVertex2f(30 + i * 25, 40);
        glEnd();
    }
}

void drawExplosion(float x, float y, float size, Color color) {
    setColor(color);
    
    for (int i = 0; i < 8; i++) {
        float angle = 2.0f * M_PI * i / 8;
        float nextAngle = 2.0f * M_PI * (i + 1) / 8;
        
        glBegin(GL_TRIANGLES);
            glVertex2f(x, y);
            glVertex2f(x + cos(angle) * size, y + sin(angle) * size);
            glVertex2f(x + cos(nextAngle) * size, y + sin(nextAngle) * size);
        glEnd();
    }
}

void drawMenu() {
    drawStars();
    
    setColor(COLOR_CYAN);
    drawString(WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT - 150, "SPACE DEFENDER", GLUT_BITMAP_TIMES_ROMAN_24);
    
    setColor(COLOR_WHITE);
    drawString(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2, "Press SPACE to Start", GLUT_BITMAP_HELVETICA_18);
    drawString(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 30, "Press I for Instructions", GLUT_BITMAP_HELVETICA_18);
    drawString(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 60, "Press Q to Quit", GLUT_BITMAP_HELVETICA_18);
    
    setColor(COLOR_YELLOW);
    drawString(WINDOW_WIDTH / 2 - 150, 100, "Computer Graphics Mini Project", GLUT_BITMAP_HELVETICA_18);
    drawString(WINDOW_WIDTH / 2 - 80, 70, "Team: Spirit", GLUT_BITMAP_HELVETICA_18);
}

void drawInstructions() {
    drawStars();
    
    setColor(COLOR_CYAN);
    drawString(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT - 100, "INSTRUCTIONS", GLUT_BITMAP_HELVETICA_18);
    
    setColor(COLOR_WHITE);
    drawString(WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT - 150, "A or Left Arrow - Move Left", GLUT_BITMAP_HELVETICA_18);
    drawString(WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT - 180, "D or Right Arrow - Move Right", GLUT_BITMAP_HELVETICA_18);
    drawString(WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT - 210, "Space - Fire", GLUT_BITMAP_HELVETICA_18);
    drawString(WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT - 240, "P - Pause Game", GLUT_BITMAP_HELVETICA_18);
    drawString(WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT - 270, "Q - Quit Game", GLUT_BITMAP_HELVETICA_18);
    
    setColor(COLOR_RED);
    drawString(WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT - 320, "Red Power-up - Extra Life", GLUT_BITMAP_HELVETICA_18);
    
    setColor(COLOR_BLUE);
    drawString(WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT - 350, "Blue Power-up - Shield", GLUT_BITMAP_HELVETICA_18);
    
    setColor(COLOR_YELLOW);
    drawString(WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT - 380, "Yellow Power-up - Rapid Fire", GLUT_BITMAP_HELVETICA_18);
    
    setColor(COLOR_PURPLE);
    drawString(WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT - 410, "Purple Power-up - Multi-Shot", GLUT_BITMAP_HELVETICA_18);
    
    setColor(COLOR_WHITE);
    drawString(WINDOW_WIDTH / 2 - 100, 100, "Press SPACE to return", GLUT_BITMAP_HELVETICA_18);
}

void drawGameOver() {
    drawStars();
    
    setColor(COLOR_RED);
    drawString(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT - 200, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
    
    setColor(COLOR_WHITE);
    char buffer[64];
    sprintf(buffer, "Final Score: %d", score);
    drawString(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2, buffer, GLUT_BITMAP_HELVETICA_18);
    
    setColor(COLOR_YELLOW);
    drawString(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 50, "Press SPACE to play again", GLUT_BITMAP_HELVETICA_18);
    drawString(WINDOW_WIDTH / 2 - 70, WINDOW_HEIGHT / 2 - 80, "Press Q to quit", GLUT_BITMAP_HELVETICA_18);
}

void drawSplashText() {
    glColor3f(1.0, 0.8, 0.0);
    drawString(WINDOW_WIDTH/2 - 245, WINDOW_HEIGHT/2 + 210, 
        "CGV mini PROJECT : Star Wars x Space Defender", 
        GLUT_BITMAP_TIMES_ROMAN_24);
    
    setColor(COLOR_CYAN);
    drawString(WINDOW_WIDTH/2 - 70, WINDOW_HEIGHT/2 + 110, 
        "~ by GROUP 17", 
        GLUT_BITMAP_HELVETICA_18);
    
    setColor(COLOR_WHITE);
    drawString(WINDOW_WIDTH/2 - 75, WINDOW_HEIGHT/2 + 60, 
    "Rachit(IIT2023100)", 
        GLUT_BITMAP_HELVETICA_18);
    
    drawString(WINDOW_WIDTH/2 - 85, WINDOW_HEIGHT/2 + 20, 
        "Sarvesh(IIT2023102)", 
        GLUT_BITMAP_HELVETICA_18);
    
    drawString(WINDOW_WIDTH/2 - 90, WINDOW_HEIGHT/2 - 20, 
        "Ashutosh(IIT2023028)",
        GLUT_BITMAP_HELVETICA_18);
    
    drawString(WINDOW_WIDTH/2 - 95, WINDOW_HEIGHT/2 - 60, 
        "Deepanshu(IIT2023013)", 
        GLUT_BITMAP_HELVETICA_18);
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    
    splashTexture = loadTexture("new5.jpeg");
    gameState = SPLASH_STATE;  
    
    initGame();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case ' ':
            switch (gameState) {
                case SPLASH_STATE:
                    gameState = MENU_STATE;
                    break;
                    
                case MENU_STATE:
                    gameState = PLAYING_STATE;
                    resetGame();
                    break;
                    
                case INSTRUCTIONS_STATE:
                    gameState = MENU_STATE;
                    break;
                    
                case PLAYING_STATE:
                    isShooting = 1;
                    break;
                    
                case GAME_OVER_STATE:
                case LEVEL_COMPLETE_STATE:
                    if (gameState == LEVEL_COMPLETE_STATE) {
                        gameState = PLAYING_STATE;
                        level++;
                        setupLevel(level);
                    } else {
                        gameState = MENU_STATE;
                    }
                    break;
            }
            break;
            
        case 'a':
        case 'A':
            isMovingLeft = 1;
            break;
            
        case 'd':
        case 'D':
            isMovingRight = 1;
            break;
            
        case 'i':
        case 'I':
            if (gameState == MENU_STATE) {
                gameState = INSTRUCTIONS_STATE;
            }
            break;
            
        case 'p':
        case 'P':
            if (gameState == PLAYING_STATE) {
                isPaused = !isPaused;
            }
            break;
            
        case 'q':
        case 'Q':
            exit(0);
            break;
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    switch (key) {
        case 'a':
        case 'A':
            isMovingLeft = 0;
            break;
            
        case 'd':
        case 'D':
            isMovingRight = 0;
            break;
            
        case ' ':
            isShooting = 0;
            break;
    }
}

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            isMovingLeft = 1;
            break;
            
        case GLUT_KEY_RIGHT:
            isMovingRight = 1;
            break;
    }
}

void specialKeysUp(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            isMovingLeft = 0;
            break;
            
        case GLUT_KEY_RIGHT:
            isMovingRight = 0;
            break;
    }
}

void idle() {
    updateGameState();
    glutPostRedisplay();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Space Defender");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);
    glutIdleFunc(idle);
    
    glutMainLoop();
    return 0;
}