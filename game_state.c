#include "game_objects.h"
#include <stdio.h>
#include <math.h>

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
            
            if (bullets[i].base.y > WINDOW_HEIGHT + 10) {
                bullets[i].base.active = 0;
            }
        }
    }
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].base.active) {
            enemies[i].base.y -= enemies[i].base.speed * deltaTime;
            enemies[i].base.x += sin(enemies[i].movePattern + enemies[i].base.y * 0.01) * 2.0f;
            enemies[i].base.x = fmaxf(30, fminf(WINDOW_WIDTH - enemies[i].base.width - 10, enemies[i].base.x));
            
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
    
    // level completion
    if (enemiesKilled >= enemiesRequired) {
        if (level < maxLevel) {
            gameState = LEVEL_COMPLETE_STATE; 
            return;  
        } else {
            gameState = GAME_OVER_STATE;
        }
    }
}

void checkCollisions() {
    // Bullet-enemy collisions
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].base.active) {
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (enemies[j].base.active) {
                    float dx = bullets[i].base.x - enemies[j].base.x;
                    float dy = bullets[i].base.y - enemies[j].base.y;
                    float distance = sqrt(dx * dx + dy * dy);
                    
                    if (distance < (bullets[i].base.width + enemies[j].base.width) / 2) {
                        int damage = (bullets[i].type == 1) ? 2 : 1;
                        enemies[j].base.health -= damage;
                        
                        Color impactColor = {1.0f, 0.8f, 0.4f}; 
                        createExplosion(bullets[i].base.x, bullets[i].base.y, 10.0f, impactColor);
                        
                        if (enemies[j].base.health <= 0) {
                            createExplosion(enemies[j].base.x, enemies[j].base.y, 
                                  enemies[j].base.width, enemies[j].base.color);
                            
                            score += enemies[j].points;
                            enemiesKilled++;
                            
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
                    if (hasShield) {
                        Color shieldColor = {0.3f, 0.3f, 1.0f}; 
                        createExplosion(player.x, player.y, player.width, shieldColor);
                        
                        hasShield = 0;
                    } else {
                        Color damageColor = {1.0f, 0.3f, 0.3f};
                        createExplosion(player.x, player.y, player.width / 2, damageColor);
                        
                        lives--;
                        if (lives <= 0) {
                            createExplosion(player.x, player.y, player.width * 2, COLOR_RED);
                            gameState = GAME_OVER_STATE;
                        }
                    }
                    
                    createExplosion(enemies[i].base.x, enemies[i].base.y, 
                                  enemies[i].base.width, enemies[i].base.color);
                    
                    invincibilityFrames = 120;
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
    
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i].active) {
            explosions[i].lifeTime--;
            if (explosions[i].lifeTime <= 0) {
                explosions[i].active = 0;
            }
        }
    }
    
    if (hasRapidFire > 0) hasRapidFire--;
    if (hasMultiShot > 0) hasMultiShot--;
}