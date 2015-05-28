// RGBpixmap.h: a class to support working with RGB pixmaps.
#ifndef _RGBPIXMAP
#define _RGBPIXMAP

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <windows.h>
#include <GL/glut.h>
#endif

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#include <assert.h>
#include <math.h>
#include <stdlib.h>

typedef unsigned char uchar;

class mRGB{   // the name RGB is already used by Windows
public: 
	uchar		r, g, b;

				mRGB(){r = g = b = 0;}
				mRGB(mRGB& p){r = p.r; g = p.g; b = p.b;}
				mRGB(uchar rr, uchar gg, uchar bb){r = rr; g = gg; b = bb;}

	void		set(uchar rr, uchar gg, uchar bb){r = rr; g = gg; b = bb;}
};

class mRGBA{
public: 
	uchar		r, g, b, a;

				mRGBA(){r = g = b = 0; a = 255;}
				mRGBA(mRGBA& p){r = p.r; g = p.g; b = p.b; a = p.a;}
				mRGBA(uchar rr, uchar gg, uchar bb, uchar aa){r = rr; g = gg; b = bb; a = aa;}

	void		set(uchar rr, uchar gg, uchar bb, uchar aa){r = rr; g = gg; b = bb; a = aa;}
};

//@@@@@@@@@@@@@@@@@@ IntPoint class @@@@@@@@@@@@@@@@
class IntPoint{ // for 2D points with integer coordinates
public:
	int			x, y;

				IntPoint(int xx, int yy){x = xx; y = yy;}
				IntPoint(){ x = y = 0;}

	void		set(int dx, int dy){x = dx; y = dy;}
	void		set(IntPoint& p){ x = p.x; y = p.y;}
};

// @@@@@@@@@@@@@@@@@@@@@@@@ IntRect class @@@@@@@@@@@@@@@@@@@@
class IntRect{ // a rectangle with integer border values
public:
	int			left, top, right, bott;

				IntRect(){left = top = right = bott = 0;}
				IntRect(int l, int t, int r, int b) {left = l; top = t; right = r; bott = b;}

	void		set(int l, int t, int r, int b) {left = l; top = t; right = r; bott = b;}
	void		set(IntRect& r) {left = r.left; top = r.top; right = r.right; bott = r.bott;}
};

//$$$$$$$$$$$$$$$$$ RGBPixmap class $$$$$$$$$$$$$$$
class RGBpixmap {
private:
	mRGB* pixel; // array of pixels

public:
	int nRows, nCols; // dimensions of the pixmap.  nCols: width, nRows: height
	int w() const { return nCols;}
	int h() const { return nRows;}

	RGBpixmap() {nRows = nCols = 0; pixel = 0;}
	RGBpixmap(int rows, int cols) //constructor
	{
		nRows = rows;
		nCols = cols;
		pixel = new mRGB[rows*cols];
	}
	int readBMPFile(string fname); // read BMP file into this pixmap
	void freeIt() // give back memory for this pixmap
	{
		delete []pixel; nRows = nCols = 0;
	}
	//<<<<<<<<<<<<<<<<<< copy >>>>>>>>>>>>>>>>>>>
	void copy(IntPoint from, IntPoint to, int x, int y, int width, int height)
	{ // copy a region of the display back onto the display
		if(nRows == 0 || nCols == 0) return;
		glCopyPixels(x, y, width, height,GL_COLOR);
	}
	//<<<<<<<<<<<<<<<<<<< draw >>>>>>>>>>>>>>>>>
	void draw()
	{ // draw this pixmap at current raster position
		if(nRows == 0 || nCols == 0) return;
		//tell OpenGL: don’t align pixels to 4 byte boundaries in memory
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glDrawPixels(nCols, nRows,GL_RGB, GL_UNSIGNED_BYTE,pixel);
	}

