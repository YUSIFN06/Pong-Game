#include <GL/glut.h>
#include <iostream>
#include <sstream>

// Window size
const int WIDTH = 640;
const int HEIGHT = 480;

// Paddle properties
const int PADDLE_WIDTH = 10;
const int PADDLE_HEIGHT = 80;
float leftPaddleY = HEIGHT / 2 - PADDLE_HEIGHT / 2;
float rightPaddleY = HEIGHT / 2 - PADDLE_HEIGHT / 2;

// Ball properties
float ballX = WIDTH / 2;	
float ballY = HEIGHT / 2;
float ballRadius = 8;
float ballSpeedX = 4;
float ballSpeedY = 3;

// Score
int scoreLeft = 0;
int scoreRight = 0;

// Control flags
bool moveUpLeft = false, moveDownLeft = false;
bool moveUpRight = false, moveDownRight = false;
bool paused = false;
bool aiEnabled = true; // AI for right paddle

void drawBorder() {
    glColor3f(1.0f, 1.0f, 0.0f);
    glLineWidth(10);

    glBegin(GL_LINES);
    glVertex2f(0, HEIGHT);
    glVertex2f(WIDTH, HEIGHT);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(0, 0);
    glVertex2f(WIDTH, 0);
    glEnd();
    
    glLineWidth(3);
    for(float i = 0; i < HEIGHT; i+=40){
    	glBegin(GL_LINES);
    	glVertex2f(WIDTH / 2, i);
    	glVertex2f(WIDTH / 2, i + 30);
    	glEnd();
	}
}

void drawRect(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawText(float x, float y, std::string text) {
	glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    for (int i = 0; i < text.length(); ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

void resetBall() {
    ballX = WIDTH / 2;
    ballY = HEIGHT / 2;
    ballSpeedX = -ballSpeedX;
    ballSpeedY = (rand() % 3 + 2) * ((rand() % 2 == 0) ? 1 : -1);
}

void updateBall() {
    if (paused) return;

    ballX += ballSpeedX;
    ballY += ballSpeedY;

    if (ballY <= 0 || ballY >= HEIGHT - ballRadius)
        ballSpeedY = -ballSpeedY;

    // Left paddle collision
    if (ballX <= 20 && ballY >= leftPaddleY && ballY <= leftPaddleY + PADDLE_HEIGHT)
        ballSpeedX = -ballSpeedX;

    // Right paddle collision
    if (ballX >= WIDTH - 20 - ballRadius && ballY >= rightPaddleY && ballY <= rightPaddleY + PADDLE_HEIGHT)
        ballSpeedX = -ballSpeedX;

    // Score conditions
    if (ballX < 0) {
        scoreRight++;
        resetBall();
    } else if (ballX > WIDTH) {
        scoreLeft++;
        resetBall();
    }
}

void updatePaddles() {
    if (paused) return;

    if (moveUpLeft && leftPaddleY > 0)
        leftPaddleY -= 5;
    if (moveDownLeft && leftPaddleY < HEIGHT - PADDLE_HEIGHT)
        leftPaddleY += 5;

    if (!aiEnabled) {
        if (moveUpRight && rightPaddleY > 0)
            rightPaddleY -= 5;
        if (moveDownRight && rightPaddleY < HEIGHT - PADDLE_HEIGHT)
            rightPaddleY += 5;
    } else {
        if (ballY > rightPaddleY + PADDLE_HEIGHT / 2)
            rightPaddleY += 3;
        else if (ballY < rightPaddleY + PADDLE_HEIGHT / 2)
            rightPaddleY -= 3;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
	// Border
	drawBorder();
    // Paddles
    drawRect(10, leftPaddleY, PADDLE_WIDTH, PADDLE_HEIGHT);
    drawRect(WIDTH - 20, rightPaddleY, PADDLE_WIDTH, PADDLE_HEIGHT);

    // Ball
    drawRect(ballX, ballY, ballRadius, ballRadius);

    // Score
    std::stringstream ss;
    ss << scoreLeft << "                    " << scoreRight;
    drawText(WIDTH / 2 - 60, HEIGHT - 30, ss.str());

    // Paused status
    if (paused)
        drawText(WIDTH / 2 - 40, HEIGHT / 2, "PAUSED");

    glutSwapBuffers();
}

void timer(int) {
    updatePaddles();
    updateBall();

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyPressed(unsigned char key, int x, int y) {
    switch (key) {
        case 'w': moveUpLeft = true; break;
        case 's': moveDownLeft = true; break;
        case 'i': moveUpRight = true; break;
        case 'k': moveDownRight = true; break;
        case 'm': paused = !paused; break;
        case 'a': aiEnabled = !aiEnabled; break;
    }
}

void keyReleased(unsigned char key, int x, int y) {
    switch (key) {
        case 'w': moveUpLeft = false; break;
        case 's': moveDownLeft = false; break;
        case 'i': moveUpRight = false; break;
        case 'k': moveDownRight = false; break;
    }
}

void init() {
    glClearColor(0, 0, 0, 1);
    glColor3f(1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Pong Game");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyReleased);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
