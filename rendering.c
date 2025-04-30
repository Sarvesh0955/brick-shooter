#include "game_objects.h"
#include <stdio.h>
#include <math.h>

// Forward declarations for screens
void drawMenu();
void drawInstructions();
void drawGameOver();
void drawLevelComplete();
void drawSplashText();
void drawStoryScreen();

// Forward declarations for game elements
void drawPlayer();
void drawBullets();
void drawEnemies();
void drawPowerUps();
void drawStars();
void drawHUD();
void drawLives();
void drawExplosion(float x, float y, float size, Color color);
void drawExplosions();

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
            
        case STORY_STATE:
            drawStoryScreen();
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
                drawExplosions(); // Draw all active explosions
                drawHUD();
                drawLives();
            }
            break;
            
        case GAME_OVER_STATE:
            drawStars();
            drawExplosions(); // Draw explosions on game over screen too
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
            switch (enemies[i].type) {
                case 0: // Standard enemy (TIE Fighter style)
                    // Draw solar panel wings with more detail
                    setColor(COLOR_DARK_GRAY);
                    glBegin(GL_POLYGON);
                    for (int j = 0; j < 8; j++) {
                        float angle = 2.0f * M_PI * j / 8;
                        float x = enemies[i].base.x - enemies[i].base.width / 2 + cos(angle) * enemies[i].base.width / 2;
                        float y = enemies[i].base.y + sin(angle) * enemies[i].base.height / 2;
                        glVertex2f(x, y);
                    }
                    glEnd();
                    
                    glBegin(GL_POLYGON);
                    for (int j = 0; j < 8; j++) {
                        float angle = 2.0f * M_PI * j / 8;
                        float x = enemies[i].base.x + enemies[i].base.width / 2 + cos(angle) * enemies[i].base.width / 2;
                        float y = enemies[i].base.y + sin(angle) * enemies[i].base.height / 2;
                        glVertex2f(x, y);
                    }
                    glEnd();
                    
                    // Add wing details (TIE Fighter panel lines)
                    setColor(COLOR_BLACK);
                    for (int wing = -1; wing <= 1; wing += 2) {
                        float centerX = enemies[i].base.x + wing * enemies[i].base.width / 2;
                        float centerY = enemies[i].base.y;
                        
                        for (int j = 0; j < 4; j++) {
                            float angle = M_PI / 4 + j * M_PI / 2;
                            glBegin(GL_LINES);
                            glVertex2f(centerX, centerY);
                            glVertex2f(centerX + cos(angle) * enemies[i].base.width / 2,
                                       centerY + sin(angle) * enemies[i].base.height / 2);
                            glEnd();
                        }
                    }

                    // Draw central cockpit (ball shape with highlight)
                    setColor(COLOR_DARK_GRAY);
                    glBegin(GL_POLYGON);
                    for (int j = 0; j < 16; j++) {
                        float angle = 2.0f * M_PI * j / 16;
                        float x = enemies[i].base.x + cos(angle) * enemies[i].base.width / 5;
                        float y = enemies[i].base.y + sin(angle) * enemies[i].base.height / 5;
                        glVertex2f(x, y);
                    }
                    glEnd();
                    
                    // Cockpit window
                    setColor(COLOR_CYAN);
                    glBegin(GL_POLYGON);
                    for (int j = 0; j < 8; j++) {
                        float angle = 2.0f * M_PI * j / 8;
                        float x = enemies[i].base.x + cos(angle) * enemies[i].base.width / 10;
                        float y = enemies[i].base.y + sin(angle) * enemies[i].base.height / 10;
                        glVertex2f(x, y);
                    }
                    glEnd();
                    
                    // Draw connecting arms with thickness
                    setColor(COLOR_LIGHT_GRAY);
                    for (int side = -1; side <= 1; side += 2) {
                        glBegin(GL_QUADS);
                        glVertex2f(enemies[i].base.x + side * enemies[i].base.width / 10, enemies[i].base.y + 3);
                        glVertex2f(enemies[i].base.x + side * enemies[i].base.width / 2, enemies[i].base.y + 3);
                        glVertex2f(enemies[i].base.x + side * enemies[i].base.width / 2, enemies[i].base.y - 3);
                        glVertex2f(enemies[i].base.x + side * enemies[i].base.width / 10, enemies[i].base.y - 3);
                        glEnd();
                    }
                    break;

                case 1: // Armored enemy (Star Destroyer style)
                    // Draw triangular body with slight gradient effect
                    setColor(COLOR_LIGHT_GRAY);
                    glBegin(GL_TRIANGLES);
                    glVertex2f(enemies[i].base.x, enemies[i].base.y + enemies[i].base.height / 2);
                    glVertex2f(enemies[i].base.x - enemies[i].base.width / 2, enemies[i].base.y - enemies[i].base.height / 2);
                    glVertex2f(enemies[i].base.x + enemies[i].base.width / 2, enemies[i].base.y - enemies[i].base.height / 2);
                    glEnd();
                    
                    // Add detail lines on the hull
                    setColor(COLOR_DARK_GRAY);
                    glLineWidth(1.5);
                    glBegin(GL_LINES);
                    // Horizontal lines
                    for (int j = 1; j < 4; j++) {
                        float y = enemies[i].base.y - enemies[i].base.height/2 + j*enemies[i].base.height/4;
                        float widthAtY = (1 - (y - enemies[i].base.y + enemies[i].base.height/2)/enemies[i].base.height) * enemies[i].base.width;
                        glVertex2f(enemies[i].base.x - widthAtY/2, y);
                        glVertex2f(enemies[i].base.x + widthAtY/2, y);
                    }
                    glEnd();
                    glLineWidth(1.0);

                    // Draw command bridge with more detail
                    setColor(COLOR_WHITE);
                    glBegin(GL_QUADS);
                    glVertex2f(enemies[i].base.x - enemies[i].base.width / 6, enemies[i].base.y - enemies[i].base.height/8);
                    glVertex2f(enemies[i].base.x + enemies[i].base.width / 6, enemies[i].base.y - enemies[i].base.height/8);
                    glVertex2f(enemies[i].base.x + enemies[i].base.width / 8, enemies[i].base.y + enemies[i].base.height / 6);
                    glVertex2f(enemies[i].base.x - enemies[i].base.width / 8, enemies[i].base.y + enemies[i].base.height / 6);
                    glEnd();
                    
                    // Add bridge windows
                    setColor(COLOR_CYAN);
                    glPointSize(2.0);
                    glBegin(GL_POINTS);
                    for (int j = -2; j <= 2; j++) {
                        glVertex2f(enemies[i].base.x + j*5, enemies[i].base.y + enemies[i].base.height/12);
                    }
                    glEnd();
                    glPointSize(1.0);
                    
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
                    // Draw main spherical body with gradient effect
                    setColor(COLOR_LIGHT_GRAY);
                    glBegin(GL_POLYGON);
                    for (int j = 0; j < 30; j++) {
                        float angle = 2.0f * M_PI * j / 30;
                        float x = enemies[i].base.x + cos(angle) * enemies[i].base.width / 2;
                        float y = enemies[i].base.y + sin(angle) * enemies[i].base.height / 2;
                        
                        // Create slight gradient effect
                        if (angle > M_PI) {
                            setColor(COLOR_DARK_GRAY);
                        } else {
                            setColor(COLOR_LIGHT_GRAY);
                        }
                        
                        glVertex2f(x, y);
                    }
                    glEnd();

                    // Draw surface details (random small craters/structures)
                    setColor(COLOR_DARK_GRAY);
                    glPointSize(2.0);
                    glBegin(GL_POINTS);
                    for (int j = 0; j < 20; j++) {
                        float angle = 2.0f * M_PI * j / 20;
                        float radius = (0.3 + 0.15 * (j % 3)) * enemies[i].base.width / 2;
                        float x = enemies[i].base.x + cos(angle) * radius;
                        float y = enemies[i].base.y + sin(angle) * radius;
                        glVertex2f(x, y);
                    }
                    glEnd();
                    glPointSize(1.0);

                    // Draw equatorial trench with more detail
                    setColor(COLOR_BLACK);
                    glLineWidth(3.0);
                    glBegin(GL_LINES);
                    glVertex2f(enemies[i].base.x - enemies[i].base.width / 2, enemies[i].base.y);
                    glVertex2f(enemies[i].base.x + enemies[i].base.width / 2, enemies[i].base.y);
                    glEnd();
                    glLineWidth(1.0);
                    
                    // Add trench details
                    setColor(COLOR_DARK_GRAY);
                    glBegin(GL_QUADS);
                    for (int j = -3; j <= 3; j++) {
                        float x = enemies[i].base.x + j * enemies[i].base.width / 7;
                        glVertex2f(x - 3, enemies[i].base.y - 4);
                        glVertex2f(x + 3, enemies[i].base.y - 4);
                        glVertex2f(x + 3, enemies[i].base.y + 4);
                        glVertex2f(x - 3, enemies[i].base.y + 4);
                    }
                    glEnd();
                    
                    // Draw superlaser dish with glow effect
                    setColor(COLOR_GREEN);
                    glBegin(GL_POLYGON);
                    for (int j = 0; j < 16; j++) {
                        float angle = 2.0f * M_PI * j / 16;
                        float x = enemies[i].base.x - enemies[i].base.width / 4 + cos(angle) * enemies[i].base.width / 8;
                        float y = enemies[i].base.y - enemies[i].base.height / 4 + sin(angle) * enemies[i].base.height / 8;
                        glVertex2f(x, y);
                    }
                    glEnd();
                    
                    // Add superlaser center
                    setColor(COLOR_BRIGHT_GREEN);
                    glBegin(GL_POLYGON);
                    for (int j = 0; j < 8; j++) {
                        float angle = 2.0f * M_PI * j / 8;
                        float x = enemies[i].base.x - enemies[i].base.width / 4 + cos(angle) * enemies[i].base.width / 16;
                        float y = enemies[i].base.y - enemies[i].base.height / 4 + sin(angle) * enemies[i].base.height / 16;
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
    
    // Draw outer explosion rays
    for (int i = 0; i < 12; i++) {
        float angle = 2.0f * M_PI * i / 12;
        float nextAngle = 2.0f * M_PI * (i + 1) / 12;
        
        glBegin(GL_TRIANGLES);
            glVertex2f(x, y);
            glVertex2f(x + cos(angle) * size, y + sin(angle) * size);
            glVertex2f(x + cos(nextAngle) * size, y + sin(nextAngle) * size);
        glEnd();
    }
    
    // Draw inner explosion with different color for depth effect
    Color innerColor = {1.0f, 0.7f, 0.2f}; // Bright orange
    setColor(innerColor);
    
    for (int i = 0; i < 8; i++) {
        float angle = 2.0f * M_PI * i / 8 + 0.3; // Offset for variation
        float nextAngle = 2.0f * M_PI * (i + 1) / 8 + 0.3;
        
        glBegin(GL_TRIANGLES);
            glVertex2f(x, y);
            glVertex2f(x + cos(angle) * size * 0.6f, y + sin(angle) * size * 0.6f);
            glVertex2f(x + cos(nextAngle) * size * 0.6f, y + sin(nextAngle) * size * 0.6f);
        glEnd();
    }
    
    // Draw center glow
    Color centerColor = {1.0f, 1.0f, 0.5f}; // Bright yellow
    setColor(centerColor);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 16; i++) {
        float angle = 2.0f * M_PI * i / 16;
        glVertex2f(x + cos(angle) * size * 0.3f, y + sin(angle) * size * 0.3f);
    }
    glEnd();
}

