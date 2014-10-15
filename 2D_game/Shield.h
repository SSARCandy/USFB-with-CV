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
#include <math.h>

#include "RGBpixmap.h"
using namespace std;

class Shield{
public:
	Shield();
	void init();
	void animation();
	void updatePosition(int playerX, int playerY);
	void eat(int x1, int y1, int x2, int y2);
	void distory(int x1, int y1, int x2, int y2);
	float X;
	float Y;
	bool isEquip;

private:
	bool touchTest(float x, float y);
	RGBApixmap frames;
	int FrameIndex;
	int width;
	int height;
	float scale;
	bool bigger;
	int Ox;
	int Oy;
};

