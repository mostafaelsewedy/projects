#include <glut.h>
#include <stdio.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

int score = 0;
int health = 5;
float playerY = 100.0f;
bool isJumping = false, isDucking = false;
float jumpHeight = 70.0f, jumpProgress = 0.0f;
float speed = 0.75f;
float obstacleX = 300.0f, collectableX = 350.0f;
int totalTime = 40;
int remainingTime = totalTime;
int frame = 0;
bool gameEnded = false, gameLost = false;
int shieldDuration = 3, doubleDuration = 3;
bool shieldActive = false;
int shieldTimer = 0, doubleTimer = 0;
float scoreMultiplier = 1.0f;
int collectableCounter = 0;
float obstacleHeight = 90.0f;
float backgroundOffset = 0.0f;
float backgroundSpeed = 0.02f;
bool isCollidingWithObstacle = false;

void resetObstacle() {
    obstacleX = 300.0f;
    obstacleHeight = 90.0f + rand() % 65;
}

enum ItemType { COLLECTABLE, POWERUP1, POWERUP2 };
ItemType currentItemType;

void randomizeItemType() {

    if (collectableCounter >= 5) {
        currentItemType = (rand() % 2 == 0) ? POWERUP1 : POWERUP2;
        collectableCounter = 0;
    }
    else {
        int randomValue = rand() % 100;

        if (randomValue < 60) {
            currentItemType = COLLECTABLE;
            collectableCounter++;
        }
        else if (randomValue < 80 && currentItemType != POWERUP1) {
            currentItemType = POWERUP1;
        }
        else if (randomValue < 100 && currentItemType != POWERUP2) {
            currentItemType = POWERUP2;
        }
        else {
            currentItemType = COLLECTABLE;
            collectableCounter++;
        }
    }
}

void drawUpperBoundary() {

    glColor3f(0.2f, 0.6f, 0.8f);
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 290.0f, 0.0f);
    glVertex3f(300.0f, 290.0f, 0.0f);
    glVertex3f(300.0f, 300.0f, 0.0f);
    glVertex3f(0.0f, 300.0f, 0.0f);
    glEnd();

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i <= 300; i += 10) {
        glVertex3f(i, 295.0f, 0.0f);
    }
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 300; i += 40) {
        glVertex3f(i, 290.0f, 0.0f);
        glVertex3f(i + 20.0f, 290.0f, 0.0f);
        glVertex3f(i + 10.0f, 275.0f, 0.0f);
    }
    glEnd();

    glColor3f(0.6f, 0.2f, 0.6f);
    glBegin(GL_POLYGON);
    glVertex3f(150.0f, 290.0f, 0.0f);
    glVertex3f(160.0f, 290.0f, 0.0f);
    glVertex3f(155.0f, 280.0f, 0.0f);
    glEnd();
}

void drawLowerBoundary() {
 
    glColor3f(0.2f, 0.8f, 0.6f);
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 80.0f, 0.0f);
    glVertex3f(300.0f, 80.0f, 0.0f);
    glVertex3f(300.0f, 90.0f, 0.0f);
    glVertex3f(0.0f, 90.0f, 0.0f);
    glEnd();

    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i <= 300; i += 10) {
        glVertex3f(i, 85.0f, 0.0f);
    }
    glEnd();

    glColor3f(0.0f, 0.5f, 1.0f);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 300; i += 40) {
        glVertex3f(i, 80.0f, 0.0f);
        glVertex3f(i + 20.0f, 80.0f, 0.0f);
        glVertex3f(i + 10.0f, 65.0f, 0.0f);
    }
    glEnd();

    glColor3f(0.8f, 0.4f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(150.0f, 85.0f, 0.0f);
    for (int i = 0; i <= 360; i += 30) {
        float theta = i * 3.14159f / 180;
        glVertex3f(150.0f + 5.0f * cos(theta), 85.0f + 5.0f * sin(theta), 0.0f);
    }
    glEnd();
}

