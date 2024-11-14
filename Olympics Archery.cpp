#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <time.h>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

float scaleFactor = 1.0f;
float scaleDirection = 1.0f;
float scaleSpeed = 0.01f;
float minScale = 1.0f;
float maxScale = 1.2f;
int startTime = (int)time(NULL) + 60;
int elapsedTime = 0;
float timeRotate = 0.0f;
int currentWallColorIndex = 0;
int currentSeatColorIndex = 0;
float rotationAngles[5][3] = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
bool animation = true;
bool flame = true;
bool gameEnd = false;
int score = 0;
float bowX = 0.5f, bowY = 0.35f;
GLfloat wallColors[4][4][4] = {
	{ {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.8f, 1.0f}, {0.0f, 0.0f, 0.5f, 1.0f}, {10.0f} },  // Blue (Europe)
	{ {1.0f, 1.0f, 0.0f, 1.0f}, {0.8f, 0.8f, 0.0f, 1.0f}, {0.5f, 0.5f, 0.0f, 1.0f}, {10.0f} },  // Yellow (Asia)
	{ {0.0f, 0.0f, 0.0f, 1.0f}, {0.2f, 0.2f, 0.2f, 1.0f}, {0.1f, 0.1f, 0.1f, 1.0f}, {10.0f} },  // Black (Africa)
	{ {1.0f, 0.0f, 0.0f, 1.0f}, {0.8f, 0.0f, 0.0f, 1.0f}, {0.5f, 0.0f, 0.0f, 1.0f}, {10.0f} }   // Red (Americas)
};
GLfloat additionalWallColors[2][4][4] = {
	{ {0.7f, 0.7f, 0.5f, 1.0f}, {0.6f, 0.6f, 0.4f, 1.0f}, {0.5f, 0.5f, 0.3f, 1.0f}, {10.0f} },
	{ {0.0f, 0.5f, 0.5f, 1.0f}, {0.0f, 0.45f, 0.45f, 1.0f}, {0.0f, 0.35f, 0.35f, 1.0f}, {10.0f} }
};
GLfloat flameColors[3][4][4] = {
		{1.0f, 0.2f, 0.0f, 1.0f},
		{1.0f, 0.6f, 0.0f, 1.0f},
		{1.0f, 1.0f, 0.0f, 1.0f}
};
GLfloat clockColor[4][4] = {{0.3f, 0.3f, 0.3f, 1.0f},{0.8f, 0.8f, 0.8f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} , {30.0f}};

//Arrow
bool arrowReady = false ;
float arrowYaw = 0.0;
float arrowPitch = 0.0;
bool arrowMoving = false;
float arrowX = 0.5f, arrowY = 0.35f, arrowZ = 1.2f;
GLfloat arrowColor[3][4][4] = {
	{ {0.3f, 0.15f, 0.0f, 1.0f}, { 0.5f, 0.25f, 0.0f, 1.0f }, { 0.3f, 0.15f, 0.0f, 1.0f }, { 10.0f } },
	{ { 0.19225f, 0.19225f, 0.19225f, 1.0f }, { 0.50754f, 0.50754f, 0.50754f, 1.0f }, { 0.508273f, 0.508273f, 0.508273f, 1.0f }, { 51.2f } },
	{ { 0.3f, 0.3f, 0.3f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 128.0f } }
};

//Player
double animationAngle = 0.0;
bool isMoving = false;
double swingSpeed = 90.0;
double currentSwingAngle = 0.0; 
double restSpeed = 7.5;

float playerX = 0.5f, playerY = 0.45f, playerZ = 1.5f;
float playerSpeed = 0.05f;
char playerMoveDirection = 'W';
char view = 'f';
float playerYaw = 0.0f;
float playerPitch = 0.0f;
float playerDeltaX = 0 , playerDeltaZ = 0;

//Camera
float cameraYaw = 0.0f;
float cameraPitch = 0.0f;
float lastMouseX = 0.0f;
float lastMouseY = 0.0f;
bool firstMouse = true;
float cameraSpeed = 0.05f;
double strayX = 0 , strayZ = 0;

void setMaterialColor(GLfloat ambient[], GLfloat diffuse[], GLfloat specular[], GLfloat shininess) {
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};
class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera frontCamera(0.5f, 0.8f, 1.75f,
	0.5f, 0.48f, 0.0f,
	0.0f, 1.0f, 0.0f);
Camera topCamera(0.5f, 2.5f, 0.6f,
	0.5f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f);
Camera sideCamera(1.98f, 0.6f, 0.7f,
	0.0f, 0.6f, 0.7f,
	0.0f, 1.0f, 0.0f);
Camera camera = frontCamera;

