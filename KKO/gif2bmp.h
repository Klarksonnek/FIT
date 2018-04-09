/**
 * Author: Bc. Klara Necasova
 * Login: xnecas24
 * Date: 16. 2. 2018
 * Description: Library for GIF to BMP conversion
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

#define GIF_FORMAT_SIZE 3
#define GIF_VERSION_SIZE 3
#define GIF_LOGICAL_SCREEN_DESCRIPTOR_SIZE 7

#define BMP_DIPHEADER_EMPTYSIZE 48

#define CONVERSION_COMPLETED 0
#define CONVERSION_ERROR -1

/**
 * Data type with the conversion statistics.
 */
typedef struct
{
	/**
	 * size of the output (BMP file)
	 *
	 */
	int64_t bmpSize;
	/**
	 * size of the input (GIF file)
	 */
	int64_t gifSize;
} tGIF2BMP;

/**
 * structure, that defines the content of the Graphics control extension.
 * Size of the members is equal to the size read from the input file.
 *
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
 * structure, that defines the content of the Image descriptor. Size
 * of the members is equal to the size read from the input file.
 *
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
 * structure, that defines the content of the Logical screen descriptor.
 * Size of the members is equal to the size read from the input file.
 * http://giflib.sourceforge.net/whatsinagif/bits_and_bytes.html
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
 * structure, that defines the color in the color table of the GIF file.
 * http://giflib.sourceforge.net/whatsinagif/bits_and_bytes.html
 */
struct Color
{
	/**
	 * Red byte.
	 */
	uint8_t red;
	/**
	 * Green byte.
	 */
	uint8_t green;
	/**
	 * Blue byte.
	 */
	uint8_t blue;
	/**
	 * Color transparency.
	 */
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
 * Structure, that represents the header of the bitmap file.
 * https://en.wikipedia.org/wiki/BMP_file_format
 */
struct BMPHeader
{
	uint16_t IDField; // ID Field (0x4d42) //2
	uint32_t sizeOfTheBMPFile; // 4
	uint32_t reserved; // 4
	uint32_t pixelArrayOffset; // 54 // 4
};

/**
 * Structure, that represents the DIP header of the BMP file.
 * https://en.wikipedia.org/wiki/BMP_file_format
 */
struct DIPHeader
{
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

/**
 * Class for manipulating the color table of GIF files
 */
class ColorTable
{
public:
	/**
	 * Sets the size of the Color table
	 * @param s Color depth
	 */
	void setSize(uint32_t s);
	uint32_t size();

	/**
	 * Inserts color into the color table.
	 * @param c Color to be inserted into the color table.
	 */
	void insert(Color c);
	/**
	 * Auxillary method, that prints the entire color table to the stdout.
	 */
	void print();
	/**
	 * Gets the specified color from the color table.
	 * @param i Index of the color to be retrieved.
	 * @return the retrieved color
	 */
	Color colorFromColorTable(uint32_t i);
	/**
	 * Method sets the color, that is transparent.
	 * @param i Index of the transparent color.
	 */
	void setTransparencyToIndex(uint16_t i);

private:
	/**
	 * Size of the color table.
	 */
	uint32_t m_size;
	/**
	 * Content of the color table.
	 */
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
	 * Method initializes the code table to the initial state.
	 */
	void initializeCodeTable(ColorTable*);
	/**
	 * Method adds new entry to the code table.
	 */
	void addRowToCodeTable(std::vector<uint32_t>);
	/**
	 * Method that obtains the first index of the selected code from
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
	 * Method gets clear code.
	 * @return clear code.
	 */
	uint32_t clearCode();
	/**
	 * Method gets current empty code (row that can be used to save new code).
	 * @return empty code
	 */
	uint32_t emptyCode();
	/**
	 * Method gets end of information code.
	 * @return end of information code.
	 */
	uint32_t endOfInformationCode();

