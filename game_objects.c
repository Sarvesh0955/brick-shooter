#include "game_objects.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#include "stb_image.h"
#define STBI_rgb_alpha 4

// Define the global variables
Color COLOR_RED = {1.0f, 0.0f, 0.0f};
Color COLOR_GREEN = {0.0f, 1.0f, 0.0f};
Color COLOR_BLUE = {0.0f, 0.0f, 1.0f};
Color COLOR_YELLOW = {1.0f, 1.0f, 0.0f};
Color COLOR_PURPLE = {0.8f, 0.2f, 0.8f};
Color COLOR_CYAN = {0.0f, 1.0f, 1.0f};
Color COLOR_ORANGE = {1.0f, 0.6f, 0.0f};
Color COLOR_WHITE = {1.0f, 1.0f, 1.0f};
Color COLOR_BLACK = {0.0f, 0.0f, 0.0f};

// Game state variables
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

// Game objects
Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];
PowerUp powerUps[MAX_POWERUPS];
Star stars[MAX_STARS];

int lastFrameTime = 0;
float deltaTime = 0.0f;
GLuint splashTexture;

// Utility functions
float randomFloat(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

void setColor(Color color) {
    glColor3f(color.r, color.g, color.b);
}

void drawString(float x, float y, const char* string, void* font) {
    glRasterPos2f(x, y);
    for (const char* c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

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

// Game initialization and setup
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

// Game object spawning functions
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