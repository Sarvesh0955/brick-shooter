#include "game_objects.h"
#include <stdio.h>
#include <stdlib.h>

// rendering.c
extern void display();

// game_state.c
extern void updateGameState();
extern void checkCollisions();

void init();
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void specialKeysUp(int key, int x, int y);
void idle();
void reshape(int w, int h);

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
                    gameState = STORY_STATE;  
                    break;
                    
                case STORY_STATE:           
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