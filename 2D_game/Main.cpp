#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <windows.h>
//#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <vector>

#include "RGBpixmap.h"
#include "Shield.h"
#include "Enemy.h"
#include "Bullet.h"
using namespace std;


//  http://devmaster.net/forums/topic/7934-aabb-collision/
bool AABBtest(float ax1, float ay1, float ax2, float ay2, float bx1, float by1, float bx2, float by2)
{
	return
		ax1 > bx2 || ax2 < bx1 ||
		ay1 > by2 || ay2 < by1;
}

static void CheckError(int line)
{
	GLenum err = glGetError();
	if (err) {
		printf("GL Error %s (0x%x) at line %d\n",
			gluErrorString(err), (int)err, line);
	}
}

// Global variables for measuring time (in milli-seconds)
int				startTime;
int				prevTime;
int				updateInterval			 = 20;
//Set windows
int				screenWidth				 = 1200;
int				screenHeight			 = 600;
int				bg1X					 = 0;			// 2400 * 500
int				bg2X					 = 0;			// 1400 * 130
//Set pictures
int				i						 = 0;
RGBApixmap		pic[7];								// create two (empty) global pixmaps
//RGBApixmap		bg;
int				whichPic				 = 0;			// which pixmap to display
//Game State
int				gameState				 = 0;			// 0:Prestart   1:In game   2:GameOver 

//Set Enemies
//struct enemy    enemy[5];
Shield			shield;
Enemy			enemy[5];
//Set Bullets
Bullet			bullet[10];
int				bulletType				 = 0;
int				bullet_on_field			 = 0;
//Set Gravity
bool			isJumping				= false;
bool			pressedJump				= false;		// Record keybord "SPACE" State
int				jump_strength			= 15;
int				gravity_counter			= 0;
//Player 
int				picX = 100;
int				picY = 400;
int				DirectState = 0;  //0:right  1:left
int				Gamescore				= 0;
bool			isDead					= false;
int				sunCounter				= 0;
int             flashIntervalCounter	= 0;
//bool			isFast = false;

/***************************/
/*  Function Declaration   */
/***************************/
void init();
void initEnemy(int index);
void jump(int value);
void GameOver();
void PressSpaceToStart();



