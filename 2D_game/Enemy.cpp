#include "Enemy.h"

Enemy::Enemy(){
	init();
}
void Enemy::loadFrames(){
	frames[0].readBMPFile("image/enemy-1.bmp");  cout << '.';
	frames[1].readBMPFile("image/enemy-2.bmp");  cout << '.';
	frames[2].readBMPFile("image/enemy-3.bmp");  cout << endl;
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	for (int i = 0; i<3; i++) frames[i].setChromaKey(255, 255, 255);
}
void Enemy::init(){
	X = 1400;
	Y = rand() % 500 + 100;

	width = frames[0].w();
	height = frames[0].h();

	FrameIndex = 0;
	speed = rand() % 8 + 3;
	isHit = false;
	gravityCounter = 0;
}
void Enemy::animation(){
	switch (FrameIndex)
	{
	case 0:
	case 1:
		FrameIndex++;
		break;
	case 2:
		FrameIndex = 0;
		break;
	default:
		FrameIndex = 2;
		break;
	}
	frames[FrameIndex].blendTex(X, Y);
}
void Enemy::updatePosition(){
	if (isHit){
		gravityCounter += 0.3;
		Y -= gravityCounter;
	}
	X -= speed;

	if (X < 0)
	{
		X = 1300;
		Y = rand() % 500 + 100;
	}
	if (Y <= 100){
		init();
	}

}
bool Enemy::hitPlayer(int x1, int y1, int x2, int y2){
	bool isTouched = !(x1 > X + width || x2 < X || y1 > Y + height || y2 < Y);
	return isTouched;
}
bool Enemy::dead(int x1, int y1, int x2, int y2){
	bool isTouched = !(x1 > X + width || x2 < X || y1 > Y + height || y2 < Y);

	if (isTouched)
		isHit = true;
	return isTouched;
}



