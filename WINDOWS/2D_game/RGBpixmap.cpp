// RGBpixmap.cpp - routines to read a BMP file
#include "RGBpixmap.h"
typedef unsigned short ushort;
typedef unsigned long ulong;

//<<<<<<<<<<<<<<<<<<<<< getShort >>>>>>>>>>>>>>>>>>>>
inline ushort getShort(fstream &inf) //helper function
{ //BMP format uses little-endian integer types
  // get a 2-byte integer stored in little-endian form
	char ic;
	ushort ip;
	inf.get(ic); ip = ic;  //first byte is little one 
	inf.get(ic);  ip |= ((ushort)ic << 8); // or in high order byte

	return ip;
}
//<<<<<<<<<<<<<<<<<<<< getLong >>>>>>>>>>>>>>>>>>>
ulong getLong(fstream &inf) //helper function
{  //BMP format uses little-endian integer types
   // get a 4-byte integer stored in little-endian form
	ulong ip = 0;
	char ic = 0;
	unsigned char uc = ic;
	inf.get(ic); uc = ic; ip = uc;
	inf.get(ic); uc = ic; ip |=((ulong)uc << 8);
	inf.get(ic); uc = ic; ip |=((ulong)uc << 16);
	inf.get(ic); uc = ic; ip |=((ulong)uc << 24);

	return ip;
}

/* --------------------------------------------------------------------
	RGBPixmap:
   -------------------------------------------------------------------- */


//<<<<<<<<<<<<<<<<<< RGBPixmap:: readBmpFile>>>>>>>>>>>>>
int RGBpixmap::readBMPFile(string fname) 
{  // Read into memory an mRGB image from an uncompressed BMP file.
	// return 0 on failure, 1 on success
	fstream inf; 
	inf.open(fname.c_str(), ios::in|ios::binary); //read binary char's
	if(!inf){ cout << " can't open file: " << fname << endl; return 0;}
	int k, row, col, numPadBytes, nBytesInRow;
	// read the file header information
	char ch1, ch2;
	inf.get(ch1); inf.get(ch2); //type: always 'BM'
	ulong fileSize =      getLong(inf);
	ushort reserved1 =    getShort(inf);    // always 0
	ushort reserved2= 	getShort(inf);     // always 0 
	ulong offBits =		getLong(inf); // offset to image - unreliable
	ulong headerSize =     getLong(inf);     // always 40
	ulong numCols =		getLong(inf); // number of columns in image
	ulong numRows = 		getLong(inf); // number of rows in image
	ushort planes=    	getShort(inf);      // always 1 
	ushort bitsPerPixel=   getShort(inf);    //8 or 24; allow 24 here
	ulong compression =    getLong(inf);      // must be 0 for uncompressed 
	ulong imageSize = 	getLong(inf);       // total bytes in image 
	ulong xPels =    	getLong(inf);    // always 0 
	ulong yPels =    	getLong(inf);    // always 0 
	ulong numLUTentries = getLong(inf);    // 256 for 8 bit, otherwise 0 
	ulong impColors = 	getLong(inf);       // always 0 
	if(bitsPerPixel != 24) 
	{ // error - must be a 24 bit uncompressed image
		cout << "not a 24 bit/pixelimage, or is compressed!\n";
		inf.close(); return 0;
	} 
	//add bytes at end of each row so total # is a multiple of 4
	// round up 3*numCols to next mult. of 4
	nBytesInRow = ((3 * numCols + 3)/4) * 4;
	numPadBytes = nBytesInRow - 3 * numCols; // need this many
	nRows = numRows; // set class's data members
	nCols = numCols;
    pixel = new mRGB[nRows * nCols]; //make space for array
	if(!pixel) return 0; // out of memory!
	long count = 0;
	char dum;
    char *buffer= new char[nCols*3];
	for(row = 0; row < nRows; row++) // read pixel values
	{
		inf.read(buffer, nCols*3);
		for(col = 0; col < nCols; col++)
		{
			//char r,g,b;
			//inf.get(b); inf.get(g); inf.get(r); //read bytes
			pixel[count].r = buffer[3*col+2]; //place them in colors
			pixel[count].g = buffer[3*col+1];
			pixel[count++].b = buffer[3*col];
		}
   		for(k = 0; k < numPadBytes ; k++) //skip pad bytes at row's end
			inf >> dum;
	}

	delete [] buffer;
	inf.close(); 
	return 1; // success
}


/* --------------------------------------------------------------------
	RGBAPixmap:
   -------------------------------------------------------------------- */


//RGBAPixmap:: readBmpFile
bool RGBApixmap:: readBMPFile(string fname) 
{  // Read into memory an mRGB image from an uncompressed BMP file.
	// return 0 on failure, 1 on success
	fstream inf; 
	inf.open(fname.c_str(), ios::in|ios::binary); //read binary char's
	if(!inf) { 
		cout << " can't open file: " << fname << endl; return 0;
	}

	int k, row, col, numPadBytes, nBytesInRow;
	// read the file header information
	char ch1, ch2;
	inf.get(ch1); inf.get(ch2); //type: always 'BM'
	ulong fileSize =      getLong(inf);
	ushort reserved1 =    getShort(inf);    // always 0
	ushort reserved2= 	getShort(inf);     // always 0 
	ulong offBits =		getLong(inf); // offset to image - unreliable
	ulong headerSize =     getLong(inf);     // always 40
	ulong numCols =		getLong(inf); // number of columns in image
	ulong numRows = 		getLong(inf); // number of rows in image
	ushort planes=    	getShort(inf);      // always 1 
	ushort bitsPerPixel=   getShort(inf);    //8 or 24; allow 24 here
	ulong compression =    getLong(inf);      // must be 0 for uncompressed 
	ulong imageSize = 	getLong(inf);       // total bytes in image 
	ulong xPels =    	getLong(inf);    // always 0 
	ulong yPels =    	getLong(inf);    // always 0 
	ulong numLUTentries = getLong(inf);    // 256 for 8 bit, otherwise 0 
	ulong impColors = 	getLong(inf);       // always 0 
	if(bitsPerPixel != 24) 
	{ // error - must be a 24 bit uncompressed image
		cout << "not a 24 bit/pixelimage, or is compressed!\n";
		inf.close(); return false;
	} 
	//add bytes at end of each row so total # is a multiple of 4
	// round up 3*numCols to next mult. of 4
	nBytesInRow = ((3 * numCols + 3)/4) * 4;
	numPadBytes = nBytesInRow - 3 * numCols; // need this many
	nRows = numRows; // set class's data members
	nCols = numCols;
    pixel = new mRGBA[nRows * nCols]; //make space for array
	if(!pixel) return 0; // out of memory!
	long count = 0;
	char dum;
    char *buffer= new char[nCols*3];
	for(row = 0; row < nRows; row++) // read pixel values
	{
		inf.read(buffer, nCols*3);
		for(col = 0; col < nCols; col++)
		{
			//char r,g,b;
			//inf.get(b); inf.get(g); inf.get(r); //read bytes
			pixel[count].r = buffer[3*col+2]; //place them in colors
			pixel[count].g = buffer[3*col+1];
			pixel[count++].b = buffer[3*col];
		}
   		for(k = 0; k < numPadBytes ; k++) //skip pad bytes at row's end
			inf >> dum;
	}

	needUpdateTex = true;

	delete [] buffer;
	inf.close(); 
	return true; // success
}