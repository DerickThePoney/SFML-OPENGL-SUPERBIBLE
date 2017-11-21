#include "stdafx.h"
#include "Utility.h"

GLuint LoadBMPImageAsTexture(const std::string & rkFilename, long & iWidth, long & iHeight)
{
	GLuint texture;

	//Load Img
	UC8* datBuff[2] = { nullptr, nullptr }; // Header buffers

	UC8* pixels = nullptr; // Pixels

	PersoBITMAPFILEHEADER* bmpHeader = nullptr; // Header
	PersoBITMAPINFOHEADER* bmpInfo = nullptr; // Info 

										 // The file... We open it with it's constructor
	std::ifstream file(rkFilename, std::ios::binary);
	if (!file)
	{
		std::cout << "Failure to open bitmap file.\n";

		return -1;
	}

	// Allocate byte memory that will hold the two headers
	datBuff[0] = new UC8[sizeof(PersoBITMAPFILEHEADER)];
	datBuff[1] = new UC8[sizeof(PersoBITMAPINFOHEADER)];

	file.read((char*)datBuff[0], sizeof(BITMAPFILEHEADER));
	file.read((char*)datBuff[1], sizeof(PersoBITMAPINFOHEADER));

	// Construct the values from the buffers
	bmpHeader = (PersoBITMAPFILEHEADER*)datBuff[0];
	bmpInfo = (PersoBITMAPINFOHEADER*)datBuff[1];

	// Check if the file is an actual BMP file
	if (bmpHeader->bfType != 0x4D42)
	{
		std::cout << "File \"" << rkFilename << "\" isn't a bitmap file\n";
		return 2;
	}

	// First allocate pixel memory
	unsigned long siz =  bmpInfo->biHeight*bmpInfo->biWidth * 3;
	//unsigned long sizR = 1024 * 1024 * 3; //bmpInfo->biHeight*bmpInfo->biWidth * 4;
	//UC8* pixelsTemp = new UC8[siz];
	pixels = new UC8[siz];

	// Go to where image data starts, then read in image data
	file.seekg(bmpHeader->bfOffBits);
	file.read((char*)pixels, bmpInfo->biHeight*bmpInfo->biWidth * 3);

	iHeight = bmpInfo->biHeight;
	iWidth = bmpInfo->biWidth;

	for (long i = 0; i < iHeight * iWidth * 3; i+=3)
	{
		UC8 ucTemp = pixels[i];
		pixels[i] = pixels[i+2];
		pixels[i + 2] = ucTemp;
	}


	// We're almost done. We have our image loaded, however it's not in the right format.
	// .bmp files store image data in the BGR format, and we have to convert it to RGB.
	// Since we have the value in bytes, this shouldn't be to hard to accomplish
	/*UC8 tmpRGB = 0; // Swap buffer
	for (unsigned long i = 0; i < 1024; i ++)
	{
		for (long j = 0; j < 1024; j += 3)
		{
			pixels[i*1024 + j] = (float)pixelsTemp[i * bmpInfo->biWidth + j + 2] / 255.0f;
			pixels[i * 1024 + j +1] = (float)pixelsTemp[i * bmpInfo->biWidth + j+ 1] / 255.0f;
			pixels[i * 1024 + j + 2] = (float)pixelsTemp[i * bmpInfo->biWidth + j] / 255.0f;
		}
	}

	// Set width and height to the values loaded from the file
	GLuint w = 1024;// bmpInfo->biWidth;
	GLuint h = 1024;//bmpInfo->biHeight;*/

	

	/*******************GENERATING TEXTURES*******************/

	glGenTextures(1, &texture);             // Generate a texture
	glBindTexture(GL_TEXTURE_2D, texture); // Bind that texture temporarily
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, iWidth, iHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureSubImage2D(texture, 0, 0, 0, iWidth, iHeight, GL_RGB8, GL_UNSIGNED_BYTE, pixels);
	

	GLint mode = GL_RGB;                   // Set the mode

	

	// Create the texture. We get the offsets from the image, then we use it with the image's
	// pixel data to create it.
	

	// Unbind the texture
	//glBindTexture(GL_TEXTURE_2D, NULL);

	// Output a successful message
	std::cout << "Texture \"" << rkFilename << "\" successfully loaded.\n";

	// Delete the two buffers.
	delete[] datBuff[0];
	delete[] datBuff[1];
	delete[] pixels;


	return texture;
}
