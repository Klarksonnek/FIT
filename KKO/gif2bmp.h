/**
 * Author: Bc. Klara Necasova
 * Login: xnecas24
 * Date: 16. 2. 2018
 * Description: Library for GIF to BMP conversion
 */

#pragma once

#include <algorithm>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

/**
 * Set to true to allow debug prints.
 */
#define DEBUG_ENABLE false
#define LOG if (DEBUG_ENABLE)

#define GIF_FORMAT_SIZE 3
#define GIF_VERSION_SIZE 3
#define GIF_LOGICAL_SCREEN_DESCRIPTOR_SIZE 7

#define BMP_DIP_HEADER_EMPTYSIZE 48

#define CONVERSION_COMPLETED 0
#define CONVERSION_ERROR -1

/**
 * Data type with the conversion statistics.
 */
typedef struct
{
	// size of the output (BMP file)
	int64_t bmpSize;
	// size of the input (GIF file)
	int64_t gifSize;
} tGIF2BMP;

/**
 * Content of the Graphics control extension.
 * Size of the members is equal to the size read from the input file.
 * @see http://giflib.sourceforge.net/whatsinagif/bits_and_bytes.html
 */
struct __attribute__ ((packed)) GraphicsControlExtension
{
	uint8_t byteSize;
	uint8_t transparentColorFlag:1;
	uint8_t userInputFlag:1;
	uint8_t disposalMethod:3;
	uint8_t reserved:3;
	uint16_t delayTime;
	uint8_t transparentColorIndex;
	uint8_t blockTerminator;
};

/**
 * Content of the Image descriptor.
 * Size of the members is equal to the size read from the input file.
 * @see http://giflib.sourceforge.net/whatsinagif/bits_and_bytes.html
 */
struct __attribute__ ((packed)) ImageDescriptor
{
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

/**
 * Content of the Logical screen descriptor.
 * Size of the members is equal to the size read from the input file.
 * @see http://giflib.sourceforge.net/whatsinagif/bits_and_bytes.html
 */
struct __attribute__ ((packed)) LogicalScreenDescriptor
{
	uint16_t canvasWidth;
	uint16_t canvasHeight;
	uint8_t sizeOfGlobalColorTable:3;
	uint8_t sortFlag:1;
	uint8_t colorResolution:3;
	uint8_t globalColorTableFlag:1;
	uint8_t backgroundColorIndex;
	uint8_t pixelAspectRatio;
};

/**
 * Color in the color table of the GIF file.
 * @see http://giflib.sourceforge.net/whatsinagif/bits_and_bytes.html
 */
struct Color
{
	// red byte
	uint8_t red;
	//green byte
	uint8_t green;
	// blue byte
	uint8_t blue;
	// color transparency
	bool transparent;

	Color():
		red(0),
		green(0),
		blue(0),
		transparent(false)
	{
	}

	Color(uint8_t red, uint8_t green, uint8_t blue, bool transparent):
		red(red),
		green(green),
		blue(blue),
		transparent(transparent)
	{
	}
};

/**
 * Header of the BMP file.
 * @see https://en.wikipedia.org/wiki/BMP_file_format
 */
struct BMPHeader
{
	uint16_t IDField; // ID Field (0x4d42)
	uint32_t sizeOfTheBMPFile;
	uint32_t reserved;
	uint32_t pixelArrayOffset;
};

/**
 * DIP header of the BMP file.
 * @see https://en.wikipedia.org/wiki/BMP_file_format
 */
struct DIPHeader
{
	uint32_t sizeOfTheDIPHeader;
	uint32_t bitmapWidth;
	uint32_t bitmapHeight;
	uint16_t numberOfPlanes;
	uint16_t bitsPerPixel;
	uint32_t compression;
	uint32_t sizeOfRawBitmapData;
	uint32_t printHorizontalResolution;
	uint32_t printVerticalResolution;
	uint32_t numberOfColorsInPallete;
	uint32_t importantColors;
	uint32_t redChannelBitMask;
	uint32_t greenChannelBitMask;
	uint32_t blueChannelBitMask;
	uint32_t alphaChannelBitMask;
	uint32_t win;
	uint8_t empty[BMP_DIP_HEADER_EMPTYSIZE];
};

/**
 * Class for manipulating the color table of GIF files.
 */
class ColorTable
{
public:
	/**
	 * Sets the size of the color table.
	 * @param s Color depth.
	 */
	void setSize(uint32_t s);
	/**
	 * Gets the size of the color table.
	 * @return size of color table.
	 */
	uint32_t size();
	/**
	 * Inserts color into the color table.
	 * @param c Color to be inserted into the color table.
	 */
	void insert(Color c);
	/**
	 * Prints the entire color table to the stdout (auxiliary method).
	 */
	void print();
	/**
	 * Gets the specified color from the color table.
	 * @param i Index of the color to be retrieved.
	 * @return the retrieved color.
	 */
	Color colorFromColorTable(uint32_t i);
	/**
	 * Sets the color, that is transparent.
	 * @param i Index of the transparent color.
	 */
	void setTransparencyToIndex(uint16_t i);

private:
	// size of the color table
	uint32_t m_size;
	// content of the color table
	std::vector<Color> m_content;
};

/**
 * Class for representing and manipulating the content of the
 * dictionary/code table.
 */
class CodeTable
{
public:
	/**
	 * Initializes the code table to the initial state.
	 */
	void initializeCodeTable(ColorTable*);
	/**
	 * Adds new entry to the code table.
	 */
	void addRowToCodeTable(std::vector<uint32_t>);
	/**
	 * Obtains the first index of the selected code from
	 * the code table.
	 * @return First index of the selected code.
	 */
	uint32_t firstIndexOfCode(uint32_t);
	/**
	 * Method gets entire row of the code table.
	 * @return selected row of the code table.
	 */
	std::vector<uint32_t> code(uint32_t);
	/**
	 * Gets clear code.
	 * @return clear code.
	 */
	uint32_t clearCode();
	/**
	 * Gets current empty code (row that can be used to save new code).
	 * @return empty code.
	 */
	uint32_t emptyCode();
	/**
	 * Gets end of information code.
	 * @return end of information code.
	 */
	uint32_t endOfInformationCode();