	//<<<<<<<<<<<<<<<<< read >>>>>>>>>>>>>>>>
	int read(int x, int y, int wid, int ht)
	{ // read a rectangle of pixels into this pixmap
		nRows = ht;
		nCols = wid;
		delete []pixel;
		pixel = new mRGB[nRows *nCols];
		if(!pixel) return -1;
		//tell OpenGL: don’t align pixels to 4 byte boundaries in memory
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(x, y, nCols, nRows, GL_RGB,GL_UNSIGNED_BYTE, pixel);
		return 0;
	}
	//<<<<<<<<<<<<<<<<< read from IntRect >>>>>>>>>>>>>>>>
	int read(IntRect r)
	{ // read a rectangle of pixels into this pixmap
		nRows = r.top - r.bott;
		nCols = r.right - r.left;
		delete []pixel;
		pixel = new mRGB[nRows *nCols]; if(!pixel) return -1;
		//tell OpenGL: don’t align pixels to 4 byte boundaries in memory
		glPixelStorei(GL_PACK_ALIGNMENT,1);
		glReadPixels(r.left,r.bott, nCols, nRows, GL_RGB, GL_UNSIGNED_BYTE, pixel);
		return 0;
	}
	//<<<<<<<<<<<<<< setPixel >>>>>>>>>>>>>
	void setPixel(int x, int y, mRGB color)
	{
		if(x>=0 && x <nCols && y >=0 && y < nRows)
			pixel[nCols * y + x] = color;
	}
	//<<<<<<<<<<<<<<<< getPixel >>>>>>>>>>>
	mRGB getPixel(int x, int y)
	{
		mRGB bad(255,255,255);
		assert(x >= 0 && x < nCols);
		assert(y >= 0 && y < nRows);
		return pixel[nCols * y + x];
	}
}; //end of class RGBpixmap


//RGBAPixmap class
class RGBApixmap {
private:
	mRGBA*		pixel; // array of pixels
	GLuint		textureId;
	bool		needUpdateTex;  //Need update to texture?

public:
	int			nRows, nCols; // dimensions of the pixmap.  nCols: width, nRows: height
	int			w() const { return nCols;}
	int			h() const { return nRows;}


	RGBApixmap() { nRows = nCols = 0; pixel = NULL; textureId = 0; needUpdateTex=false;}
	RGBApixmap(int rows, int cols) //constructor
	{
		nRows = rows;
		nCols = cols;
		pixel = new mRGBA[rows*cols];

		textureId = 0;
		needUpdateTex = true;
	}

	~RGBApixmap() { 
		delete []pixel; 
		// should we clear texture object??
		if(!glIsTexture(textureId)) {
			glDeleteTextures(1, &textureId);
		}
	}

	RGBApixmap(const RGBApixmap & pixmap) {
		nRows = pixmap.nRows;
		nCols = pixmap.nCols;
		pixel = new mRGBA[nRows*nCols];
		memcpy(pixel, pixmap.pixel, sizeof(mRGBA) * nRows * nCols);
		textureId = pixmap.textureId;
		needUpdateTex = true;
	}

	bool readBMPFile(string fname); // read BMP file into this pixmap

	void freeIt() // give back memory for this pixmap
	{
		delete []pixel; 
		nRows = nCols = 0;
	}

	// copy a region of the display back onto the display
	void copy(IntPoint from, IntPoint to, int x, int y, int width, int height)
	{ 
		if(nRows == 0 || nCols == 0) return;
		glCopyPixels(x, y, width, height, GL_COLOR);
	}

	void setTexture() {
		if(!glIsTexture(textureId)) {
			glGenTextures(1, &textureId);
		}
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nCols, nRows, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
		
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_NEAREST
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//GL_LINEAR

		needUpdateTex = false;
	}

	//blend
	void blend()
	{
	    if(nRows == 0 || nCols == 0) return;
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	    glEnable(GL_BLEND);
		
		//tell OpenGL: donot align pixels to 4 byte boundaries in memory
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glDrawPixels(nCols, nRows,GL_RGBA, GL_UNSIGNED_BYTE, pixel);

		glDisable(GL_BLEND);
	}

