/**
 * Author: Bc. Klara Necasova
 * Login: xnecas24
 * Date: 16. 2. 2018
 * Description: Library for GIF to BMP conversion
 * TODO: use map
 */

#pragma once

#include <algorithm>
#include <cstdint>
#include <bitset>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <map>

/**
 * true ak sa maju povolit debug vypisy
 */
#define DEBUG_ENABLE false
#define LOG if (DEBUG_ENABLE)

using namespace std;

#define GIF_FORMAT_SIZE 3
#define GIF_VERSION_SIZE 3
#define GIF_LOGICAL_SCREEN_DESCRIPTOR_SIZE 7

#define BMP_DIPHEADER_EMPTYSIZE 48

#define CONVERSION_COMPLETED 0
#define CONVERSION_ERROR -1

struct __attribute__ ((packed)) GraphicsControlExtension {
	uint8_t byteSize;
	uint8_t transparentColorFlag:1;
	uint8_t userInputFlag:1;
	uint8_t disposalMethod:3;
	uint8_t reserved:3;
	uint16_t delayTime;
	uint8_t transparentColorIndex;
	uint8_t blockTerminator;
};

struct __attribute__ ((packed)) ImageDescriptor {
	uint16_t imageLeft;
	uint16_t imageTop;
	uint16_t imageWidth;
	uint16_t imageHeight;
	uint8_t sizeOfLocalColorTable:3;
	uint8_t reserved:2;
	uint8_t sortFlag:1;
	uint8_t interlaceFlag:1;
	uint8_t localColorTableFlag:1;
};

struct __attribute__ ((packed)) LogicalScreenDescriptor {
	uint16_t canvasWidth;
	uint16_t canvasHeight;
	uint8_t sizeOfGlobalColorTable:3;
	uint8_t sortFlag:1;
	uint8_t colorResolution:3;
	uint8_t globalColorTableFlag:1;
	uint8_t backgroundColorIndex;
	uint8_t pixelAspectRatio;
};

struct Color {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	bool transparent;
};

class ColorTable {
public:
	void setSize(uint32_t s);
	uint32_t size();

	void insert(Color c);
	void print();
	Color getColorFromColorTable(uint32_t i);
	void setTransparencyToIndex(uint16_t i);

private:
	uint32_t m_size;
	vector<Color> m_content; // colors in the color/code table
};

class CodeTable {
public:
	void initializeCodeTable(ColorTable*);
	void addRowToCodeTable(vector<uint32_t>);
	uint32_t getFirstIndexOfCode(uint32_t);
	vector<uint32_t> getCode(uint32_t);
	uint32_t getClearCode();
	uint32_t getEmptyCode();
	uint32_t getEndOfInformationCode();
	void printCodeTable();
	void setInitialCodeSize(uint8_t);
	void incrementCurrentCodeSize();
	uint8_t getCurrentCodeSize();
	void setCurrentCodeSize(uint8_t);
	void resetCurrentCodeSize();
	uint8_t getInitialCodeSize();

	void setCurrentCode(uint32_t);
	uint32_t getCurrentCode();
	void setPreviousCode(uint32_t);
	uint32_t getPreviousCode();

	void clearCodeTable();

private:
	vector<vector<uint32_t>> m_rows;
	uint32_t m_clearCode;
	uint32_t m_endOfInformationCode;
	uint32_t m_emptyCode;
	uint32_t m_currentCode;
	uint32_t m_previousCode;
	uint8_t initialCodeSize;
	uint8_t currentCodeSize;
};

class GIFImage {
private:
	FILE* m_file;
	GraphicsControlExtension m_graphicsControlExtension;
	ImageDescriptor m_imageDescriptor;
	ColorTable* m_globalColorTable;
	ColorTable m_localColorTable;
	vector<uint32_t> m_indexStream;
	CodeTable m_codeTable;

public:
	void setFile(FILE*);

	void loadImage(ColorTable* gct);

	void loadImageDescriptor();
	void printImageDescriptor();

	bool getLocalColorTableFlag();
	void setLocalColorTable(ColorTable* table);

	void createLocalColorTable();

	void handleGraphicsControlExtension();
	void handleEmptyGraphicsControlExtension();
	void printGraphicsControlExtension();

	void decodeImageData();

	size_t getSizeOfTheIndexStream();

	uint16_t getImageWidth();
	uint16_t getImageHeight();

	vector<uint32_t> getIndexStream();
	Color getColorFromColorTable(uint32_t);

	bool isImageInterlaced();
};




class GIFFormat  {
private:
	int m_status;
	vector<uint8_t> m_data;

	FILE* m_file;
	string m_headerFormat;
	string m_headerVersion;
	LogicalScreenDescriptor m_logicalScreenDescriptor;
	ColorTable m_globalColorTable;
	GIFImage m_image;

	uint8_t m_numberOfImages;

public:
	GIFFormat (FILE*);

	void loadHeader();

	void loadLogicalScreenDescriptor();
	void printLogicalScreenDescriptor();

	bool getGlobalColorTableFlag();
	uint8_t getSizeOfGlobalColorTable();
	void createGlobalColorTable();
	ColorTable getColorTable();

	void loadBlocks();
	GIFImage* getImageData();

	int64_t getFileSize();
};

////////////////////////////////////////////////////////////

struct BMPHeader {
	uint16_t IDField; // ID Field (0x4d42) //2
	uint32_t sizeOfTheBMPFile; // 4
	uint32_t reserved; // 4
	uint32_t pixelArrayOffset; // 54 // 4
};

struct DIPHeader {
	uint32_t sizeOfThdDIPHeader; // 40
	uint32_t bitmapWidth; // gif->width
	uint32_t bitmapHeight; // git->height
	uint16_t numberOfPlanes; // 1
	uint16_t bitsPerPixel; // 24
	uint32_t compression; // 0;
	uint32_t sizeOfRawBitmapData; // calculate;
	uint32_t printHorizontalResolution; //0
	uint32_t printVerticalResolution; // 0
	uint32_t numberOfColorsInPallete; // 0
	uint32_t importantColors; // 0
	uint32_t redChannelBitMask;
	uint32_t greenChannelBitMask;
	uint32_t blueChannelBitMask;
	uint32_t alphaChannelBitMask;
	uint32_t win;
	uint8_t empty[BMP_DIPHEADER_EMPTYSIZE];
};

class BMPFormat {
private:
	FILE* m_file;

	GIFImage* m_gifImage;
	BMPHeader m_bmpHeader;
	DIPHeader m_dipHeader;

	size_t m_paddingSize;
	uint8_t m_paddingValue;


public:
	BMPFormat(GIFFormat*, FILE*);

	void setPadding();

	void handleBMPHeader();
	void handleDIPHeader();
	void handlePixelArray();

uint64_t getBMPSize();
};

typedef struct {
	int64_t bmpSize;
	int64_t gifSize;
} tGIF2BMP;


int gif2bmp(tGIF2BMP *gif2bmp, FILE* inputFile, FILE* outputFile);

