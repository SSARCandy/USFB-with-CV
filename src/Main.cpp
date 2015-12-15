//#include <windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
//#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <vector>

#include "RGBpixmap.h"
//#include "Shield.h"
#include "Enemy.h"
#include "Bullet.h"
using namespace std;
using namespace cv;


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


bool first = true;

VideoCapture cap(CV_CAP_ANY);
Mat frame,bkFrame,refFrame,diff;
Mat hotSpot_up, hotSpot_down;
Size defaultSize(1280, 720);
Size hotSpotSize(200, 200);
# define ACTIVATE_THRESHOLD 0.25
# define DEFAULT_JUMP_STRENGTH 15
const int THRESHOLDING = hotSpotSize.height*hotSpotSize.width*ACTIVATE_THRESHOLD;
bool jumpLock = false;

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
RGBApixmap		pic[8];								// create two (empty) global pixmaps
//RGBApixmap		bg;
int				whichPic				 = 0;			// which pixmap to display
//Game State
int				gameState				 = 0;			// 0:Prestart   1:In game   2:GameOver

//Set Enemies
//struct enemy    enemy[5];
//Shield			shield;
Enemy			enemy[5];
//Set Bullets
Bullet			bullet[10];
int				bulletType				 = 0;
int				bullet_on_field			 = 0;
//Set Gravity
bool			isJumping				= false;
bool			pressedJump				= false;		// Record keybord "SPACE" State
int				jump_strength			= DEFAULT_JUMP_STRENGTH;
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
void up(int value);
void down(int value);
void GameOver();
void PressSpaceToStart();



void myKeysForCV(char key)
{
    switch (key)
    {
        case 'W':
        case 'w':
            if (!pressedJump){
                if (!isDead) {
                    pressedJump = true;
                    gravity_counter = 0;
                    jump_strength = DEFAULT_JUMP_STRENGTH;
                    up(0);
                }
            }
            break;
        case 'S':
        case 's':
            if (!pressedJump){
                if (!isDead) {
                    pressedJump = true;
                    gravity_counter = 0;
                    jump_strength = DEFAULT_JUMP_STRENGTH;
                    down(0);
                }
            }
            break;
        case 'x':
            pressedJump =false;
    }
}

void detectAction() {
    if(!first){
        cap.read(frame);
        cvtColor(frame, bkFrame, CV_BGR2GRAY);
        absdiff(bkFrame, refFrame, diff);
//        threshold(diff, diff, 1, 255, CV_THRESH_BINARY);
        
        rectangle(diff, Point(3, 3), Point(200, 200), Scalar(255), 3, 8, 0);
        rectangle(diff, Point(3, 517), Point(200, 717), Scalar(255), 3, 8, 0);
        imshow("FrameDifference", diff);
        double v1, v2;
        Mat tmp = Mat::zeros(defaultSize, CV_8UC1);
        
        tmp = hotSpot_up.mul(diff/255);
//        imshow("up", tmp);
        v1 = sum(tmp)[0]/255;

        tmp = hotSpot_down.mul(diff/255);
//        imshow("down", tmp);
        v2 = sum(tmp)[0]/255;
        cout<<v2<<endl;
        if (v1 > THRESHOLDING){
            cout<<"uppp "<< v1 <<endl;
            myKeysForCV('w');
        } else if(v2 > THRESHOLDING) {
            cout<<"DOWNNNNNN"<< v2 <<endl;
            myKeysForCV('s');
        } else {
            myKeysForCV('x');
        }
    }
}

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
    detectAction();
    
    // Measure the elapsed time
	int currTime = glutGet(GLUT_ELAPSED_TIME);
	int timeSincePrevFrame = currTime - prevTime;
	int elapsedTime = currTime - startTime;
	prevTime = currTime;

	
//    if (updateInterval > 10)
//		updateInterval = 20 - Gamescore / 10;

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


    if (picY <= 100){
        isDead = true;
    }

    
	if (!isJumping)
		pic[whichPic].blendTexRotate(picX, picY, 1, 1, gravity_counter * -5);
	else
		pic[whichPic].blendTex(picX, picY);

	for (int i = 0; i < 5; i++){
		enemy[i].animation();

        if (enemy[i].hitPlayer(picX, picY, picX + pic[whichPic].w(), picY + pic[whichPic].h())) {
			isDead = true;
			enemy[i].init();
		}
	}