//myReshape
void myReshape(int w, int h)
{
	/* Save the new width and height */
	screenWidth = w;
	screenHeight = h;

	/* Reset the viewport... */
	glViewport(0, 0, screenWidth, screenHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, (GLfloat)screenWidth, 0.0, (GLfloat)screenHeight, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//myDisplay
void myDisplay(void)
{
	// Measure the elapsed time
	int currTime = glutGet(GLUT_ELAPSED_TIME);
	int timeSincePrevFrame = currTime - prevTime;
	int elapsedTime = currTime - startTime;
	prevTime = currTime;

	//if (1000 / timeSincePrevFrame > 100){
	//	Sleep(27);
	//}

	if (updateInterval > 10)
		updateInterval = 20 - Gamescore / 10;

	char fpsmss[30];
	sprintf(fpsmss, "Fps %.1f", 1000.0 / timeSincePrevFrame);

	glClear(GL_COLOR_BUFFER_BIT);

	//draw background
	glRasterPos2i(50, 50);

	if (isDead && flashIntervalCounter++ < 5){
		glColor3f(255, 255, 255);
		glRectf(0, 0, screenWidth, screenHeight);
	}
	else{
		pic[3].blendTex(bg1X, 100);
		pic[4].blendTex(bg2X, -30);
		if (isDead && picY <= 100)	
			GameOver();
	}
	pic[5].blendTex(10, 480, 1.5, 1.5);

	//prevent player keep stay at roof
	if (picY >= 500){
		if (sunCounter++ > 100){
			//GameOver();
			whichPic = 6;
			shield.init();
			isDead = true;
		}
	}
	else sunCounter = 0;

	if (picY <= 100){
		if (shield.isEquip){
			shield.distory(picX, picY, picX + pic[whichPic].w(), picY + pic[whichPic].h());
			jump(0);
		}
		else{
			isDead = true;
		}
	}

	shield.animation();
	shield.eat(picX, picY, picX + pic[whichPic].w(), picY + pic[whichPic].h());

	if (!isJumping)
		pic[whichPic].blendTexRotate(picX, picY, 1, 1, gravity_counter * -5);
	else
		pic[whichPic].blendTex(picX, picY);

	for (int i = 0; i < 5; i++){
		enemy[i].animation();
		if (shield.isEquip){
			shield.distory(enemy[i].X, enemy[i].Y, enemy[i].X + enemy[i].width, enemy[i].Y + enemy[i].height);
			if (!shield.isEquip) enemy[i].init();
		}
		if (enemy[i].hitPlayer(picX, picY, picX + pic[whichPic].w(), picY + pic[whichPic].h())) {
			isDead = true;
			enemy[i].init();
		}
	}
	for (int i = 0; i < bullet_on_field; i++){
		bullet[i].animation();
		for (int j = 0; j < 5; j++){
			if (bullet[i].type == 1){
  				if (enemy[j].dead(0, bullet[i].Y, 1200, bullet[i].Y + bullet[i].height)){
					Gamescore++;
				}
			}
			else{
				if (enemy[j].dead(bullet[i].X, bullet[i].Y, bullet[i].X + bullet[i].width, bullet[i].Y + bullet[i].height)){
					Gamescore++;
					bullet[i] = bullet[bullet_on_field - 1];
					bullet_on_field--;
				}
			}
		}
	}

	//Font
	char mss[30];
	sprintf(mss, "Scores :  %d", Gamescore);

	glColor3f(1.0, 0.0, 0.0);  //set font color
	glRasterPos2i(520, 550);    //set font start position
	for (int i = 0; i<strlen(mss); i++) 
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, mss[i]);

	glRasterPos2i(480, 10);    //set font start position
	sprintf(mss, "1~3  to Change Wepons");
	for (int i = 0; i<strlen(mss); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, mss[i]);

	glColor3f(0.0, 0.5, 0.3);  //set font color
	glRasterPos2i(10, 10);    //set font start position
	sprintf(mss, "Press \"Q\" to Exit");
	for (int i = 0; i<strlen(mss); i++) 
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, mss[i]);
	

	glRasterPos2i(screenWidth - 100, 10);    //set font start position
	for (int i = 0; i<strlen(fpsmss); i++) 
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, fpsmss[i]);

	if (gameState == 0)
		PressSpaceToStart();

	CheckError(__LINE__);
	glutSwapBuffers();
}

void GameOver(){
	gameState = 0;
	char mss[50];
	sprintf(mss, "Game  Over!!      Your Scores: %d", Gamescore);

	glColor3f(1.0, 0.0, 0.0);  //set font color
	glRasterPos2i(440, 330);    //set font start position
	for (int i = 0; i < strlen(mss); i++)  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, mss[i]);
	for (int i = 0; i < bullet_on_field; i++)  bullet[i].X = 1500;
}

void PressSpaceToStart(){
	char mss[50];
	sprintf(mss, "Press   \"Space\"   To   Start");

	glColor3f(1.0, 0.0, 0.0);  //set font color
	glRasterPos2i(465, 260);    //set font start position
	for (int i = 0; i < strlen(mss); i++)  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, mss[i]);
	for (int i = 0; i < bullet_on_field; i++)  bullet[i].X = 1500;
}

void jump(int value)
{
	whichPic = 2;
	if (value < 14) {
		isJumping = true;
		if (picY < screenHeight - 50)
			picY += jump_strength--;
		value++;
		glutTimerFunc(20, jump, value);
	}
	else {
		isJumping = false;
		jump_strength = 15;
		whichPic = 0;
	}
}

void shoot(int i){
	if (gameState == 1 && bullet_on_field < 10){
		bullet[bullet_on_field].init(picY, bulletType);
		bullet_on_field++;
	}
}

