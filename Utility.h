#pragma once

#include <string>
#include <fstream>

#define OGL_4_5 0

#define SAFE_DELETE(x) if(x != nullptr){delete x; x = nullptr;}
#define SAFE_DELETE_ARRAY(x) if(x != nullptr){delete[] x; x = nullptr;}

typedef unsigned char UC8;
typedef unsigned __int32 UI32;
typedef __int32 I32;
typedef float F32;
typedef double F64;

#pragma pack(2)

typedef struct 
{
	unsigned long			biSize;
	long					biWidth;
	long					biHeight;
	unsigned short			biPlanes;
	unsigned short			biBitCount;
	unsigned long			biCompression;
	unsigned long			biSizeImage;
	long					biXPelsPerMeter;
	long					biYPelsPerMeter;
	unsigned long			biClrUsed;
	unsigned long			biClrImportant;
} PersoBITMAPINFOHEADER;


typedef struct 
{
	unsigned short	bfType;
	unsigned long   bfSize;
	unsigned short			bfReserved1;
	unsigned short			bfReserved2;
	unsigned long   bfOffBits;
} PersoBITMAPFILEHEADER;


GLuint LoadBMPImageAsTexture(const std::string& rkFilename, long &iWidth, long &iHeight);

#pragma pack()

template<typename T>
T Lerp(const T& P1, const T& P2, float t)
{
	return (1 - t) * P1 + t * P2;
}