void drawPlayer() {
    
    float torsoHeight = isDucking ? 15.0f : 20.0f;
    float headY = isDucking ? playerY + torsoHeight + 5.0f : playerY + torsoHeight + 10.0f;

    // Torso
    glColor3f(0.8f, 0.52f, 0.25f);
    glBegin(GL_QUADS);
    glVertex3f(50.0f, playerY, 0.0f);
    glVertex3f(70.0f, playerY, 0.0f);
    glVertex3f(70.0f, playerY + torsoHeight, 0.0f);
    glVertex3f(50.0f, playerY + torsoHeight, 0.0f);
    glEnd();

    // Arm
    glColor3f(1.0f, 0.85f, 0.68f);
    glBegin(GL_POLYGON);
    glVertex3f(55.0f, playerY + 5.0f + (isDucking ? 0.0f : 5.0f), 0.0f);
    glVertex3f(65.0f, playerY + 5.0f + (isDucking ? 0.0f : 5.0f), 0.0f);
    glVertex3f(65.0f, playerY + 18.0f + (isDucking ? 0.0f : 5.0f), 0.0f);
    glVertex3f(55.0f, playerY + 18.0f + (isDucking ? 0.0f : 5.0f), 0.0f);
    glEnd();

    // Head
    glColor3f(1.0f, 0.85f, 0.68f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(60.0f, headY, 0.0f);
    for (int i = 0; i <= 360; i += 10) {
        float theta = i * 3.14159f / 180;
        float x = 10.0f * cos(theta);
        float y = 10.0f * sin(theta);
        glVertex3f(60.0f + x, headY + y, 0.0f);
    }
    glEnd();

    // Eye
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    glVertex3f(65.0f, headY + 2.0f, 0.0f);
    glEnd();

    // Legs
    glColor3f(0.1f, 0.5f, 0.8f);
    // Left leg
    glBegin(GL_QUADS);
    glVertex3f(50.0f, playerY - 10.0f, 0.0f);
    glVertex3f(58.0f, playerY - 10.0f, 0.0f);
    glVertex3f(58.0f, playerY, 0.0f);
    glVertex3f(50.0f, playerY, 0.0f);
    glEnd();

    // Right leg
    glBegin(GL_QUADS);
    glVertex3f(62.0f, playerY - 10.0f, 0.0f);
    glVertex3f(70.0f, playerY - 10.0f, 0.0f);
    glVertex3f(70.0f, playerY, 0.0f);
    glVertex3f(62.0f, playerY, 0.0f);
    glEnd();

}

void drawObstacle() {
   
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(obstacleX, obstacleHeight, 0.0f);
    glVertex3f(obstacleX + 15.0f, obstacleHeight, 0.0f);
    glVertex3f(obstacleX + 15.0f, obstacleHeight + 15.0f, 0.0f);
    glVertex3f(obstacleX, obstacleHeight + 15.0f, 0.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex3f(obstacleX, obstacleHeight + 15.0f, 0.0f);
    glVertex3f(obstacleX + 15.0f, obstacleHeight + 15.0f, 0.0f);
    glVertex3f(obstacleX + 7.5f, obstacleHeight + 25.0f, 0.0f);
    glEnd();

}

void drawCollectable() {
    
    static float rotationAngle = 0.0f;

    glPushMatrix();
    glTranslatef(collectableX, 150.0f, 0.0f);
    glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);
    glTranslatef(-collectableX, -150.0f, 0.0f);

    glColor3f(0.8f, 0.6f, 0.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 360; i += 10) {
        float theta = i * 3.14159f / 180;
        glVertex3f(collectableX + 12.0f * cos(theta), 150.0f + 12.0f * sin(theta), 0.0f);
    }
    glEnd();

    glColor3f(1.0f, 0.84f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(collectableX, 150.0f, 0.0f);
    for (int i = 0; i <= 360; i += 10) {
        float theta = i * 3.14159f / 180;
        glVertex3f(collectableX + 10.0f * cos(theta), 150.0f + 10.0f * sin(theta), 0.0f);
    }
    glEnd();

    glColor3f(0.9f, 0.9f, 0.4f);
    glBegin(GL_QUADS);
    glVertex3f(collectableX - 3.0f, 150.0f - 3.0f, 0.0f);
    glVertex3f(collectableX + 3.0f, 150.0f - 3.0f, 0.0f);
    glVertex3f(collectableX + 3.0f, 150.0f + 3.0f, 0.0f);
    glVertex3f(collectableX - 3.0f, 150.0f + 3.0f, 0.0f);
    glEnd();

    glColor3f(0.9f, 0.7f, 0.1f);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 360; i += 30) {
        float theta1 = i * 3.14159f / 180;
        float theta2 = (i + 15) * 3.14159f / 180;
        glVertex3f(collectableX, 150.0f, 0.0f);
        glVertex3f(collectableX + 12.0f * cos(theta1), 150.0f + 12.0f * sin(theta1), 0.0f);
        glVertex3f(collectableX + 12.0f * cos(theta2), 150.0f + 12.0f * sin(theta2), 0.0f);
    }
    glEnd();

    glPopMatrix();

    rotationAngle += 2.0f;
    if (rotationAngle > 360.0f) {
        rotationAngle -= 360.0f;
    }
}

void drawPowerUp1() {

    static float scaleFactor = 1.0f;
    static bool growing = true;

    glPushMatrix();
    glTranslatef(collectableX, 150.0f, 0.0f);
    glScalef(scaleFactor, scaleFactor, 1.0f);
    glTranslatef(-collectableX, -150.0f, 0.0f);

    glColor3f(0.5f, 0.5f, 0.9f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 360; i += 10) {
        float theta = i * 3.14159f / 180;
        glVertex3f(collectableX + 12.0f * cos(theta), 150.0f + 12.0f * sin(theta), 0.0f);
    }
    glEnd();

    glColor3f(0.6f, 0.7f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(collectableX, 150.0f, 0.0f);
    for (int i = 0; i <= 360; i += 10) {
        float theta = i * 3.14159f / 180;
        glVertex3f(collectableX + 10.0f * cos(theta), 150.0f + 10.0f * sin(theta), 0.0f);
    }
    glEnd();

    glColor3f(0.9f, 0.9f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(collectableX - 1.5f, 150.0f + 6.0f, 0.0f);
    glVertex3f(collectableX + 1.5f, 150.0f + 6.0f, 0.0f);
    glVertex3f(collectableX + 1.5f, 150.0f - 6.0f, 0.0f);
    glVertex3f(collectableX - 1.5f, 150.0f - 6.0f, 0.0f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f(collectableX - 6.0f, 150.0f + 1.5f, 0.0f);
    glVertex3f(collectableX + 6.0f, 150.0f + 1.5f, 0.0f);
    glVertex3f(collectableX + 6.0f, 150.0f - 1.5f, 0.0f);
    glVertex3f(collectableX - 6.0f, 150.0f - 1.5f, 0.0f);
    glEnd();

    glPopMatrix();

    if (growing) {
        scaleFactor += 0.01f;
        if (scaleFactor >= 1.2f) {
            growing = false;
        }
    }
    else {
        scaleFactor -= 0.01f;
        if (scaleFactor <= 1.0f) {
            growing = true;
        }
    }

}

void drawPowerUp2() {

    static float bounceOffset = 0.0f;
    static float bounceSpeed = 0.05f;
    static bool movingUp = true;

    if (movingUp) {
        bounceOffset += bounceSpeed;
        if (bounceOffset > 5.0f) {
            movingUp = false;
        }
    }
    else {
        bounceOffset -= bounceSpeed;
        if (bounceOffset < -5.0f) {
            movingUp = true;
        }
    }

    glPushMatrix();
    glTranslatef(0.0f, bounceOffset, 0.0f);

    glColor3f(0.5f, 1.0f, 0.5f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 360; i += 10) {
        float theta = i * 3.14159f / 180;
        glVertex3f(collectableX + 12.0f * cos(theta), 150.0f + 12.0f * sin(theta), 0.0f);
    }
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(collectableX, 150.0f, 0.0f);
    for (int i = 0; i <= 360; i += 10) {
        float theta = i * 3.14159f / 180;
        glVertex3f(collectableX + 10.0f * cos(theta), 150.0f + 10.0f * sin(theta), 0.0f);
    }
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(collectableX - 5.0f, 150.0f + 4.0f, 0.0f);
    glVertex3f(collectableX + 5.0f, 150.0f + 4.0f, 0.0f);
    glVertex3f(collectableX + 5.0f, 150.0f + 6.0f, 0.0f);
    glVertex3f(collectableX - 5.0f, 150.0f + 6.0f, 0.0f);

    glVertex3f(collectableX - 5.0f, 150.0f - 4.0f, 0.0f);
    glVertex3f(collectableX + 5.0f, 150.0f - 4.0f, 0.0f);
    glVertex3f(collectableX + 5.0f, 150.0f - 6.0f, 0.0f);
    glVertex3f(collectableX - 5.0f, 150.0f - 6.0f, 0.0f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f(collectableX + 3.0f, 150.0f + 4.0f, 0.0f);
    glVertex3f(collectableX + 5.0f, 150.0f + 4.0f, 0.0f);
    glVertex3f(collectableX + 5.0f, 150.0f - 4.0f, 0.0f);
    glVertex3f(collectableX + 3.0f, 150.0f - 4.0f, 0.0f);
    glEnd();

    glPopMatrix();
}

void drawHUD() {

    glColor3f(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < 5; i++) {
        glBegin(GL_LINE_LOOP);
        glVertex3f(10 + i * 30, 260, 0.0f);
        glVertex3f(10 + i * 30 + 20, 260, 0.0f);
        glVertex3f(10 + i * 30 + 20, 270, 0.0f);
        glVertex3f(10 + i * 30, 270, 0.0f);
        glEnd();
    }

    glColor3f(0.0f, 1.0f, 0.0f);
    for (int i = 0; i < health; i++) {
        glBegin(GL_QUADS);
        glVertex3f(10 + i * 30, 260, 0.0f);
        glVertex3f(10 + i * 30 + 20, 260, 0.0f);
        glVertex3f(10 + i * 30 + 20, 270, 0.0f);
        glVertex3f(10 + i * 30, 270, 0.0f);
        glEnd();
    }

    char scoreStr[50];
    snprintf(scoreStr, sizeof(scoreStr), "Score: %d", score);
    glRasterPos2f(200, 260);
    for (int i = 0; scoreStr[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreStr[i]);
    }

    int minutes = remainingTime / 60;
    int seconds = remainingTime % 60;
    char timerStr[50];
    snprintf(timerStr, sizeof(timerStr), "Time: %02d:%02d", minutes, seconds);
    glRasterPos2f(10, 240);
    for (int i = 0; timerStr[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, timerStr[i]);
    }

    if (shieldActive && scoreMultiplier == 2.0f) {
        snprintf(timerStr, sizeof(timerStr), "Shield Active: %d s, Double Score Active: %d s", shieldTimer / 60, doubleTimer / 60); 
    }
    else if (shieldActive) {
        snprintf(timerStr, sizeof(timerStr), "Shield Active: %d s", shieldTimer / 60);
    }
    else if (scoreMultiplier == 2.0f) {
        snprintf(timerStr, sizeof(timerStr), "Double Score Active: %d s", doubleTimer / 60);
    }
    else {
        snprintf(timerStr, sizeof(timerStr), "No Active Power-Up");
    }

    glRasterPos2f(10, 220);
    for (int i = 0; timerStr[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, timerStr[i]);
    }
}

void drawGameEndScreen() {
    glColor3f(1.0f, 1.0f, 1.0f);
    char endStr[50];
    if (gameLost) {
        snprintf(endStr, sizeof(endStr), "Game Lose - Score: %d", score);
    }
    else {
        snprintf(endStr, sizeof(endStr), "Game End - Score: %d", score);
    }
    glRasterPos2f(100, 150);
    for (int i = 0; endStr[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, endStr[i]);
    }
}

void drawBackground() {
    for (float y = 0.0f; y <= 300.0f; y += 1.0f) {
        float colorValue = (y / 300.0f);
        glColor3f(0.5f, 0.5f + colorValue * 0.5f, 1.0f);
        glBegin(GL_LINES);
        glVertex3f(0.0f, y + backgroundOffset, 0.0f);
        glVertex3f(300.0f, y + backgroundOffset, 0.0f);
        glEnd();
    }
    backgroundOffset += backgroundSpeed;
    if (backgroundOffset > 10.0f) {
        backgroundOffset -= 10.0f;
    }
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawBackground();

    if (gameEnded) {
        drawGameEndScreen();
    }
    else {
        drawPlayer();
        drawObstacle();
        drawUpperBoundary();
        drawLowerBoundary();
        drawHUD();
        switch (currentItemType) {
        case COLLECTABLE:
            drawCollectable();
            break;
        case POWERUP1:
            drawPowerUp1();
            break;
        case POWERUP2:
            drawPowerUp2();
            break;
        }
    }

    glFlush();
}

void Timer(int value) {
    frame++;

    if (gameEnded) {
        glutPostRedisplay();
        return;
    }

    if (isJumping) {
        if (jumpProgress < jumpHeight && playerY + 40.0f <= 280.0f) {
            playerY += 5.0f;
            jumpProgress += 5.0f;
        }
        else {
            isJumping = false;
        }
    }
    else if (playerY > 100.0f) {
        playerY -= 7.5f;
        if (playerY < 100.0f) {
            playerY = 100;
        }
    }

    if (!isCollidingWithObstacle) {
        obstacleX -= speed;
        collectableX -= speed;

        if (obstacleX < -20.0f) {
            resetObstacle();
        }

        if (collectableX < -20.0f) {
            collectableX = 350.0f + rand() % 100;
            randomizeItemType();
        }

        if (abs(collectableX - obstacleX) < 30.0f) {
            collectableX = obstacleX + 50.0f;
        }

        speed += 0.01f;
    }

    if(obstacleX < 70.0f && obstacleX + 15.0f > 50.0f) {
        float playerHeight = isDucking ? 25.0f : 40.0f;
        if (playerY < obstacleHeight + 25.0f && playerY + playerHeight > obstacleHeight) {
            if (!shieldActive && !isCollidingWithObstacle) {
                health--;
                isCollidingWithObstacle = true;
                if (health == 0) {
                    gameLost = true;
                    gameEnded = true;
                }
            }
            if (shieldActive && !isCollidingWithObstacle) {
                isCollidingWithObstacle = true;
            }
        }
        else {
            isCollidingWithObstacle = false;
        }
    }

    if (collectableX < 70.0f && collectableX + 10.0f > 50.0f && playerY + 40.0f > 140.0f && playerY < 160.0f) {
        if (currentItemType == POWERUP1) {
            shieldActive = true;
            shieldTimer = shieldDuration * 60;
        }
        else if (currentItemType == POWERUP2) {
            scoreMultiplier = 2.0f;
            doubleTimer = doubleDuration * 60;
        }

        score += 1 * scoreMultiplier;
        collectableX = 350.0f;
        randomizeItemType();
    }

    if (shieldTimer > 0) {
        shieldTimer--;
        if (shieldTimer == 0) {
            if (shieldActive) {
                shieldActive = false;
            }
        }
    }

    if (doubleTimer > 0) {
        doubleTimer--;
        if (doubleTimer == 0) {
            if (scoreMultiplier == 2.0f) {
                scoreMultiplier = 1.0f;
            }
        }
    }

    if (health == 0 || remainingTime == 0) {
        scoreMultiplier = 1.0f;
    }

    if (frame == 60 && remainingTime > 0) {
        remainingTime--;
        frame = 0;
    }

    if (remainingTime == 0) {
        gameEnded = true;
    }

    glutPostRedisplay();
    glutTimerFunc(1000 / 60, Timer, 0); // 60 FPS 
}

void Keyboard(unsigned char key, int x, int y) {
    if (key == ' ' && !isJumping && playerY == 100.0f) {
        isJumping = true;
        jumpProgress = 0.0f;
    }
    else if (key == 'd') {
        isDucking = true;
    }
}

void KeyboardUp(unsigned char key, int x, int y) {
    if (key == 'd') {
        isDucking = false;
        playerY = 100.0f;
    }
}

int main(int argc, char** argv) {

    srand(static_cast<unsigned int>(time(0)));

    glutInit(&argc, argv);
    glutInitWindowSize(450, 300);
    glutInitWindowPosition(150, 150);
    glutCreateWindow("2D Infinite Runner");
    glutDisplayFunc(Display);
    glutKeyboardFunc(Keyboard);
    glutKeyboardUpFunc(KeyboardUp);
    glutTimerFunc(0, Timer, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gluOrtho2D(0.0, 300, 0.0, 300);

    glutMainLoop();
    return 0;
}