//Extras
void drawGameEndScreen() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
	gluOrtho2D(0, windowWidth, 0, windowHeight);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(1.0f, 1.0f, 1.0f);

	char endStr[50];
	if (score == 0) {
		snprintf(endStr, sizeof(endStr), "GAME LOSE");
	}
	else if (score == 10) {
		snprintf(endStr, sizeof(endStr), "BULLSEYE - Score: %d", score);
	}
	else {
		snprintf(endStr, sizeof(endStr), "GAME END - Score: %d", score);
	}

	int textWidth = 0;
	for (int i = 0; endStr[i] != '\0'; i++) {
		textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, endStr[i]);
	}

	int xPos = (windowWidth - textWidth) / 2;
	int yPos = windowHeight / 2;

	glRasterPos2i(xPos, yPos);
	for (int i = 0; endStr[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, endStr[i]);
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
void calculateScore() {
	
		float rotatedArrowX = arrowX;
		float rotatedArrowY = arrowY;

		float radians = DEG2RAD(-15);

		rotatedArrowY = arrowY * cos(radians) - arrowZ * sin(radians);

		float distance = sqrt(pow(rotatedArrowX - 0.5, 2) + pow(rotatedArrowY - 0.31, 2));

		if (distance <= 0.03 * scaleFactor) {
			score = 10;
		}
		else if (distance <= 0.06 * scaleFactor) {
			score = 8;
		}
		else if (distance <= 0.09 * scaleFactor) {
			score = 6;
		}
		else if (distance <= 0.12 * scaleFactor) {
			score = 4;
		}
		else {
			score = 0;
		}
}
void updateAnimations(int value) {

	if (animation) {
		scaleFactor += scaleSpeed * scaleDirection;

		if (scaleFactor >= maxScale || scaleFactor <= minScale) {
			scaleDirection = -scaleDirection;
		}
	}

	if (flame) {
		for (int i = 0; i < 3; i++) {
			flameColors[i][0][0] = 1.0f;
			flameColors[i][1][0] = 0.4f + static_cast<float>(rand() % 40) / 100.0f;
			flameColors[i][2][0] = static_cast<float>(rand() % 20) / 100.0f;
		}

		for (int i = 0; i < 5; i++) {
			rotationAngles[i][0] = rand() % 360;
			rotationAngles[i][1] = rand() % 360;
			rotationAngles[i][2] = rand() % 360;
		}
	}

	glutPostRedisplay();

	glutTimerFunc(16, updateAnimations, 0);
}
void updateTimer(int value) {
	int currentTime = (int)time(NULL);
	elapsedTime = startTime - currentTime;

	if (elapsedTime <= 0) {
		elapsedTime = 0;
		gameEnd = true;
	}

	if (elapsedTime % 3 == 0) {
		currentWallColorIndex = (currentWallColorIndex + 1) % 2;
		if (animation)
			currentSeatColorIndex = (currentSeatColorIndex + 1) % 4;

		setMaterialColor(additionalWallColors[currentWallColorIndex][0], additionalWallColors[currentWallColorIndex][1], additionalWallColors[currentWallColorIndex][2], additionalWallColors[currentWallColorIndex][3][0]);
	}

	timeRotate += (360 / 60);

	glutPostRedisplay();

	if (elapsedTime > 0) {
		glutTimerFunc(1000, updateTimer, 0);
	}
	else {
		glutPostRedisplay();
	}
}

//Player
bool canMoveTo(double newX, double newZ) {

	double minX = 0.2 , maxX = 0.8 ;
	double minZ = 1.2 , maxZ = 1.73;

	if (newX < minX || newX > maxX || newZ < minZ || newZ > maxZ) {
		return false;
	}

	return true;
}
void movePlayer(double speed, int direction) {

	float radians = DEG2RAD(playerYaw);

	double deltaX = 0.0;
	double deltaZ = 0.0;
	isMoving = false;

	if (direction == 1) { // Forward
		deltaX = speed * sin(radians);
		deltaZ = speed * cos(radians);
		isMoving = true;
	}
	else if (direction == -1) { // Backward
		deltaX = speed * sin(radians);
		deltaZ = speed * cos(radians);
		isMoving = true;
	}
	else if (direction == 2) { // Left
		deltaX = speed * cos(radians);
		deltaZ = speed * sin(radians);
		isMoving = true;
	}
	else if (direction == -2) { // Right
		deltaX = speed * cos(radians);
		deltaZ = speed * sin(radians);
		isMoving = true;
	}

	if (canMoveTo(playerX + deltaX, playerZ + deltaZ)) {
		playerX += deltaX;
		playerZ += deltaZ;

		playerDeltaX = deltaX;
		playerDeltaZ = deltaZ;

		if (view == 'f') {
			camera.moveX(- (deltaX + strayX));
			camera.moveZ(- (deltaZ + strayZ));
			strayX = 0;
			strayZ = 0;
		}
		else {
			strayX += deltaX;
			strayZ += deltaZ;
		}

		if (!arrowReady) {
			arrowX += deltaX;
			arrowZ += deltaZ;
		}
	}
	else {
		isMoving = false;
	}

	if (isMoving) {
		animationAngle += swingSpeed;
		if (animationAngle > 360.0) animationAngle -= 360.0;
		currentSwingAngle = 15.0 * sin(DEG2RAD(animationAngle));
	}
	else {
		if (currentSwingAngle > 0.0) {
			currentSwingAngle -= restSpeed;
			if (currentSwingAngle < 0.0) currentSwingAngle = 0.0;
		}
		else if (currentSwingAngle < 0.0) {
			currentSwingAngle += restSpeed;
			if (currentSwingAngle > 0.0) currentSwingAngle = 0.0;
		}
	}
}
void updatePlayerRotation() {

	float diff = cameraYaw - playerYaw;
	float diff2 = cameraPitch - playerPitch;

	if (diff > 180.0f) {
		diff -= 360.0f;
	}
	else if (diff < -180.0f) {
		diff += 360.0f;
	}

	if (diff2 > 180.0f) {
		diff2 -= 360.0f;
	}
	else if (diff2 < -180.0f) {
		diff2 += 360.0f;
	}

	playerYaw += diff * cameraSpeed * 0.1f;
	playerPitch += diff2 * cameraSpeed;
	
	arrowYaw = playerYaw;
	arrowPitch = playerPitch;

	if (playerYaw > 15.0f)
		arrowYaw = 15.0f;
	else if (playerYaw < -15.0f)
		arrowYaw = -15.0f;

	if (playerPitch > 15.0f)
		arrowPitch = 15.0f;
	else if (playerPitch < -15.0f)
		arrowPitch = -15.0f;

	if (playerYaw > 180.0f) {
		playerYaw -= 360.0f;
	}
	else if (playerYaw < -180.0f) {
		playerYaw += 360.0f;
	}
	if (playerPitch > 180.0f) {
		playerPitch -= 360.0f;
	}
	else if (playerPitch < -180.0f) {
		playerPitch += 360.0f;
	}
}
void drawPlayer(double columnHeight, double baseRadius) {
	GLUquadric* quad = gluNewQuadric();

	GLfloat skinAmbient[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	GLfloat skinDiffuse[] = { 0.9f, 0.7f, 0.5f, 1.0f };
	GLfloat skinSpecular[] = { 0.7f, 0.5f, 0.3f, 1.0f };
	GLfloat skinShininess = 10.0f;
	setMaterialColor(skinAmbient, skinDiffuse, skinSpecular, skinShininess);

	glPushMatrix();
	glTranslated(playerX, playerY, playerZ);
	glRotated(180, 0, 1, 0);
	
	glPushMatrix();
	glRotated(playerYaw, 0, 1, 0);
	glRotated(playerPitch, 1, 0, 0);

	// Head
	glPushMatrix();
	gluSphere(quad, 0.08, 20, 20);
	glPopMatrix();

	// Eyes
	GLfloat eyeColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	setMaterialColor(eyeColor, eyeColor, eyeColor, 10.0f);

	// Left eye
	glPushMatrix();
	glTranslated(-0.03, 0.03, 0.07);
	gluSphere(quad, 0.02, 10, 10);
	glPopMatrix();

	// Right eye
	glPushMatrix();
	glTranslated(0.03, 0.03, 0.07);
	gluSphere(quad, 0.02, 10, 10);
	glPopMatrix();

	glPopMatrix();

	// Torso
	GLfloat shirtColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	setMaterialColor(shirtColor, shirtColor, shirtColor, 10.0f);
	glPushMatrix();
	glTranslated(0, -0.175, 0);
	glScaled(0.12, 0.18, 0.1);
	glutSolidCube(1);
	glPopMatrix();

	// Pants
	GLfloat pantsColor[] = { 0.2f, 0.3f, 0.6f, 1.0f };
	setMaterialColor(pantsColor, pantsColor, pantsColor, 10.0f);

	double swingOffset = 15.0 * sin(DEG2RAD(animationAngle));

	// Right leg
	glPushMatrix();
	glTranslated(-baseRadius * 0.2, -0.25, 0);
	glRotated(90 - currentSwingAngle, 1, 0, 0);
	gluCylinder(quad, 0.02, 0.02, 0.2, 30, 30);
	glPopMatrix();

	// Left leg
	glPushMatrix();
	glTranslated(baseRadius * 0.2, -0.25, 0);
	glRotated(90 + currentSwingAngle, 1, 0, 0);
	gluCylinder(quad, 0.02, 0.02, 0.2, 30, 30);
	glPopMatrix();

	// Arms
	GLfloat armColor[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	setMaterialColor(armColor, armColor, armColor, 10.0f);

	// Right arm
	glPushMatrix();
	glTranslated(-baseRadius * 0.5, -0.1, 0);
	glRotated(90 , 0, 0, 1);
	glRotated(45, 1, 0, 0);
	gluCylinder(quad, 0.02, 0.02, 0.15, 30, 30);
	glPopMatrix();


	// Left arm
	glPushMatrix();
	glTranslated(baseRadius * 0.5, -0.1, 0);
	glRotated(90 - currentSwingAngle, 1, 0, 0);
	gluCylinder(quad, 0.02, 0.02, 0.15, 30, 30);
	glPopMatrix();

	glPopMatrix();

	gluDeleteQuadric(quad);
}

//Boundary Walls Alternating Colors f(n) = updateTimer
void drawBackWall(double thickness) {

	setMaterialColor(additionalWallColors[currentWallColorIndex][0], additionalWallColors[currentWallColorIndex][1], additionalWallColors[currentWallColorIndex][2], additionalWallColors[currentWallColorIndex][3][0]);

	glPushMatrix();
	glRotated(-90, 1.0, 0.0, 0.0);
	glTranslated(0.5, 0.75, 0.5);
	glScaled(3.0, thickness, 1.0);
	glutSolidCube(1);
	glPopMatrix();

	GLUquadric* quad = gluNewQuadric();
	for (int i = 0; i < 8; i++) {
		int colorIndex = (i / 2) % 4;
		setMaterialColor(wallColors[colorIndex][0], wallColors[colorIndex][1], wallColors[colorIndex][2], wallColors[colorIndex][3][0]);

		glPushMatrix();
		glRotated(90, 1.0, 0, 0);
		glTranslated(i * (1.5 / 8) - 0.15, -0.75, -1.0);
		gluCylinder(quad, thickness, thickness, 1.0, 20, 20);
		glPopMatrix();
	}

	gluDeleteQuadric(quad);
}
void drawSideWall(double thickness, float transX) {

	setMaterialColor(additionalWallColors[currentWallColorIndex][0], additionalWallColors[currentWallColorIndex][1], additionalWallColors[currentWallColorIndex][2], additionalWallColors[currentWallColorIndex][3][0]);

	glPushMatrix();
	glRotated(90, 0, 0, 1.0);
	glTranslated(0.5, transX , 0.5);
	glScaled(1.0, thickness, 2.5);
	glutSolidCube(1);
	glPopMatrix();

	GLUquadric* quad = gluNewQuadric();
	for (int i = 0; i < 8; i++) {
		int colorIndex = (i / 2) % 4;
		setMaterialColor(wallColors[colorIndex][0], wallColors[colorIndex][1], wallColors[colorIndex][2], wallColors[colorIndex][3][0]);

		glPushMatrix();
		glRotated(90, 0, 0, 1.0);
		glRotated(90, 0, 1.0, 0);
		glTranslated(-0.5 + i * (2.0 / 8) - 1.0, transX, 0.0);
		gluCylinder(quad, thickness, thickness, 1.0, 20, 20);
		glPopMatrix();
	}
	
	gluDeleteQuadric(quad);
}

//Ground
void drawGround(double thickness) {

	GLfloat groundAmbient[] = { 0.1f, 0.4f, 0.1f, 1.0f };
	GLfloat groundDiffuse[] = { 0.0f, 0.6f, 0.0f, 1.0f };
	GLfloat groundSpecular[] = { 0.1f, 0.3f, 0.1f, 1.0f };
	GLfloat groundShininess = 10.0f;
	setMaterialColor(groundAmbient, groundDiffuse, groundSpecular, groundShininess);

	glPushMatrix();
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(3.0, thickness, 2.5);
	glutSolidCube(1);
	glPopMatrix();
}

//Goal Target Scaling f(n) = updateAnimations
void drawTargetLeg() {
	
	GLUquadric* quad = gluNewQuadric();

	GLfloat legAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat legDiffuse[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat legSpecular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat legShininess = 5.0f;
	setMaterialColor(legAmbient, legDiffuse, legSpecular, legShininess);

	gluCylinder(quad, 0.01, 0.01, 0.4, 20, 20);
	gluDeleteQuadric(quad);
}
void drawTargetRings() {
	GLfloat colors[4][4] = {
		{1.0f, 0.0f, 0.0f, 1.0f},  // Red
		{1.0f, 1.0f, 0.0f, 1.0f},  // Yellow
		{0.0f, 0.0f, 1.0f, 1.0f},  // Blue
		{0.0f, 0.0f, 0.0f, 1.0f}   // Black
	};

	GLUquadric* quad = gluNewQuadric();

	glDisable(GL_LIGHTING);

	glPushMatrix();
	glScalef(scaleFactor, scaleFactor, scaleFactor);

	glColor4fv(colors[3]);
	gluDisk(quad, 0.09, 0.12, 50, 1);

	glColor4fv(colors[2]);
	gluDisk(quad, 0.06, 0.09, 50, 1);

	glColor4fv(colors[1]);
	gluDisk(quad, 0.03, 0.06, 50, 1);

	glColor4fv(colors[0]);
	gluDisk(quad, 0.0, 0.03, 50, 1);

	glPopMatrix();

	glEnable(GL_LIGHTING);

	gluDeleteQuadric(quad);
}
void drawTarget(double topWid, double topThick) {
	
	GLfloat topAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat topDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat topSpecular[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat topShininess = 50.0f;
	setMaterialColor(topAmbient, topDiffuse, topSpecular, topShininess);

	// Target
	glPushMatrix();
	glTranslated(0.5, 0.3, 0);
	glRotated(75, 1, 0, 0);
	glScaled(topWid, topThick, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.5, 0.3 + topThick / 2, topThick);
	glRotated(-15, 1, 0, 0);
	drawTargetRings();
	glPopMatrix();

	glPushMatrix();

	// Front left leg
	glPushMatrix();
	glTranslated(0.4 , 0.0,  -0.15);
	glRotated(-75, 1, 0, 0);
	drawTargetLeg();
	glPopMatrix();

	// Front right leg
	glPushMatrix();
	glTranslated(0.6 , 0.0, -0.15);
	glRotated(-75, 1, 0, 0);
	drawTargetLeg();
	glPopMatrix();

	// Back left leg
	glPushMatrix();
	glTranslated(0.4 , 0.0,  0.06);
	glRotated(-105, 1, 0, 0);
	drawTargetLeg();
	glPopMatrix();

	// Back right leg
	glPushMatrix();
	glTranslated(0.6 , 0.0, 0.06);
	glRotated(-105, 1, 0, 0);
	drawTargetLeg();
	glPopMatrix();

	glPopMatrix();
}

void moveArrow() {
	if (arrowMoving == true) {

		float radians = DEG2RAD(-arrowYaw * 0.35);

		double deltaX = 0.0;
		double deltaZ = 0.0;
		double deltaY = 0.0;

		deltaX = 0.1 * sin(radians);
		deltaZ = 0.1 * cos(radians);
		deltaY = 0.1 * sin(DEG2RAD(-arrowPitch * 0.35));

		while (!(arrowX + deltaX < -1.0 || arrowX + deltaX > 2.0 || arrowZ + deltaZ < 0.15)) {
			arrowX += deltaX;
			arrowZ -= deltaZ;
			arrowY += deltaY;
		}

		calculateScore();
	}
}
void drawReadyArrow() {

	if (isMoving && !arrowMoving) {
		arrowX += playerDeltaX;
		arrowZ += playerDeltaZ;
	}

	glPushMatrix();

	if (!arrowMoving) {
		glRotated(arrowYaw * 0.35, 0, 1, 1);
	}

	glPushMatrix();
	setMaterialColor(arrowColor[0][0], arrowColor[0][1], arrowColor[0][2], arrowColor[0][3][0]);
	glTranslated(arrowX, arrowY, arrowZ);
	GLUquadric* body = gluNewQuadric();
	gluCylinder(body, 0.005, 0.005, 0.15, 20, 20);
	glPopMatrix();

	glPushMatrix();
	setMaterialColor(arrowColor[1][0], arrowColor[1][1], arrowColor[1][2], arrowColor[1][3][0]);
	glTranslated(arrowX, arrowY, arrowZ);
	glRotated(180, 0, 1, 0);
	glutWireCone(0.01, 0.06, 50, 50);
	glPopMatrix();

	glPushMatrix();
	setMaterialColor(arrowColor[2][0], arrowColor[2][1], arrowColor[2][2], arrowColor[2][3][0]);
	glTranslated(arrowX, arrowY, arrowZ + 0.15);
	glRotated(45, 0, 1, 0);
	glutWireCone(0.01, 0.04, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslated(arrowX, arrowY, arrowZ + 0.15);
	glRotated(-45, 0, 1, 0);
	glutWireCone(0.01, 0.04, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslated(arrowX, arrowY, arrowZ + 0.15);
	glutWireCone(0.01, 0.04, 50, 50);
	glPopMatrix();

	glPopMatrix();

}
void drawCrossHair() {
	float radians = DEG2RAD(-arrowYaw * 0.35);

	double deltaX = 0.0;
	double deltaZ = 0.0;
	double deltaY = 0.0;

	deltaX = 0.1 * sin(radians);
	deltaZ = 0.1 * cos(radians);
	deltaY = 0.1 * sin(DEG2RAD(-arrowPitch * 0.35));

	float tempX = arrowX, tempZ = arrowZ, tempY = arrowY;

	if (isMoving && !arrowMoving) {
		tempX = arrowX - playerDeltaX;
		tempZ = arrowZ - playerDeltaZ;
		isMoving = false;
	}

	while (!(tempX + deltaX < -1.0 || tempX + deltaX > 2.0 || tempZ + deltaZ < 0.15)) {
		tempX += deltaX;
		tempZ -= deltaZ;
		tempY += deltaY;
	}

	if (view == 'f') {
		glDisable(GL_DEPTH_TEST);

		glColor3f(1.0f, 1.0f, 1.0f);

		glPushMatrix();
		glTranslatef(tempX, tempY, tempZ);

		glPushMatrix();
		glScalef(0.04f, 0.005f, 0.005f);
		glutSolidCube(1.0f);
		glPopMatrix();

		glPushMatrix();
		glScalef(0.005f, 0.04f, 0.005f);
		glutSolidCube(1.0f);
		glPopMatrix();

		glPopMatrix();

		glEnable(GL_DEPTH_TEST);
	}
}

//Objects

//Minor 1 Spectators Alternating Colors f(n) = updateTimer
void drawSpectators(double baseWidth, double baseHeight, double stepHeight, double stepDepth, double xPos, double zPos) {

	GLfloat stairAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat stairDiffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat stairSpecular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat stairShininess = 10.0f;


	for (int i = 0; i < 5; i++) {
		setMaterialColor(stairAmbient, stairDiffuse, stairSpecular, stairShininess);

		glPushMatrix();
		glTranslated(xPos, baseHeight + i * stepHeight, zPos - i * stepDepth);
		glScaled(baseWidth, stepHeight, stepDepth);
		glutSolidCube(1);
		glPopMatrix();

		if (i % 2 == 0) {
			for (int j = 0; j < 4; j++) {

				setMaterialColor(wallColors[currentSeatColorIndex][0], wallColors[currentSeatColorIndex][1], wallColors[currentSeatColorIndex][2], wallColors[currentSeatColorIndex][3][0]);
				double offset = (j * 0.2) - 0.3;

				// Seat
				glPushMatrix();
				glTranslated(xPos + offset, baseHeight + i * stepHeight + 0.045, zPos - i * stepDepth + 0.04);
				glRotated(90, 1, 0, 0);
				GLUquadric* quad = gluNewQuadric();
				gluCylinder(quad, 0.03, 0.1, 0.02, 50, 1);
				glPopMatrix();
				gluDeleteQuadric(quad);

				// Backrest
				glPushMatrix();
				glTranslated(xPos + offset, baseHeight + i * stepHeight + 0.06, zPos - i * stepDepth - 0.05);
				gluCylinder(quad, 0.03, 0.1, 0.02, 50, 1);
				glPopMatrix();
			}
		}
	}
}

//Minor 2 Bow Translating on Player Movement
void drawBow() {

	GLfloat bowAmbient[] = { 0.4f, 0.2f, 0.1f, 1.0f };
	GLfloat bowDiffuse[] = { 0.6f, 0.3f, 0.1f, 1.0f };
	GLfloat bowSpecular[] = { 0.2f, 0.1f, 0.05f, 1.0f };
	GLfloat bowShininess = 12.0f;

	GLfloat stringAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat stringDiffuse[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat stringSpecular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat stringShininess = 5.0f;

	glPushMatrix();

	glRotated(arrowYaw * 0.35, 1, 1, 1);

	float deltaX = bowX - playerX;

	glPushMatrix();
	setMaterialColor(bowAmbient, bowDiffuse, bowSpecular, bowShininess);
	glTranslated(bowX - deltaX, bowY - 0.1, playerZ - 0.15);
	glRotated(75, 1, 0, 0);
	GLUquadric* body = gluNewQuadric();
	gluCylinder(body, 0.005, 0.005, 0.15, 20, 20);
	glPopMatrix();

	glPushMatrix();
	setMaterialColor(bowAmbient, bowDiffuse, bowSpecular, bowShininess);
	glTranslated(bowX - deltaX, bowY + 0.05, playerZ - 0.15);
	glRotated(90, 1, 0, 0);
	gluCylinder(body, 0.005, 0.005, 0.15, 20, 20);
	glPopMatrix();

	glPushMatrix();
	setMaterialColor(bowAmbient, bowDiffuse, bowSpecular, bowShininess);
	glTranslated(bowX - deltaX, bowY + 0.19, playerZ - 0.1125);
	glRotated(105, 1, 0, 0);
	gluCylinder(body, 0.005, 0.005, 0.15, 20, 20);
	glPopMatrix();

	glPushMatrix();
	setMaterialColor(stringAmbient, stringDiffuse, stringSpecular, stringShininess);
	glTranslated(bowX - deltaX, bowY + 0.2, playerZ - 0.1125);
	glRotated(90, 1, 0, 0);
	gluCylinder(body, 0.002, 0.002, 0.43, 20, 20);
	glPopMatrix();

	glPopMatrix();
}

//Minor 3 Clocks Rotating f(n) = updateTimer
void drawClock(float transX) {
	GLfloat clockCol[4][4] = { {1.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 0.0f, 1.0f},
	};
	GLUquadric* quad = gluNewQuadric();

	glDisable(GL_LIGHTING);

	glPushMatrix();
	glColor4fv(clockCol[0]);

	glTranslated(transX, 0.65, -0.725);
	gluDisk(quad, 0.0, 0.09, 50, 1);

	glColor4fv(clockCol[1]);
	gluDisk(quad, 0.09, 0.1, 50, 1);

	glRotated(90, 0, 1, 0);
	glRotated(-90, 1, 0, 0);
	glRotated(timeRotate, 1, 0, 0);
	glTranslated(0, 0, 0.025);
	gluCylinder(quad, 0.005, 0.005, 0.06, 50, 50);

	glPopMatrix();

	glEnable(GL_LIGHTING);
}

//Major 1 Arrow Bag Scaling f(n) = updateAnimations
void drawInventoryArrow(double x, double z) {

	glPushMatrix();
	setMaterialColor(arrowColor[0][0], arrowColor[0][1], arrowColor[0][2], arrowColor[0][3][0]);
	glTranslated(playerX + x, playerY - 0.1, playerZ + z);
	glRotated(90, 1, 0, 0);
	glScalef(scaleFactor * 0.8, scaleFactor * 0.8, scaleFactor * 0.8);
	GLUquadric* body = gluNewQuadric();
	gluCylinder(body, 0.005, 0.005, 0.15, 20, 20);
	glPopMatrix();

	glPushMatrix();
	setMaterialColor(arrowColor[1][0], arrowColor[1][1], arrowColor[1][2], arrowColor[1][3][0]);
	glTranslated(playerX + x, playerY - 0.25, playerZ + z);
	glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	glScalef(scaleFactor * 0.8, scaleFactor * 0.8, scaleFactor * 0.8);
	glutWireCone(0.01, 0.06, 50, 50);
	glPopMatrix();

	glPushMatrix();
	setMaterialColor(arrowColor[2][0], arrowColor[2][1], arrowColor[2][2], arrowColor[2][3][0]);
	glTranslated(playerX + x, playerY - 0.11, playerZ + z);
	glRotated(45, 0, 0, 1);
	glRotated(-90, 1, 0, 0);
	glScalef(scaleFactor * 0.8, scaleFactor * 0.8, scaleFactor * 0.8);
	glutWireCone(0.01, 0.04, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslated(playerX + x, playerY - 0.11, playerZ + z);
	glRotated(-45, 0, 0, 1);
	glRotated(-90, 1, 0, 0);
	glScalef(scaleFactor * 0.8, scaleFactor * 0.8, scaleFactor * 0.8);
	glutWireCone(0.01, 0.04, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslated(playerX + x, playerY -0.11, playerZ + z);
	glRotated(-90, 1, 0, 0);
	glScalef(scaleFactor * 0.8, scaleFactor * 0.8, scaleFactor * 0.8);
	glutWireCone(0.01, 0.04, 50, 50);
	glPopMatrix();

}
void drawArrowBag(double x, double z) {

	GLfloat havanAmbient[] = { 0.25f, 0.14f, 0.09f, 1.0f };
	GLfloat havanDiffuse[] = { 0.56f, 0.28f, 0.18f, 1.0f };
	GLfloat havanSpecular[] = { 0.3f, 0.15f, 0.09f, 1.0f };
	GLfloat havanShininess = 15.0f;
	
	glPushMatrix();
	setMaterialColor(havanAmbient, havanDiffuse, havanSpecular, havanShininess);
	glTranslated(playerX + x, playerY - 0.1, playerZ + z);
	glRotated(90, 1, 0, 0);
	glScalef(scaleFactor * 0.8, scaleFactor * 0.8, scaleFactor * 0.8);
	GLUquadric* body = gluNewQuadric();
	gluCylinder(body, 0.025, 0.025, 0.2, 20, 20);
	glPopMatrix();

	drawInventoryArrow(0.04, 0.065);
	drawInventoryArrow(0.05, 0.08);
	drawInventoryArrow(0.06, 0.065);
}

//Major 2 Olympic Flame Rotating f(n) = updateAnimations
void drawOlympicFlame(double baseRadius, double topRadius, double columnHeight) {

	GLUquadric* quad = gluNewQuadric();

	GLfloat columnAmbient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat columnDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat columnSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat columnShininess = 30.0f;
	setMaterialColor(columnAmbient, columnDiffuse, columnSpecular, columnShininess);

	glPushMatrix();
	glTranslated(-0.7, (columnHeight / 2) + 0.3, 0.625);
	glRotated(90, 1, 0, 0);
	gluCylinder(quad, baseRadius * 0.65, topRadius, columnHeight, 30, 30); 
	glPopMatrix();

	gluDeleteQuadric(quad);

	GLUquadric* quad2 = gluNewQuadric();

	glPushMatrix();
	glTranslated(-0.7, (columnHeight / 3) + 0.25 , 0.625);
	glRotated(-90, 1, 0, 0); 
	gluCylinder(quad2, baseRadius * 0.2, baseRadius, 0.2, 30, 30);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.7, (columnHeight / 3) + 0.25, 0.625);
	glRotated(-90, 1, 0, 0);
	gluDisk(quad2, 0, baseRadius * 0.2, 30, 1);
	glPopMatrix();

	gluDeleteQuadric(quad2);

	GLUquadric* quad3 = gluNewQuadric();

	glPushMatrix();
	glTranslated(-0.7, (columnHeight / 2) + 0.35, 0.625);
	for (int i = 0; i < 3; i++) {
		double randomRadiusFactor = 0.05 * (rand() % 10) / 100.0;
		double layerRadius = baseRadius * (0.5 - i * 0.15) + randomRadiusFactor;

		setMaterialColor(flameColors[i][0], flameColors[i][1], flameColors[i][2], flameColors[i][3][0]);
		gluSphere(quad3, layerRadius, 20, 20);
		glTranslated(0, 0.05, 0); 
		for (int j = 0; j < 5; j++) {
			int r = rand() % 5;
			glPushMatrix();
			glRotated(rotationAngles[r][0], 1, 0, 0);
			glRotated(rotationAngles[r][1], 0, 1, 0);
			glRotated(rotationAngles[r][2], 0, 0, 1);
			gluDisk(quad3, 0, layerRadius, 20, 1);
			glPopMatrix();
		}
	}
	glPopMatrix();

	gluDeleteQuadric(quad3);
}

//Lights
void setupLights() {
	GLfloat globalAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

	GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lightPosition[] = { 0.5f, 1.5f, 1.5f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

//Camera
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glRotatef(cameraYaw, 0.0f, 1.0f, 0.0f);

	glRotatef(cameraPitch, 1.0f, 0.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();

	playerYaw = -cameraYaw;
	updatePlayerRotation();
}

void Display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (gameEnd) {
		drawGameEndScreen();
	}
	else {
		setupCamera();
		setupLights();

		//Player
		drawPlayer(0.4, 0.2);

		//Walls / Fences
		drawSideWall(0.02, 1);
		drawSideWall(0.02, -2);
		drawBackWall(0.02);

		//Ground
		drawGround(0.02);

		//Goal
		drawTarget(0.3, 0.02);

		//Objects
		drawSpectators(0.75, 0.05, 0.05, 0.1, -0.6, -0.3);
		drawSpectators(0.75, 0.05, 0.05, 0.1, 1.6, -0.3);
		drawClock(-0.6);
		drawClock(1.6);
		drawOlympicFlame(0.1, 0.1, 0.6);
		drawArrowBag(0.05, 0.07);
		drawBow();
		if (arrowReady)
			drawReadyArrow();
		drawCrossHair();

	}

	glFlush();
	glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y) {
	float d = 0.01;

	switch (key) {
	case 'q': case 'Q':
		camera.moveZ(d);
		break;
	case 'e': case 'E':
		camera.moveZ(-d);
		break;
	case '1':
		camera = frontCamera;
		view = 'f';
		break;
	case '2':
		camera = topCamera;
		view = 't';
		break;
	case '3':
		camera = sideCamera;
		view = 's';
		break;
	case 'm': case 'M':
		if(!arrowMoving)
			animation = !animation;
		break;
	case 'f': case 'F':
		flame = !flame;
		break;
	case 'r': case 'R':
		if (!arrowReady) {
			arrowReady = true;
		}	
		break;
	case 'w': case 'W':
		playerMoveDirection = 'W';
		movePlayer(-playerSpeed,1); // Move forward
		break;
	case 's': case 'S':
		playerMoveDirection = 'S';
		movePlayer(playerSpeed,-1); // Move backward
		break;
	case 'a': case 'A':
		playerMoveDirection = 'A';
		movePlayer(-playerSpeed, 2); // Move left
		break;
	case 'd': case 'D':
		playerMoveDirection = 'D'; 
		movePlayer(playerSpeed, -2); // Move right
		break;
	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}
void Special(int key, int x, int y) {
	float d = 0.01;

	switch (key) {
	case GLUT_KEY_UP:
		if (view == 't') {
			camera.moveZ(d);
		}
		camera.moveY(d);
		break;
	case GLUT_KEY_DOWN:
		if (view == 't') {
			camera.moveZ(-d);
		}
		camera.moveY(-d);
		break;
	case GLUT_KEY_LEFT:
		camera.moveX(d);
		break;
	case GLUT_KEY_RIGHT:
		camera.moveX(-d);
		break;
	}

	glutPostRedisplay();
}

void mouseMovement(int x, int y) {

	if (firstMouse) {
		lastMouseX = x;
		lastMouseY = y;
		firstMouse = false;
	}

	float deltaX = x - lastMouseX;
	float deltaY = y - lastMouseY;

	cameraYaw += deltaX * 0.1f;
	cameraPitch += deltaY * 0.1f; 

	if (cameraPitch > 89.0f) cameraPitch = 89.0f;
	if (cameraPitch < -89.0f) cameraPitch = -89.0f;

	lastMouseX = x;
	lastMouseY = y;

	glutPostRedisplay();
}
void onMouseClick(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if(arrowReady) {
			arrowMoving = true;
			moveArrow();
			animation = false;
		}
	}
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);

	int windowPosX = (screenWidth - 720) / 2;
	int windowPosY = (screenHeight - 480) / 2;

	glutInitWindowSize(720, 480);
	glutInitWindowPosition(windowPosX, windowPosY);

	glutCreateWindow("Olympics Archery");

	glutTimerFunc(1000, updateTimer, 0);
	glutTimerFunc(16, updateAnimations, 0);

	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutPassiveMotionFunc(mouseMovement);
	glutMouseFunc(onMouseClick);
	glutSpecialFunc(Special);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}