	void blendTex(int x, int y, float scalex=1.0f, float scaley=1.0f)
	{
		if(nRows == 0 || nCols == 0) return;
		if(needUpdateTex)
			setTexture();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	    glEnable(GL_BLEND);	

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glPushMatrix();
			glTranslatef(x, y, 0);
			glScalef(scalex, scaley, 1);
			glBegin(GL_QUADS);
				glTexCoord2f(0, 0); glVertex2f(0, 0);
				glTexCoord2f(1, 0); glVertex2f(nCols, 0);
				glTexCoord2f(1, 1); glVertex2f(nCols, nRows);
				glTexCoord2f(0, 1); glVertex2f(0, nRows);
			glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);

		glDisable(GL_BLEND);
	}

	//rotate along the center of image
	void blendTexRotate(int x, int y, float scalex=1.0f, float scaley=1.0f, float angle=0.0f)
	{
	    if(nRows == 0 || nCols == 0) return;

		if(needUpdateTex)
			setTexture();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	    glEnable(GL_BLEND);	

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glPushMatrix();
			glTranslatef(x+0.5*w(), y+0.5*h(), 0);
			glRotatef(angle, 0, 0, 1);
			glTranslatef(-0.5*w(), -0.5*h(), 0);
			glScalef(scalex, scaley, 1);
			glBegin(GL_QUADS);
				glTexCoord2f(0, 0); glVertex2f(0, 0);
				glTexCoord2f(1, 0); glVertex2f(nCols, 0);
				glTexCoord2f(1, 1); glVertex2f(nCols, nRows);
				glTexCoord2f(0, 1); glVertex2f(0, nRows);
			glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);

		glDisable(GL_BLEND);
	}

	// draw this pixmap at current raster position
	void draw()
	{ 
		if(nRows == 0 || nCols == 0) return;

		//tell OpenGL: donot align pixels to 4 byte boundaries in memory
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glDrawPixels(nCols, nRows,GL_RGBA, GL_UNSIGNED_BYTE, pixel);
	}

	// read a rectangle of pixels into this pixmap
	int read(int x, int y, int wid, int ht)
	{ 
		nRows = ht;
		nCols = wid;
		delete []pixel;
		pixel = new mRGBA[nRows *nCols];
		if(!pixel) return -1;
		//tell OpenGL: donot align pixels to 4 byte boundaries in memory
		glPixelStorei(GL_PACK_ALIGNMENT,1);
		glReadPixels(x, y, nCols, nRows, GL_RGBA,GL_UNSIGNED_BYTE,pixel);
		needUpdateTex = true;
		return 0;
	}

	// read a rectangle of pixels into this pixmap
	int read(IntRect r)
	{ 
		nRows = r.top - r.bott;
		nCols = r.right - r.left;
		delete []pixel;
		pixel = new mRGBA[nRows *nCols]; if(!pixel) return -1;
		//tell OpenGL: donot align pixels to 4 byte boundaries in memory
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(r.left,r.bott, nCols, nRows, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
		needUpdateTex = true;
		return 0;
	}

	//
	void setChromaKey(uchar rr, uchar gg, uchar bb)
	{
		int count=0;
		mRGBA temp;
		for(int row=0; row<nCols; row++) {
			for(int col=0; col<nRows; col++) {
				temp=pixel[count];
				if (temp.r==rr && temp.g==gg && temp.b==bb) {
					pixel[count++].a=0;
				} else pixel[count++].a=255;
			}
		}

		setTexture();
	}


	//<<<<<<<<<<<<<< setPixel >>>>>>>>>>>>>
	void setPixel(int x, int y, mRGBA color)
	{
		if(x>=0 && x <nCols && y >=0 && y < nRows) {
			pixel[nCols * y + x] = color;
		}

		needUpdateTex = true;
	}
	//<<<<<<<<<<<<<<<< getPixel >>>>>>>>>>>
	mRGBA getPixel(int x, int y)
	{
		mRGBA bad(255,255,255,255);
		assert(x >= 0 && x < nCols);
		assert(y >= 0 && y < nRows);
		return pixel[nCols * y + x];
	}
}; //end of class RGBApixmap

#endif