//	for (int i = 0; i < bullet_on_field; i++){
//		bullet[i].animation();
//		for (int j = 0; j < 5; j++){
//			if (bullet[i].type == 1){
//  				if (enemy[j].dead(0, bullet[i].Y, 1200, bullet[i].Y + bullet[i].height)){
//					Gamescore++;
//				}
//			}
//			else{
//				if (enemy[j].dead(bullet[i].X, bullet[i].Y, bullet[i].X + bullet[i].width, bullet[i].Y + bullet[i].height)){
//					Gamescore++;
//					bullet[i] = bullet[bullet_on_field - 1];
//					bullet_on_field--;
//				}
//			}
//		}
//	}

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

void up(int value)
{
    whichPic = 2;
    if (value < 5) {
        isJumping = true;
        if (picY < screenHeight - 52)
            picY += jump_strength--;
        value++;
        glutTimerFunc(40, up, value);
    }
    else {
        isJumping = false;
        jump_strength = DEFAULT_JUMP_STRENGTH;
        whichPic = 0;
    }
}

void down(int value)
{
    whichPic = 7;
    if (value < 5) {
        isJumping = true;
        if (picY > 110)
            picY -= jump_strength--;
        value++;
        glutTimerFunc(40, down, value);
    }
    else {
        isJumping = false;
        jump_strength = DEFAULT_JUMP_STRENGTH;
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

	if (isDead){
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

	for (int i = 0; i < 5; i++)
	{
		Gamescore += enemy[i].updatePosition();
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
            if (gameState == 0){
                gameState = 1;
                update(0);
                picY = 400;
                Gamescore = 0;
                isDead = false;
                init();
            }
            break;
        case 'W':
        case 'w':
            if (!pressedJump){
                if (!isDead) {
                    pressedJump = true;
                    gravity_counter = 0;
                    jump_strength = DEFAULT_JUMP_STRENGTH;
                    up(0);
                    //				shoot(0);
                }
            }
            break;
        case 'S':
        case 's':
            if (!pressedJump){
                if (!isDead) {
                    pressedJump = true;
                    gravity_counter = 0;
                    jump_strength = DEFAULT_JUMP_STRENGTH;
                    down(0);
                    //				shoot(0);
                }
            }
            break;
	}
//	glutPostRedisplay();
}
void myKeysUp(unsigned char key, int x, int y){
	if (key == 'W' || key == 'w' || key == 'S' || key == 's')
		pressedJump = false;
}

void init()
{
	glutKeyboardFunc(myKeys);
	glutKeyboardUpFunc(myKeysUp);
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myReshape);

	glShadeModel(GL_SMOOTH);

	glClearColor(1.0f, 1.0f, 0.0f, 0.0f); //background color(1.0, 1.0, 1.0): white color

	isJumping = false;
	jump_strength = DEFAULT_JUMP_STRENGTH;
	gravity_counter = 0;
	updateInterval = 20;

	picX = 100;
	picY = 400;
	flashIntervalCounter = 0;

	pressedJump = false;
	//init enemies
	for (int i = 0; i < 5; i++)	enemy[i].init();
	for (int i = 0; i < 10; i++){
		bullet[i].init(picY, bulletType);
	}

    
    hotSpot_down = Mat::zeros(defaultSize, CV_8UC1);
    hotSpot_up = Mat::zeros(defaultSize, CV_8UC1);

    hotSpot_up = imread("image/hot_up.png", CV_LOAD_IMAGE_GRAYSCALE);
    hotSpot_down = imread("image/hot_down.png", CV_LOAD_IMAGE_GRAYSCALE);
//    imshow("ll", hotSpot_up);
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
	pic[6].readBMPFile("image/bird_burned.bmp"); cout << '.';
    pic[7].readBMPFile("image/bird_dive.bmp");  cout << '.'
	<< endl;
    

    

	for (int i = 0; i < 8; i++) pic[i].setChromaKey(255, 255, 255);
	for (int i = 0; i < 5; i++)	enemy[i].loadFrames();

	
    namedWindow("Webcam", CV_WINDOW_AUTOSIZE);
    namedWindow("FrameDifference", CV_WINDOW_AUTOSIZE);
    
    while (first) {
        if(cap.read(frame)){
            cap.read(frame);
            
            diff = Mat::zeros(frame.size(), CV_8UC1);
            bkFrame = Mat::zeros(frame.size(), CV_8UC1);
            refFrame = Mat::zeros(frame.size(), CV_8UC1);
            
            cvtColor(frame, refFrame, CV_BGR2GRAY);

            first = false;
        }
    }
    
	// Initialize the time variables
	startTime = glutGet(GLUT_ELAPSED_TIME);
	prevTime = startTime;

	glutMainLoop();

	return 0;
}