void drawExplosions() {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i].active) {
            drawExplosion(explosions[i].x, explosions[i].y, explosions[i].size, explosions[i].color);
        }
    }
}

void drawMenu() {
    drawStars();
    
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float glow = (sin(time * 2.0f) + 1.0f) * 0.3f;
    float scale = 1.0f + sin(time * 1.5f) * 0.05f;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    glPushMatrix();
    glTranslatef(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 150, 0);
    glScalef(scale, scale, 1.0f);
    setColor(COLOR_CYAN);
    drawString(-120, 0, "SPACE DEFENDER", GLUT_BITMAP_TIMES_ROMAN_24);
    glPopMatrix();
    
    setColor(COLOR_ORANGE);
    drawString(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT - 180, "The Last Stand", GLUT_BITMAP_HELVETICA_18);
    
    setColor(COLOR_YELLOW);
    glColor4f(1.0f, 1.0f, 0.0f, 0.7f + sin(time) * 0.3f);
    drawString(WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT - 220, "\"In a galaxy filled with chaos,", GLUT_BITMAP_HELVETICA_12);
    drawString(WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT - 240, "you are the last hope for the Republic.\"", GLUT_BITMAP_HELVETICA_12);
    
    glColor4f(1.0f, 1.0f, 1.0f, 0.8f + glow);
    drawString(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2, "> Press SPACE to Start <", GLUT_BITMAP_HELVETICA_18);
    
    setColor(COLOR_WHITE);
    glColor4f(1.0f, 1.0f, 1.0f, 0.7f + sin(time * 1.5f) * 0.3f);
    drawString(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 30, "Press I for Instructions", GLUT_BITMAP_HELVETICA_18);
    drawString(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 60, "Press Q to Quit", GLUT_BITMAP_HELVETICA_18);
    
    setColor(COLOR_YELLOW);
    drawString(WINDOW_WIDTH / 2 - 150, 120, "Computer Graphics Mini Project", GLUT_BITMAP_9_BY_15);
    
    glColor4f(0.0f, 1.0f, 1.0f, 0.7f + sin(time * 2.0f) * 0.3f);
    drawString(WINDOW_WIDTH / 2 - 80, 90, "Team: Spirit", GLUT_BITMAP_HELVETICA_18);
    
    setColor(COLOR_WHITE);
    glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
    drawString(WINDOW_WIDTH - 70, 20, "v1.0", GLUT_BITMAP_8_BY_13);
    
    glDisable(GL_BLEND);
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
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    
    // Enable blending for glow effects
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    // Title with pulsing glow effect
    float titleGlow = (sin(time * 2.0f) + 1.0f) * 0.3f;
    glColor4f(1.0f, 0.8f, 0.0f, 0.7f + titleGlow);
    
    // Draw title with scale effect
    glPushMatrix();
    float titleScale = 1.0f + sin(time * 1.5f) * 0.05f;
    glTranslatef(WINDOW_WIDTH/2 - 245, WINDOW_HEIGHT/2 + 210, 0);
    glScalef(titleScale, titleScale, 1.0f);
    drawString(0, 0, "CGV mini PROJECT : Star Wars x Space Defender", 
        GLUT_BITMAP_TIMES_ROMAN_24);
    glPopMatrix();
    
    // Group name with cyan glow
    float groupGlow = (sin(time * 3.0f) + 1.0f) * 0.3f;
    glColor4f(0.0f, 1.0f, 1.0f, 0.7f + groupGlow);
    drawString(WINDOW_WIDTH/2 - 70, WINDOW_HEIGHT/2 + 110, 
        "~ by GROUP 17", 
        GLUT_BITMAP_HELVETICA_18);
    
    glColor3f(0.7f, 0.8f, 1.0f);
    drawString(WINDOW_WIDTH/2 - 75, WINDOW_HEIGHT/2 + 60, 
        "Rachit(IIT2023100)", 
        GLUT_BITMAP_HELVETICA_18);
    
    glColor3f(0.7f, 1.0f, 0.8f);
    drawString(WINDOW_WIDTH/2 - 85, WINDOW_HEIGHT/2 + 20, 
        "Sarvesh(IIT2023101)", 
        GLUT_BITMAP_HELVETICA_18);
    
    glColor3f(1.0f, 0.7f, 1.0f);
    drawString(WINDOW_WIDTH/2 - 90, WINDOW_HEIGHT/2 - 20, 
        "Ashutosh(IIT2023028)",
        GLUT_BITMAP_HELVETICA_18);
    
    glColor3f(1.0f, 0.8f, 0.7f);
    drawString(WINDOW_WIDTH/2 - 95, WINDOW_HEIGHT/2 - 60, 
        "Deepanshu(IIT2023013)", 
        GLUT_BITMAP_HELVETICA_18);
    

    
    glDisable(GL_BLEND);
}

