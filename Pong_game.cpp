#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include <ctime>

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

bool gameStarted = false;
bool gameOver = false;

// Timer variables
int gameTime = 10;
int timeLeft = gameTime;
time_t startTime;

void drawText(float x, float y, std::string text) {
	glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    for (int i = 0; i < text.length(); ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

void drawGameOverScreen() {
	std::string winner;
	if (scoreLeft > scoreRight) winner = "Player 1 Wins!";
	else if (scoreLeft < scoreRight && !aiEnabled) winner = "Player 2 Wins!";
	else if (scoreLeft < scoreRight && aiEnabled) winner = "You Lose!";
	else winner = "It is a tie!";
	drawText(WIDTH / 2 - 40, HEIGHT / 2, winner);
	drawText(WIDTH / 2 - 40, HEIGHT / 2 + 40, "Game Over");
}

void drawStartScreen() {
	drawText(WIDTH / 2 - 60, HEIGHT / 2, "Press Space to start");
}

void timer() {
	
}

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

void resetBall() {
    ballX = WIDTH / 2;
    ballY = HEIGHT / 2;
    ballSpeedX = -ballSpeedX;
    ballSpeedY = (rand() % 3 + 2) * ((rand() % 2 == 0) ? 1 : -1);
}

void updateBall() {
    if (paused || gameOver) return;

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
    if (paused || gameOver) return;

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
	
	// Game not started yet
	if (!gameStarted) {
		drawStartScreen();
		glutSwapBuffers();
		return;
	}
	
    // Paddles
    drawRect(10, leftPaddleY, PADDLE_WIDTH, PADDLE_HEIGHT);
    drawRect(WIDTH - 20, rightPaddleY, PADDLE_WIDTH, PADDLE_HEIGHT);

    // Ball
    drawRect(ballX, ballY, ballRadius, ballRadius);

    // Score
    std::stringstream ss;
    ss << scoreLeft << "                    " << scoreRight;
    drawText(WIDTH / 2 - 60, HEIGHT - 30, ss.str());
    
    // Timer
	std::stringstream ts;
	ts << "Time Left: " << timeLeft << "s";
	drawText(10, HEIGHT - 30, ts.str());

    // Paused status
    if (paused)
        drawText(WIDTH / 2 - 40, HEIGHT / 2, "PAUSED");
    
    // Game Over Screen
    if (gameOver) {
		drawGameOverScreen();
		drawText(WIDTH / 2 - 80, HEIGHT / 2 - 40, "Press 'R' to Restart");
	}

    glutSwapBuffers();
}

void restartGame() {
    // Reset scores
    scoreLeft = 0;
    scoreRight = 0;

    // Reset paddle positions
    leftPaddleY = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    rightPaddleY = HEIGHT / 2 - PADDLE_HEIGHT / 2;

    // Reset ball
    resetBall();

    // Reset flags
    paused = false;
    gameOver = false;

    // Restart timer
    startTime = time(NULL);
}

void timer(int) {
	if (gameStarted && !paused && !gameOver) {
		timeLeft = gameTime - (int)(time(NULL) - startTime);
		if (timeLeft <= 0) {
			gameOver = true;
			timeLeft = 0;
		}
	}
	
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
        case 32: // Space key to start the game
		if (!gameStarted) {
			gameStarted = true;
			startTime = time(NULL);
			timeLeft = gameTime;
		}
		break;
		case 'r': restartGame(); break;
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