	/**
	 * Method sets initial code size for the decoding.
	 */
	void setInitialCodeSize(uint8_t);
	/**
	 * Method increment current code size
	 */
	void incrementCurrentCodeSize();
	/**
	 * Method gets current code size.
	 * @return Current code size.
	 */
	uint8_t getCurrentCodeSize();
	/**
	 * Method sets current code size.
	 */
	void setCurrentCodeSize(uint8_t);
	/**
	 * Method resets the code size to the initial size.
	 */
	void resetCurrentCodeSize();

	/**
	 * Method sets the current code to be decoded.
	 */
	void setCurrentCode(uint32_t);

	/**
	 * Method returns the current code.
	 * @return current code that is being decoded.
	 */
	uint32_t currentCode();

	/**
	 * Method sets previous code, that was deocded in the previous step.
	 */
	void setPreviousCode(uint32_t);

	/**
	 * Method gets previous code.
	 */
	uint32_t previousCode();

	/**
	 * Method clears code table to the initial state.
	 */
	void clearCodeTable();

private:
	/**
	 * Data of the code table (individual rows)
	 */
	std::vector<std::vector<uint32_t>> m_rows;
	/**
	 * Clear code
	 */
	uint32_t m_clearCode;
	/**
	 * End of information code
	 */
	uint32_t m_endOfInformationCode;
	/**
	 * Current empty code (the place to save new code)
	 */
	uint32_t m_emptyCode;
	/**
	 * Currently handeled code
	 */
	uint32_t m_currentCode;
	/**
	 * Previously handeled code
	 */
	uint32_t m_previousCode;
	/**
	 * Initial code size
	 */
	uint8_t m_initialCodeSize;
	/**
	 * Current code size
	 */
	uint8_t m_currentCodeSize;
};

/**
 * Class, that represents the GIF file.
 */
class GIFFormat
{
public:
	/**
	 * Constructor of the class. Loads the open file descriptor.
	 */
	GIFFormat(FILE*);

	/**
	 * Method, that loads the file header of the GIF file.
	 */
	void loadHeader();

	/**
	 * Method, that loads the logical screen descriptor of the GIF file.
	 */
	void loadLogicalScreenDescriptor();

	/**
	 * Method gets the global color table flag. If the global color
	 * table is present, we can load it.
	 * @return
	 */
	bool getGlobalColorTableFlag();
	/**
	 * Method gets the size of the global color table based on the
	 * loaded color depth.
	 * @return
	 */
	uint8_t getSizeOfGlobalColorTable();
	/**
	 * Method loads the color data from file.
	 */
	void createGlobalColorTable();

	/**
	 * This method handles the loading of the remaining blocks,
	 * that comprise the GIF file.
	 */
	void loadBlocks();

	/**
	 * Method calculates the size of the GIF file in bytes.
	 * @return size of the GIF file in bytes
	 */
	int64_t getFileSize();

	// image methods

	/**
	 * Method handles the image blocks (image desriptor, local color
	 * table and image data)
	 */
	void loadImage();

	/**
	 * Method loads the image descriptor.
	 */
	void loadImageDescriptor();

	/**
	 * Auxillary method, that prints the content of the image descriptor
	 * to the stdout.
	 */
	void printImageDescriptor();

	/**
	 * Method, that gets the value of the local color table flag. If the
	 * flag is set to true, the local color table is present and needs
	 * to be loaded and used for the image.
	 */
	bool getLocalColorTableFlag();

	/**
	 * Method creates the local color table.
	 */
	void createLocalColorTable();

	/**
	 * Method handles the graphics control extension (animations, transparency)
	 */
	void handleGraphicsControlExtension();
	/**
	 * Method handles the special case of missing graphics control extension.
	 */
	void handleEmptyGraphicsControlExtension();

	/**
	* Auxillary method, that prints the content of the graphics control
	 * extension to the stdout.
	*/
	void printGraphicsControlExtension();

	/**
	 * Method decodes the image data (LZW)
	 */
	void decodeImageData();

