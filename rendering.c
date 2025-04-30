#include "game_objects.h"
#include <stdio.h>
#include <math.h>

// Forward declarations for screens
void drawMenu();
void drawInstructions();
void drawGameOver();
void drawLevelComplete();
void drawSplashText();

// Forward declarations for game elements
void drawPlayer();
void drawBullets();
void drawEnemies();
void drawPowerUps();
void drawStars();
void drawHUD();
void drawLives();
void drawExplosion(float x, float y, float size, Color color);

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
        "Sarvesh(IIT2023101)", 
        GLUT_BITMAP_HELVETICA_18);
    
    drawString(WINDOW_WIDTH/2 - 90, WINDOW_HEIGHT/2 - 20, 
        "Ashutosh(IIT2023028)",
        GLUT_BITMAP_HELVETICA_18);
    
    drawString(WINDOW_WIDTH/2 - 95, WINDOW_HEIGHT/2 - 60, 
        "Deepanshu(IIT2023013)", 
        GLUT_BITMAP_HELVETICA_18);
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