void update(int i)
{
	//Background 1 Moving
	if (bg1X >= -1200)	bg1X -= 1;
	else         		bg1X  = 0;
	//Background 2 Moving
	if (bg2X >= -100)	bg2X -= 6;
	else        		bg2X  = 0;
	
	if (picY > 100){
		if (!isJumping)
			picY = (gravity_counter < 15) ? picY - gravity_counter++ : picY - gravity_counter;
		else
			gravity_counter = 0;
	}

	for (int i = 0; i < bullet_on_field; i++){
		//bulletX[i] += 15;
		if (bullet[i].updatePosition(picY)){
			bullet[i] = bullet[bullet_on_field-1];
			bullet_on_field--;
		}
	}

	shield.updatePosition(picX, picY);

	for (int i = 0; i < 5; i++)
	{
		enemy[i].updatePosition();
	}
	if (gameState == 1){
		glutPostRedisplay();
		glutTimerFunc(updateInterval, update, 0);
	}
}

void myKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		bulletType = 0;
		break;
	case '2':
		bulletType = 1;
		break;
	case '3':
		bulletType = 2;
		break;
	case 'Q':
	case 'q':
		exit(0);
		break;
	case ' ':
		if (!pressedJump){
			if (gameState == 0){
				gameState = 1;
				update(0);
				picY = 400;
				Gamescore = 0;
				isDead = false;
				init();
			}
			if (!isDead) {
				pressedJump = true;
				gravity_counter = 0;
				jump_strength = 15;
				jump(0);
				shoot(0);
			}
		}
		break;
	}
//	glutPostRedisplay();
}
void myKeysUp(unsigned char key, int x, int y){
	if (key == ' ')
		pressedJump = false;
}

void init()
{
	//GLenum err = glewInit();
	//if (GLEW_OK != err)
	//{
	//  // Problem: glewInit failed, something is seriously wrong.
	//  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	//}

//	glutSpecialFunc(SpecialKeys);
	glutKeyboardFunc(myKeys);
	glutKeyboardUpFunc(myKeysUp);
	//	glutMouseFunc(myMouse);
	//	glutMotionFunc(mouseMove);
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myReshape);

	glShadeModel(GL_SMOOTH);
	//glEnable(GL_DEPTH_TEST);

	glClearColor(1.0f, 1.0f, 0.0f, 0.0f); //background color(1.0, 1.0, 1.0): white color

	isJumping = false;
	jump_strength = 15;
	gravity_counter = 0;
	updateInterval = 20;
	 
	picX = 100;
	picY = 400;
	flashIntervalCounter = 0;

	pressedJump = false;
	shield.init();
	//init enemies
	for (int i = 0; i < 5; i++)	enemy[i].init();
	for (int i = 0; i < 10; i++){
		bullet[i].init(picY, bulletType);
	}
//	isFast = false;
}

int main(int argc, char **argv)
{
	//FreeConsole();

	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(50, 30);
	glutCreateWindow("Ultimate Shooting Flappy Bird -- SSARCandy");

	init();

	srand(time(0));  //rand seed

	cout << "Reading sprite";
	pic[0].readBMPFile("image/bird.bmp");  cout << '.';
	pic[1].readBMPFile("image/bird.bmp");  cout << '.';
	pic[2].readBMPFile("image/bird_jump.bmp");  cout << '.';
	pic[3].readBMPFile("image/bg1.bmp"); cout << '.';
	pic[4].readBMPFile("image/bg2.bmp"); cout << '.';
	pic[5].readBMPFile("image/sun1.bmp"); cout << '.';
	pic[6].readBMPFile("image/bird_burned.bmp"); cout << '.'
	<< endl;


	for (int i = 0; i < 7; i++) pic[i].setChromaKey(255, 255, 255);
	for (int i = 0; i < 5; i++)	enemy[i].loadFrames();

	//cout<<"Reading Backgroud........"<<endl;
	//bg.readBMPFile("image/bg1.bmp");

	// Initialize the time variables
	startTime = glutGet(GLUT_ELAPSED_TIME);
	prevTime = startTime;

	glutMainLoop();

	return 0;
}