	/**
	 * Method loads the data from the input file and stores them as bits
	 * that will be decoded.
	 * @param blockSize initial block size
	 * @param dataBits string with bits, that will be filled (entire bit
	 * stream at the end)
	 */
	void loadDataBits(uint8_t blockSize, std::string* dataBits);

	/**
	 * Method gets the number of pixels that were decoded.
	 * @return  size of the index stream.
	 */
	size_t getSizeOfTheIndexStream();


	/**
	 * Method gets the image width from the image descriptor.
	 * @return image width
	 */
	uint16_t getImageWidth();
	/**
	 * Method gets the image height from the image descriptor.
	* @return image height
	*/
	uint16_t getImageHeight();

	/**
	 * Method gets the entire index stream.
	 * @return index stream.
	 */
	std::vector<uint32_t> getIndexStream();

	/**
	 * Method returns the selected color from the local color table.
	 * @return selected color from the local color table.
	 */
	Color getColorFromColorTable(uint32_t);

	/**
	 * Is the image interlaced?
	 * @return method returns true if the imege is interlaced, false otherwise
	 */
	bool isImageInterlaced();

private:
	/**
	 * Pointer to the opened GIF file.
	 */
	FILE* m_file;
	/**
	 * Saved format from the file header.
	 */
	std::string m_headerFormat;
	/**
	 * Saved version from the file header.
	 */
	std::string m_headerVersion;
	/**
	 * Logical screen descriptor.
	 */
	LogicalScreenDescriptor m_logicalScreenDescriptor;
	/**
	 * Global color table.
	 */
	ColorTable m_globalColorTable;

	/**
	 * Graphics control extension.
	 */
	GraphicsControlExtension m_graphicsControlExtension;
	/**
	 * Image descriptor.
	 */
	ImageDescriptor m_imageDescriptor;
	/**
	 * Local color table
	 */
	ColorTable m_localColorTable;
	/**
	 * Index stream. The ouput of the decoding of the image data.
	 */
	std::vector<uint32_t> m_indexStream;
	/**
	 * Code table (dictionary) for the modified LZW algorithm.
	 */
	CodeTable m_codeTable;
};

/**
 * Class, that represents the BMP file
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
	 * Method, that sets the required padding (mostly unused, due to
	 * the transparency).
	 */
	void setPadding();

	/**
	 * Method, that handles the BMP header. It fills the header with
	 * data and writes it in the file.
	 */
	void handleBMPHeader();
	/**
	 * Method, that handles the DIP header. It fills the header with
	 * data and writes it in the file.
	 */
	void handleDIPHeader();
	/**
	 * Method, that handles the BMP pixel array. It saves the pixels
	 * in the proper order, handles interlacing and
	 * transparency.
	 */
	void handlePixelArray();

	/**
	 * Method returns the size of the generated BMP file (the value
	 * is calculated in the header).
	 * @return size of the BMP file.
	 */
	uint64_t getBMPSize();

private:
	/**
	 * Pointer to the opened BMP file.
	 */
	FILE* m_file;

	/**
	 * Parsed GIF image data.
	 */
	GIFFormat* m_gifFormat;
	/**
	 * Header of the BMP file.
	 */
	BMPHeader m_bmpHeader;
	/**
	 * DIP header of the BMP file.
	 */
	DIPHeader m_dipHeader;

	/**
	 * Mostly unsued value for additional padding.
	 */
	size_t m_paddingSize;
	/**
	 * If there ever was any need for padding, its value.
	 */
	uint8_t m_paddingValue;
};

/**
 * The entry point of the conversion library.
 * @param gif2bmp Statistics of the conversion.
 * @param inputFile Pointer to the input file.
 * @param outputFile Pointer to the ouput file.
 * @return the result of the conversion (0 for success, -1 otherwise)
 */
int gif2bmp(tGIF2BMP *gif2bmp, FILE* inputFile, FILE* outputFile);