void drawStoryScreen() {
    drawStars();
    
    // Get current time for animations
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    
    // Title with glowing effect
    setColor(COLOR_YELLOW);
    float glow = (sin(time * 2.0f) + 1.0f) * 0.3f;
    glColor4f(1.0f, 1.0f, 0.0f, 0.7f + glow);
    
    const char* title = "A LONG TIME AGO IN A GALAXY FAR, FAR AWAY...";
    int titleWidth = 0;
    for (const char* c = title; *c != '\0'; c++) {
        titleWidth += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }
    
    // Draw title with scaling effect
    glPushMatrix();
    glTranslatef(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 150, 0);
    float scale = 1.0f + sin(time * 1.5f) * 0.05f;
    glScalef(scale, scale, 1.0f);
    drawString(-titleWidth / 2, 0, title, GLUT_BITMAP_TIMES_ROMAN_24);
    glPopMatrix();
    
    // Subtitle with different color
    setColor(COLOR_CYAN);
    const char* subtitle = "Chapter I: The IIITA Chronicles";
    int subtitleWidth = 0;
    for (const char* c = subtitle; *c != '\0'; c++) {
        subtitleWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
    }
    drawString(WINDOW_WIDTH / 2 - subtitleWidth / 2, WINDOW_HEIGHT - 200, 
        subtitle, GLUT_BITMAP_HELVETICA_18);
    
    // Story text with fade-in effect
    float alpha = (sin(time) + 1.0f) * 0.5f;
    glColor4f(1.0f, 1.0f, 1.0f, alpha);
    
    const char* line1 = "In the prestigious halls of IIITA, a young Jedi programmer named";
    const char* line2 = "Mr. Pavan discovers an ancient computer infected by the Dark Side.";
    const char* line3 = "The Separatist virus army threatens to corrupt all systems.";
    const char* line4 = "As IIITA's last hope, you must pilot your ship through";
    const char* line5 = "the digital space and defend the Republic's sacred code!";
    
    // Calculate widths for center alignment
    int lineWidths[5] = {0};
    const char* lines[] = {line1, line2, line3, line4, line5};
    
    for (int i = 0; i < 5; i++) {
        for (const char* c = lines[i]; *c != '\0'; c++) {
            lineWidths[i] += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }
    
    // Draw story text with vertical spacing
    for (int i = 0; i < 5; i++) {
        drawString(WINDOW_WIDTH / 2 - lineWidths[i] / 2, 
            WINDOW_HEIGHT / 2 + 60 - (i * 30), 
            lines[i], GLUT_BITMAP_HELVETICA_18);
    }
    
    // Epic call to action with pulsing effect
    setColor(COLOR_RED);
    float pulse = (sin(time * 3.0f) + 1.0f) * 0.5f;
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f + pulse);
    
    const char* callToAction = "Will you accept this mission, young Padawan?";
    int ctaWidth = 0;
    for (const char* c = callToAction; *c != '\0'; c++) {
        ctaWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
    }
    drawString(WINDOW_WIDTH / 2 - ctaWidth / 2, WINDOW_HEIGHT / 2 - 110, 
        callToAction, GLUT_BITMAP_HELVETICA_18);
    
    // Continue text with blinking effect
    const char* continueText = "Press SPACE to begin your journey";
    int continueWidth = 0;
    for (const char* c = continueText; *c != '\0'; c++) {
        continueWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    if ((int)(time * 2) % 2) { // Blinking effect
        setColor(COLOR_WHITE);
        drawString(WINDOW_WIDTH / 2 - continueWidth / 2, WINDOW_HEIGHT / 2 - 150, 
            continueText, GLUT_BITMAP_HELVETICA_18);
    }
    
    // Draw small decorative Jedi symbol
    setColor(COLOR_YELLOW);
    glBegin(GL_TRIANGLES);
    float symbolSize = 20.0f;
    float centerX = WINDOW_WIDTH / 2;
    float centerY = WINDOW_HEIGHT / 2 - 200;
    glVertex2f(centerX, centerY + symbolSize);
    glVertex2f(centerX - symbolSize, centerY - symbolSize);
    glVertex2f(centerX + symbolSize, centerY - symbolSize);
    glEnd();
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