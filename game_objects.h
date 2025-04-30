#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#include <GL/glut.h>

#define M_PI 3.14159265358979323846
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define SPLASH_STATE -1
#define MENU_STATE 0
#define INSTRUCTIONS_STATE 1
#define STORY_STATE 5 
#define PLAYING_STATE 2
#define GAME_OVER_STATE 3
#define LEVEL_COMPLETE_STATE 4

#define MAX_BULLETS 50
#define MAX_ENEMIES 30
#define MAX_POWERUPS 5
#define MAX_STARS 100

typedef struct {
    GLfloat r, g, b;
} Color;

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

typedef struct {
    float x, y;
    float brightness;
    float speed;
} Star;

extern Color COLOR_RED;
extern Color COLOR_GREEN;
extern Color COLOR_BLUE;
extern Color COLOR_YELLOW;
extern Color COLOR_PURPLE;
extern Color COLOR_CYAN;
extern Color COLOR_ORANGE;
extern Color COLOR_WHITE;
extern Color COLOR_BLACK;
extern Color COLOR_DARK_GRAY;
extern Color COLOR_LIGHT_GRAY;
extern Color COLOR_BRIGHT_GREEN;

extern int gameState;
extern int score;
extern int lives;
extern int level;
extern int maxLevel;
extern int enemiesKilled;
extern int enemiesRequired;
extern float difficultyMultiplier;
extern int isPaused;

extern GameObject player;
extern int isMovingLeft;
extern int isMovingRight;
extern int isShooting;
extern int hasShield;
extern int hasRapidFire;
extern int hasMultiShot;
extern int shootCooldown;
extern int invincibilityFrames;

extern Bullet bullets[MAX_BULLETS];
extern Enemy enemies[MAX_ENEMIES];
extern PowerUp powerUps[MAX_POWERUPS];
extern Star stars[MAX_STARS];

extern int lastFrameTime;
extern float deltaTime;
extern GLuint splashTexture;

float randomFloat(float min, float max);
void setColor(Color color);
void drawString(float x, float y, const char* string, void* font);
GLuint loadTexture(const char* filename);

void initGame();
void resetGame();
void setupLevel(int level);

void spawnEnemy();
void spawnPowerUp(float x, float y);
void fireBullet(int type, float angleOffset);

void updateGameState();
void checkCollisions();

#endif