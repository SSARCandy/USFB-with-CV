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
using namespace std;

class Bullet{
private:
	float speed;
	float gravityCounter;

public:
	float X;
	float Y;
	int width;
	int height;
	int type;

	Bullet(){
		init(400, 0);
	}

	void init(int playerY, int t){
		type = t;
		switch (type){
		case 0:
			X = 100;
			Y = playerY + 15;
			width = 50;
			height = 5;
			speed = 15;
			break;
		case 1:
			X = 100;
			Y = playerY + 18;
			width = 1100;
			height = 4;
			speed = 200;
			break;
		case 2:
			X = 100;
			Y = playerY + 15;
			width = 20;
			height = 20;
			speed = rand() % 20 + 3;
			gravityCounter = 0;
			break;

		}
	}

	void animation(){
		glColor4f(1, 0, 0, 0);
		glRectf(X, Y, X + width, Y + height);
	}
	
	// Return   True: is distroy  /  False: is not distroy
	bool updatePosition(int playerY){
		switch (type){
		case 0:
		case 1:
			X += speed;
			if (X > 1200){
				init(playerY, type);
				return true;
			}
			return false;
			break;
		case 2:
			gravityCounter += 0.3;
			Y -= gravityCounter;
			X += speed;
			if (Y <= 100){
				init(playerY, type);
				return true;
			}
			return false;
			break;
		default:
			return false;
			break;
		}
	}
};
