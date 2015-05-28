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

#include "RGBpixmap.hpp"
using namespace std;

class Enemy{
public:
	Enemy();
	void init();
	void loadFrames();
	void animation();
	void updatePosition();
	bool hitPlayer(int x1, int y1, int x2, int y2);
	bool dead(int x1, int y1, int x2, int y2);
	float X;
	float Y;
	int width;
	int height;

private:
	RGBApixmap frames[3];
	int FrameIndex;
	float speed;
	float gravityCounter;
	bool isHit;
};