	/**
	 * Sets initial code size for the decoding.
	 */
	void setInitialCodeSize(uint8_t);
	/**
	 * Increments current code size.
	 */
	void incrementCurrentCodeSize();
	/**
	 * Gets current code size.
	 * @return current code size.
	 */
	uint8_t getCurrentCodeSize();
	/**
	 * Sets current code size.
	 */
	void setCurrentCodeSize(uint8_t);
	/**
	 * Resets the code size to the initial size.
	 */
	void resetCurrentCodeSize();
	/**
	 * Sets the current code to be decoded.
	 */
	void setCurrentCode(uint32_t);
	/**
	 * Returns the current code.
	 * @return current code that is being decoded.
	 */
	uint32_t currentCode();
	/**
	 * Sets previous code, that was decoded in the previous step.
	 */
	void setPreviousCode(uint32_t);
	/**
	 * Gets previous code.
	 * @return previous code.
	 */
	uint32_t previousCode();
	/**
	 * Clears code table to the initial state.
	 */
	void clearCodeTable();

private:
	// data of the code table (individual rows)
	std::vector<std::vector<uint32_t>> m_rows;
	// clear code
	uint32_t m_clearCode;
	// end of information code
	uint32_t m_endOfInformationCode;
	// current empty code (the place to save new code)
	uint32_t m_emptyCode;
	// currently handled code
	uint32_t m_currentCode;
	// previously handled code
	uint32_t m_previousCode;
	// initial code size
	uint8_t m_initialCodeSize;
	// current code size
	uint8_t m_currentCodeSize;
};

/**
 * Class representing the GIF file.
 */
class GIFFormat
{
public:
	/**
	 * Constructor of the class. Loads the open file descriptor.
	 */
	GIFFormat(FILE*);
	/**
	 * Loads the file header of the GIF file.
	 */
	void loadHeader();
	/**
	 * Loads the Logical screen descriptor of the GIF file.
	 */
	void loadLogicalScreenDescriptor();
	/**
	 * Gets the global color table flag. If the Global color
	 * table is present, it can be loaded.
	 * @return global color table flag.
	 */
	bool getGlobalColorTableFlag();
	/**
	 * Gets the size of the global color table based on the
	 * loaded color depth.
	 * @return size of global color table.
	 */
	uint8_t getSizeOfGlobalColorTable();
	/**
	 * Loads the color data from file.
	 */
	void createGlobalColorTable();
	/**
	 * Handles the loading of the remaining blocks,
	 * that comprise the GIF file.
	 */
	void loadBlocks();
	/**
	 * Calculates the size of the GIF file in bytes.
	 * @return size of the GIF file in bytes.
	 */
	int64_t getFileSize();

