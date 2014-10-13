#include "Shield.h"

Shield::Shield(){
	init();
}
void Shield::init(){
	X = rand() % 3000 + 1500;
	Y = rand() % 300 + 100;;
	frames[0].readBMPFile("image/es1.bmp");  cout << '.';

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	frames[0].setChromaKey(255, 255, 255);

	width = frames[0].w();
	height = frames[0].h();
	Ox = X + width / 2;
	Oy = Y + height / 2;
	FrameIndex = 0;
	scale = 0;
	bigger = true;
	isEquip = false;
}
void Shield::animation(){
	if (bigger){
		scale += 0.01;
		if (scale > 1.15) bigger = false;
	}
	else{
		scale -= 0.01;
		if (scale < 0.85) bigger = true;
	}
	frames[FrameIndex].blendTex(X, Y, scale, scale);
}
void Shield::updatePosition(int playerX, int playerY){
	if (isEquip){
		Ox = playerX + 25;
		Oy = playerY + 25;
	}
	else
	{
		Ox -= 5;
		if (X <= -100)	init();
	}	
	X = Ox - scale*width / 2;
	Y = Oy - scale*height / 2;
}
void Shield::eat(int x1, int y1, int x2, int y2){
	bool isTouched = touchTest(x1, y1) || touchTest(x1, y2) || touchTest(x2, y1) || touchTest(x2, y2);
	if (isTouched){
		isEquip = true;
	}
}
void Shield::distory(int x1, int y1, int x2, int y2){
	bool isTouched = touchTest(x1, y1) || touchTest(x1, y2) || touchTest(x2, y1) || touchTest(x2, y2);

	if (isEquip && isTouched)	init();
}

bool Shield::touchTest(float x, float y){
	//http://mail.smhs.kh.edu.tw/~tch044/ciransphere/SUB-1.htm
	float x1 = X, x2 = X + width*scale;
	float y1 = Y + height*scale / 2, y2 = Y + height*scale / 2;
	return (x - x1)*(x - x2) + (y - y1)*(y - y2) > 0 ? false : true;
}