	/**
	 * Handles the image blocks (image descriptor, local color
	 * table and image data)
	 */
	void loadImage();
	/**
	 * Loads the image descriptor.
	 */
	void loadImageDescriptor();
	/**
	 * Prints the content of the image descriptor
	 * to the stdout (auxiliary method).
	 */
	void printImageDescriptor();
	/**
	 * Gets the local color table flag. If the
	 * flag is set to true, the local color table is present and needs
	 * to be loaded and used for the image.
	 * @return local color table flag.
	 */
	bool getLocalColorTableFlag();
	/**
	 * Creates the local color table.
	 */
	void createLocalColorTable();
	/**
	 * Handles the graphics control extension (animations, transparency).
	 */
	void handleGraphicsControlExtension();
	/**
	 * Handles the special case of missing graphics control extension.
	 */
	void handleEmptyGraphicsControlExtension();
	/**
	* Prints the content of the graphics control
	* extension to the stdout (auxiliary method).
	*/
	void printGraphicsControlExtension();
	/**
	 * Decodes the image data (LZW).
	 */
	void decodeImageData();
	/**
	 * Loads the data from the input file and stores them as bits
	 * that will be decoded.
	 * @param blockSize Initial block size.
	 * @param dataBits String with bits, that will be filled (entire bit
	 * stream at the end).
	 */
	void loadDataBits(uint8_t blockSize, std::string* dataBits);
	/**
	 * Gets the number of pixels that were decoded.
	 * @return  size of the index stream.
	 */
	size_t getSizeOfTheIndexStream();
	/**
	 * Gets the image width from the image descriptor.
	 * @return image width.
	 */
	uint16_t getImageWidth();
	/**
	 * Gets the image height from the image descriptor.
	* @return image height.
	*/
	uint16_t getImageHeight();
	/**
	 * Gets the entire index stream.
	 * @return index stream.
	 */
	std::vector<uint32_t> getIndexStream();
	/**
	 * Returns the selected color from the local color table.
	 * @return selected color from the local color table.
	 */
	Color getColorFromColorTable(uint32_t);
	/**
	 * Finds out if the image is interlaced.
	 * @return true if the image is interlaced, false otherwise.
	 */
	bool isImageInterlaced();

private:
	// pointer to the opened GIF file
	FILE* m_file;
	// saved format from the file header
	std::string m_headerFormat;
	// saved version from the file header
	std::string m_headerVersion;
	// logical screen descriptor
	LogicalScreenDescriptor m_logicalScreenDescriptor;
	// global color table
	ColorTable m_globalColorTable;
	// graphics control extension
	GraphicsControlExtension m_graphicsControlExtension;
	// image descriptor
	ImageDescriptor m_imageDescriptor;
	// local color table
	ColorTable m_localColorTable;
	// index stream, the ouput of the decoding of the image data
	std::vector<uint32_t> m_indexStream;
	// code table (dictionary) for the modified LZW algorithm
	CodeTable m_codeTable;
};

/**
 * Class representing the BMP file.
 */
class BMPFormat
{
public:
	/**
	 * The constructor, that uses the parsed GIF file and the pointer
	 * to the opened BMP file (for writing).
	 */
	BMPFormat(GIFFormat*, FILE*);
	/**
	 * Sets the required padding (mostly unused, due to
	 * the transparency).
	 */
	void setPadding();
	/**
	 * Handles the BMP header. It fills the header with
	 * data and writes it in the file.
	 */
	void handleBMPHeader();
	/**
	 * Handles the DIP header. It fills the header with
	 * data and writes it in the file.
	 */
	void handleDIPHeader();
	/**
	 * Handles the BMP pixel array. It saves the pixels
	 * in the proper order, handles interlacing and
	 * transparency.
	 */
	void handlePixelArray();
	/**
	 * Gets the size of the generated BMP file (the value
	 * is calculated in the header).
	 * @return size of the BMP file.
	 */
	uint64_t getBMPSize();

private:
	// pointer to the opened BMP file
	FILE* m_file;
	// parsed GIF image data
	GIFFormat* m_gifFormat;
	// header of the BMP file
	BMPHeader m_bmpHeader;
	// DIP header of the BMP file
	DIPHeader m_dipHeader;
	// mostly unused value for additional padding
	size_t m_paddingSize;
	// padding value (if needed)
	uint8_t m_paddingValue;
};

/**
 * The entry point of the conversion library.
 * @param gif2bmp Statistics of the conversion.
 * @param inputFile Pointer to the input file.
 * @param outputFile Pointer to the output file.
 * @return the result of the conversion (0 for success, -1 otherwise).
 */
int gif2bmp(tGIF2BMP *gif2bmp, FILE* inputFile, FILE* outputFile);
