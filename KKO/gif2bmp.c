/**
 * Author: Bc. Klara Necasova
 * Login: xnecas24
 * Date: 16. 2. 2018
 * Description: Library for GIF to BMP conversion
 */

#include "gif2bmp.h"

int gif2bmp(tGIF2BMP* gif2bmp, FILE* inputFile, FILE* outputFile)
{
	// create a GIF file object gifFile
	GIFFormat gifFile(inputFile);

	// get the file size for the statistics
	gif2bmp->gifSize = gifFile.getFileSize();

	// status variable (state of the conversion)
	int8_t status = CONVERSION_COMPLETED;

	try {
		// load file heaader, check the file type and version
		//status = gifFile.loadHeader();
		gifFile.loadHeader();

		// load and print logical screen descriptor
		// TODO: return value from load (did the function load enough data?, was the logical size positive, etc.)
		gifFile.loadLogicalScreenDescriptor();
		//gifFile.printLogicalScreenDescriptor();

		if (gifFile.getGlobalColorTableFlag()) {
			// create and print global color table
			gifFile.createGlobalColorTable();
		}

		// TODO: handle any number of images inside one file
		// TODO: handle errors (return value from loading, or try/catch)
		//load blocks
		gifFile.loadBlocks();

		// the GIF file was parsed successfully, generate the BMP file
		BMPFormat bmpFile(&gifFile, outputFile);

		bmpFile.handleBMPHeader();
		bmpFile.handleDIPHeader();
		bmpFile.handlePixelArray();

		gif2bmp->bmpSize = bmpFile.getBMPSize();

	}
	catch (const exception &ex)
	{
		cerr << ex.what() << endl;
		status = CONVERSION_ERROR;
	}
	catch (const bad_alloc &ex)
	{
		cerr << "bad_alloc caught: " << ex.what() << endl;
		// todo set status
	}

	return status;

	/*// TODO: handle any number of images inside one file
	// TODO: handle errors (return value from loading, or try/catch)
	//load blocks
	gifFile.loadBlocks();

	// the GIF file was parsed successfully, generate the BMP file
	BMPFormat bmpFile(&gifFile, outputFile);

	bmpFile.handleBMPHeader();
	bmpFile.handleDIPHeader();
	bmpFile.handlePixelArray();

	return CONVERSION_COMPLETED;*/
}

GIFFormat::GIFFormat (FILE* f)
{
	m_file = f;
	// TODO: unused?
	m_status = 0;
}

void GIFFormat::loadHeader()
{
	/// load format of the file
	char tmpFormat[GIF_FORMAT_SIZE+1];
	fread(&tmpFormat,  sizeof(uint8_t), GIF_FORMAT_SIZE, m_file);
	tmpFormat[GIF_FORMAT_SIZE] = '\0';

	m_headerFormat.assign(tmpFormat);

	/// load version of the file

	char tmpVersion[GIF_VERSION_SIZE + 1];

	fread(&tmpVersion, sizeof(uint8_t), GIF_VERSION_SIZE, m_file);
	tmpVersion[GIF_VERSION_SIZE] = '\0';

	m_headerVersion.assign(tmpVersion);

	if (m_headerFormat != "GIF")
 //       retVal =  GIF_ERR_INCORRECT_FORMAT;
		  throw runtime_error("Incorrect file format.");

	// TODO: currently, the implementation supports version 87a and blocks from 89a (animations)
	if (m_headerVersion != "87a" && m_headerVersion != "89a")
		throw runtime_error("Incorrect file version.");
}

void GIFFormat::loadLogicalScreenDescriptor()
{
	size_t itemCnt = fread(&m_logicalScreenDescriptor, sizeof(m_logicalScreenDescriptor), 1, m_file);

	// check if was read successfully all items
	if (itemCnt * sizeof(m_logicalScreenDescriptor) != GIF_LOGICAL_SCREEN_DESCRIPTOR_SIZE)
		throw runtime_error("Incorrect size of the logical screen descriptor");

	// check?
}

void GIFFormat::printLogicalScreenDescriptor()
{
	printf("%d\n", m_logicalScreenDescriptor.canvasWidth);
	printf("%d\n", m_logicalScreenDescriptor.canvasHeight);
	printf("%d\n", m_logicalScreenDescriptor.sizeOfGlobalColorTable);
	printf("%d\n", m_logicalScreenDescriptor.sortFlag);
	printf("%d\n", m_logicalScreenDescriptor.colorResolution);
	printf("%d\n", m_logicalScreenDescriptor.globalColorTableFlag);
	printf("%d\n", m_logicalScreenDescriptor.backgroundColorIndex);
	printf("%d\n", m_logicalScreenDescriptor.pixelAspectRatio);
}

void GIFFormat::createGlobalColorTable()
{
	// set size of the globalColorTable from the descriptor
	m_globalColorTable.setSize(getSizeOfGlobalColorTable());

	for (size_t i = 0; i < m_globalColorTable.size(); i++) {
		// is c transparent? cant tell in the global color table, so false for all colors
		Color c{0, 0, 0,false};

		size_t itemCnt = fread(&c, 3*sizeof(uint8_t), 1, m_file);

		if (itemCnt != 1)
			throw runtime_error("Global color table - incorrect color strorage");

		m_globalColorTable.insert(c);
	}
}

bool GIFFormat::getGlobalColorTableFlag()
{
	return m_logicalScreenDescriptor.globalColorTableFlag;
}


uint8_t GIFFormat::getSizeOfGlobalColorTable()
{
	return m_logicalScreenDescriptor.sizeOfGlobalColorTable;
}

void GIFFormat::loadBlocks()
{
	bool noGgraphicsExtensionBlock = true;
	size_t cnt;

	uint8_t tmpByte;

	// read next byte
	cnt = fread(&tmpByte, sizeof(uint8_t), 1, m_file);

	if (cnt != 1)
		throw runtime_error("File structure error.");

	// loop until the end of the image (or until the number of processed images is not equal to 1)
	while (tmpByte != 0x3b) {
		// we have 0 images loaded right now
		m_numberOfImages = 0;

		// extension block
		if (tmpByte == 0x21) {
			// select the extension block, that follows (next byte)
			uint8_t extensionBlockType;
			cnt = fread(&extensionBlockType, sizeof(uint8_t), 1, m_file);

			if (cnt != 1)
				throw runtime_error("Block structure error.");

			switch (extensionBlockType) {
				case 0xF9 :
					LOG cout << "Graphics Extension Block" << endl;
					m_image.setFile(m_file);
					noGgraphicsExtensionBlock = false;
					m_image.handleGraphicsControlExtension();
					break;
				case 0x01:
					LOG cout << "Plain Text Extension" << endl;
					// TODO: handle plain text extension (just skip appropriate number of bytes)

					// how many bytes can we skip?
					uint8_t skip;
					cnt = fread(&skip, sizeof(uint8_t), 1, m_file);

					if (cnt != 1)
						throw runtime_error("Block structure error.");

					// skip the skip number of bytes
					cnt = fread(nullptr, sizeof(uint8_t),skip,m_file);

					if (cnt != 1)
						throw runtime_error("Block structure error.");

					// size of the datablock
					uint8_t blockSize;
					cnt = fread(&blockSize, sizeof(uint8_t),skip,m_file);

					if (cnt != 1)
						throw runtime_error("Block structure error.");

					// loop until blockSize isnt 0
					while (blockSize != 0x00)
					{
						// read blockSize of bytes
						cnt = fread(nullptr, sizeof(uint8_t),blockSize,m_file);

						// TODO: check condition
						if (cnt != blockSize)
							throw runtime_error("Block structure error.");

						// read the byte following the block to determine, if its over or not
						cnt = fread(&blockSize, sizeof(uint8_t),skip,m_file);

						if (cnt != 1)
							throw runtime_error("Block structure error.");
					}

					break;
				case 0xFF:
					LOG cout << "Application Extension" << endl;
                    // read until the end of the extension
                    while (tmpByte != 0x00)
                        fread(&tmpByte, sizeof(uint8_t),1,m_file);
                    break;
				case 0xFE:
					LOG cout << "Comment extension" << endl;
					uint8_t numberOfBytes;
                    fread(&numberOfBytes, sizeof(uint8_t),1,m_file);
                    while (numberOfBytes != 0x00)
                    {
                        for (uint8_t i=0; i<numberOfBytes; i++)
                            fread(&skip, sizeof(uint8_t),1,m_file);

                        fread(&numberOfBytes, sizeof(uint8_t),1,m_file);
                    }
					break;
				default:
					break;
			}
		}
		// image
		else if (tmpByte == 0x2C) {
			m_image.setFile(m_file);

			if (noGgraphicsExtensionBlock)
				m_image.handleEmptyGraphicsControlExtension();

			m_image.loadImage(&m_globalColorTable);

			// TODO: discuss behavior with KN, currently this
			break;
		}

		// temporary read to prevent infinite looping
		fread(&tmpByte, sizeof(uint8_t), 1, m_file);
	}
}

void GIFImage::handleGraphicsControlExtension()
{
	// read the extension block into memory
	fread(&m_graphicsControlExtension, sizeof(m_graphicsControlExtension), 1, m_file);

	// print the controlExtension (debug)
	LOG printGraphicsControlExtension();

	// animations detected
	if (m_graphicsControlExtension.delayTime != 0) {
		LOG cout << "GIF file contains animations. Converting first image." << endl;
	}
}

void GIFImage::printGraphicsControlExtension()
{
	printf("byteSize: %d\n", m_graphicsControlExtension.byteSize);
	printf("transparentColorFlag: %d\n", m_graphicsControlExtension.transparentColorFlag);
	printf("userInputFlag: %d\n", m_graphicsControlExtension.userInputFlag);
	printf("disposalMethod: %d\n", m_graphicsControlExtension.disposalMethod);
	printf("delayTime: %d\n", m_graphicsControlExtension.delayTime);
	printf("transparentColorIndex: %d\n", m_graphicsControlExtension.transparentColorIndex);
	printf("blockTerminator: %d\n", m_graphicsControlExtension.blockTerminator);
}


// TODO: refactor (remove from GIF format, insert into GIF image)
void GIFImage::loadImage(ColorTable* gct)
{
	loadImageDescriptor();
	LOG printImageDescriptor();

	// local color table exists
	if (getLocalColorTableFlag()) {
		// TODO: implement local color table
		LOG cout << "Local color table detected" << endl;
		// load data and fill local color table
		createLocalColorTable();
	}
	else {
		// use global color table instead
		m_localColorTable = *gct;
	}


	// if there is a transparency present
	if (m_graphicsControlExtension.transparentColorFlag == 1)
	{
		m_localColorTable.setTransparencyToIndex(m_graphicsControlExtension.transparentColorIndex);

	}
	// print current color table
	m_localColorTable.print();

	// decode image data
	decodeImageData();
}

ColorTable GIFFormat::getColorTable()
{
	return m_globalColorTable;
}

GIFImage *GIFFormat::getImageData()
{
	return &m_image;
}

int64_t GIFFormat::getFileSize() {

	int64_t size = 0;

	fseek(m_file, 0, SEEK_END);
	size = ftell(m_file);

	fseek(m_file, 0, SEEK_SET);

	return size;
}

bool GIFImage::isImageInterlaced()
{
	if (m_imageDescriptor.interlaceFlag == 1)
		return true;
	else
		return false;
}

void ColorTable::setSize(uint32_t s)
{
	this->m_size = static_cast<uint32_t>(pow(2, s + 1));
}

uint32_t ColorTable::size()
{
	return m_size;
}

void ColorTable::insert(Color c)
{
	m_content.push_back(c);
}

void ColorTable::print()
{
	LOG cout << "size " << m_content.size() << endl;

	for (size_t i = 0; i < m_content.size(); i++) {
		LOG printf("%lld %x %x %x %d\n", i, m_content[i].red, m_content[i].green, m_content[i].blue, m_content[i].transparent);
	}

	LOG cout << "end" << endl;
}

Color ColorTable::getColorFromColorTable(uint32_t i)
{
	return m_content[i];
}

void ColorTable::setTransparencyToIndex(uint16_t i) {
	m_content[i].transparent = true;
}


void GIFImage::loadImageDescriptor()
{
	fread(&m_imageDescriptor, sizeof(m_imageDescriptor), 1, m_file);
}

void GIFImage::printImageDescriptor()
{
	LOG printf("imageLeft: %d\n", m_imageDescriptor.imageLeft);
	LOG printf("imageTop: %d\n", m_imageDescriptor.imageTop);
	LOG printf("imageWidth: %d\n", m_imageDescriptor.imageWidth);
	LOG printf("imageHeight: %d\n", m_imageDescriptor.imageHeight);
	LOG printf("sizeOfLocalColorTable: %d\n", m_imageDescriptor.sizeOfLocalColorTable);
	LOG printf("sortFlag: %d\n", m_imageDescriptor.sortFlag);
	LOG printf("interlaceFlag: %d\n", m_imageDescriptor.interlaceFlag);
	LOG printf("localColorTableFlag: %d\n", m_imageDescriptor.localColorTableFlag);
}

void GIFImage::setFile(FILE *f)
{
	m_file = f;
}

bool GIFImage::getLocalColorTableFlag()
{
	return m_imageDescriptor.localColorTableFlag;
}

void GIFImage::setLocalColorTable(ColorTable *table)
{
	m_localColorTable = *table;
}

void GIFImage::decodeImageData()
{
	// read the initial code size
	uint8_t initialCodeSize;
	fread(&initialCodeSize, sizeof(uint8_t), 1, m_file);

	// number of bits used (initially and currently)
	m_codeTable.setInitialCodeSize(initialCodeSize);

	LOG printf("%d \n",m_codeTable.getInitialCodeSize());

	uint8_t blockSize;

	// read the size of the following block (first block)
	fread(&blockSize,sizeof(uint8_t), 1, m_file);

	// bits that will be parsed
	string bitsAll;
	// until data is available
	while (blockSize != 0x00) {
		string bits;

		// for block saving
		vector<uint8_t> data;
		uint8_t tmp;

		for (uint8_t i = 0; i < blockSize; i++) {
			fread(&tmp, sizeof(uint8_t), 1, m_file);
			data.push_back(tmp);
		}

		// uint8_t to binary string conversion and creating of one string
		for (int i = blockSize - 1; i >= 0; i--) {
			bitset<8> set(data[i]);
			bits.append(set.to_string());
		}

		// create final string from created strings
		bitsAll.insert(bitsAll.begin(), bits.begin(), bits.end());
		// read the size of the following block
		fread(&blockSize, sizeof(uint8_t), 1, m_file);
	}

	// until all bits are parsed, keep loading additional codes
	while (!bitsAll.empty()) {
		string tmp;
		/// get following code from the bitstream
		for (int i = 0; i < m_codeTable.getCurrentCodeSize(); i++) {
			tmp.push_back(bitsAll.back());
			bitsAll.pop_back();

			// last bit was removed, break from the loop
			if (bitsAll.empty())
			{
				LOG cout << "Last bit removed, breaking" << endl;
				break;
			}

		}

		// due to the endiennes, reverse the loaded sequence of bits
		reverse(tmp.begin(), tmp.end());
		// string to unsigned long integer conversion
		m_codeTable.setCurrentCode(static_cast<uint16_t>(strtoul(tmp.c_str(), nullptr, 2)));

		LOG cout << "currentBits: " << tmp << endl;
		LOG printf("currentCode: %u\n", m_codeTable.getCurrentCode());
		LOG printf("previousCode: %d\n", m_codeTable.getPreviousCode());
		LOG printf("clearCode: %d\n", m_codeTable.getClearCode());
		LOG printf("emptyCode: %d\n", m_codeTable.getEmptyCode());
		LOG printf("numberOfBits: %d\n", m_codeTable.getCurrentCodeSize());
		LOG cout << m_localColorTable.size() << endl;
		/// handle the code
		// the current code value is equal to the clear code
		//if (m_codeTable.getCurrentCode() == m_localColorTable.getSizeOfColorTable()) {
		if (m_codeTable.getCurrentCode() == pow(2,initialCodeSize)) {
			LOG cout << "Clear code" << endl;
			// create code table
			m_codeTable.initializeCodeTable(&m_localColorTable);
			m_codeTable.resetCurrentCodeSize();
		}
		// the current code value is equal to the end of information code
		else if (m_codeTable.getCurrentCode() == m_codeTable.getEndOfInformationCode()) {
			LOG cout << "End of the image" << endl;
			//m_codeTable.printCodeTable();
			break;
		}
		else {
			// if the previous code from the byte stream was clear code
			if (m_codeTable.getPreviousCode() == m_codeTable.getClearCode()) {
				LOG cout << "Just pushing back the currentCode" << endl;
				LOG printf("previousCode: %d\n", m_codeTable.getPreviousCode());
				LOG printf("clearCode: %d\n", m_codeTable.getClearCode());
				LOG printf("codeValue: %d\n",m_codeTable.getCurrentCode());
				// place the color index into the index stream
				//m_indexStream.push_back(m_codeTable.getCurrentCode());
				vector<uint32_t> tmpInsert;
				tmpInsert = m_codeTable.getCode(m_codeTable.getCurrentCode());
				m_indexStream.insert(m_indexStream.end(), tmpInsert.begin(), tmpInsert.end());
				//m_indexStream.push_back(m_codeTable.getCode(m_codeTable.getCurrentCode()));
			}
			// if the currentCode is not in the code table
			else if (m_codeTable.getCurrentCode() >= m_codeTable.getEmptyCode()) {
				LOG cout << "Code is not in the codeTable" << endl;
				LOG printf("previousCode: %d\n", m_codeTable.getPreviousCode());
				// get the first index of the previous code
				uint32_t k = m_codeTable.getFirstIndexOfCode(m_codeTable.getPreviousCode());
				LOG printf("first index: %d\n", k);

				/// modify the codeStream
				vector<uint32_t> tmpPreviousCode = m_codeTable.getCode(m_codeTable.getPreviousCode());
				vector<uint32_t> tmpInsert;
				tmpInsert.insert(tmpInsert.end(), tmpPreviousCode.begin(), tmpPreviousCode.end());
				// add k to the end
				tmpInsert.push_back(k);
				// add previousCode
				m_indexStream.insert(m_indexStream.end(), tmpInsert.begin(), tmpInsert.end());

				LOG cout << "Value inserted into the indexStream: " ;

				for (size_t i = 0; i < tmpInsert.size(); i++) {
					LOG printf("%d ", tmpInsert[i]);
				}

				LOG cout << endl;

				/// add new row to the code table (variable tmpInsert)
				m_codeTable.addRowToCodeTable(tmpInsert);

				LOG cout << "Value inserted into the code table: " ;

				for (size_t i = 0; i < tmpInsert.size(); i++) {
					LOG printf("%d ", tmpInsert[i]);
				}

				LOG cout << endl;
			}
			// if the current code is in the code table
			else {
				LOG cout << "Code is in code table" << endl;
				// current code is inserted into the index stream
				vector<uint32_t> tmpCode = m_codeTable.getCode(m_codeTable.getCurrentCode());
				m_indexStream.insert(m_indexStream.end(), tmpCode.begin(), tmpCode.end());

				LOG cout << "Value inserted into the index stream: " ;

				for (size_t i = 0; i < tmpCode.size(); i++) {
					LOG printf("%d ", tmpCode[i]);
				}
				LOG cout << endl;

				// k is the first index of the current code
				uint32_t k = tmpCode[0];

				vector<uint32_t> tmpInsert;
				LOG printf("previousCode: %d\n", m_codeTable.getPreviousCode());
				vector<uint32_t> tmpPreviousCode = m_codeTable.getCode(m_codeTable.getPreviousCode());
				LOG printf("size of previous code: %lu\n", tmpPreviousCode.size());

				tmpInsert.insert(tmpInsert.end(), tmpPreviousCode.begin(), tmpPreviousCode.end());
				tmpInsert.push_back(k);

				/// add new row to the codeTable (variable tmpInsert)
				m_codeTable.addRowToCodeTable(tmpInsert);
			}
		}

		// increment number of bits
		if (m_codeTable.getEmptyCode() == pow(2,m_codeTable.getCurrentCodeSize()))
			m_codeTable.incrementCurrentCodeSize();

		// if the currentCodeSize exceeds the maximum codeSize, set it to the maximumSize (12)
		if (m_codeTable.getCurrentCodeSize() >= 13)
		{
			m_codeTable.setCurrentCodeSize(12);
		}

		m_codeTable.setPreviousCode(m_codeTable.getCurrentCode());
	}
}

size_t GIFImage::getSizeOfTheIndexStream()
{
	return m_indexStream.size();
}

uint16_t GIFImage::getImageHeight()
{
	return m_imageDescriptor.imageHeight;
}

uint16_t GIFImage::getImageWidth()
{
	return m_imageDescriptor.imageWidth;
}

vector<uint32_t> GIFImage::getIndexStream()
{
	return m_indexStream;
}

Color GIFImage::getColorFromColorTable(uint32_t i)
{
	return m_localColorTable.getColorFromColorTable(i);
}

void GIFImage::createLocalColorTable() {


	m_localColorTable.setSize(m_imageDescriptor.sizeOfLocalColorTable);

	for (size_t i = 0; i < m_localColorTable.size(); i++) {
		Color c{0, 0, 0, false};
		fread(&c, 3*sizeof(uint8_t), 1, m_file);

		//printf("%x %x %x\n", c.red, c.green,c.blue);
		m_localColorTable.insert(c);
	}
}

void GIFImage::handleEmptyGraphicsControlExtension()
{
	m_graphicsControlExtension = {0,0,0,0,0,0,0,0};

	LOG printGraphicsControlExtension();
}

void CodeTable::addRowToCodeTable(vector<uint32_t> r)
{
	LOG cout << "adding row to code table: ";
	for (uint32_t i : r)
		LOG cout << hex << i << " ";

	LOG cout << endl;

	m_rows.push_back(r);
	m_emptyCode = static_cast<uint32_t>(m_rows.size());
}

uint32_t CodeTable::getEndOfInformationCode()
{
	return m_endOfInformationCode;
}

uint32_t CodeTable::getClearCode()
{
	return m_clearCode;
}

void CodeTable::initializeCodeTable(ColorTable *table)
{
	// clear the code table if it contains something
	clearCodeTable();

	vector<uint32_t> tmp;
	for (size_t i = 0; i < table->size(); i++) {
		tmp.push_back(static_cast<unsigned char>(i));
		addRowToCodeTable(tmp);
		tmp.clear();
	}

	// fix for the smaller color table ( number of colors smaller than 4)
	if (table->size() < 3)
	{
		size_t toFill = 4 - table->size(); // how many do we need to fill

		for (size_t i = 0; i < toFill; i++)
		{
			tmp.push_back(static_cast<unsigned char>(m_rows.size()));
			addRowToCodeTable(tmp);
			tmp.clear();
		}
	}

	m_clearCode = static_cast<uint32_t>(m_rows.size());
	// clear index
	tmp.push_back(m_clearCode);
	addRowToCodeTable(tmp);
	tmp.clear();

	m_endOfInformationCode = static_cast<uint32_t>(m_rows.size());
	// end of data index
	tmp.push_back(m_endOfInformationCode);
	addRowToCodeTable(tmp);
	tmp.clear();

	m_emptyCode = static_cast<uint32_t>(m_rows.size());

	// set the previous code to clear code
	m_previousCode = m_clearCode;
}

uint32_t CodeTable::getEmptyCode()
{
	return m_emptyCode;
}

vector<uint32_t> CodeTable::getCode(uint32_t c)
{
	return m_rows[c];
}

uint32_t CodeTable::getFirstIndexOfCode(uint32_t pc)
{
	return m_rows[pc][0];
}

void CodeTable::printCodeTable()
{
	for (size_t i = 0; i < m_rows.size(); i++) {
		cout << "code: " << hex << i << ": ";
		for (uint32_t j : m_rows[i])
			cout << hex << j << ": ";

		cout << endl;
	}
}

void CodeTable::setInitialCodeSize(uint8_t cs)
{
	initialCodeSize = static_cast<uint8_t>(cs + 1);

	currentCodeSize = initialCodeSize;
}

uint8_t CodeTable::getInitialCodeSize()
{
	return initialCodeSize;
}

uint8_t CodeTable::getCurrentCodeSize()
{
	return currentCodeSize;
}

void CodeTable::incrementCurrentCodeSize()
{
	currentCodeSize++;
}

void CodeTable::resetCurrentCodeSize()
{
	currentCodeSize = initialCodeSize;
}

void CodeTable::setCurrentCode(uint32_t cc)
{
	m_currentCode = cc;
}

uint32_t CodeTable::getCurrentCode()
{
	return m_currentCode;
}

void CodeTable::setPreviousCode(uint32_t pc)
{
	m_previousCode = pc;
}

uint32_t CodeTable::getPreviousCode()
{
	return m_previousCode;
}

void CodeTable::clearCodeTable()
{
	LOG cout << "clearing code table" << endl;

	m_rows.clear();

	LOG cout << "size of the code table" << m_rows.size() << endl;
}

void CodeTable::setCurrentCodeSize(uint8_t size)
{
	currentCodeSize = size;
}

BMPFormat::BMPFormat(GIFFormat* format, FILE* file)
{
	// get parsed data and insert it to BMP file
	m_gifImage = format->getImageData();

	m_file = file;

	// set the padding for the BMP image being created
	setPadding();
}

void BMPFormat::handleBMPHeader()
{
	m_bmpHeader.IDField = 0x4d42;
	// offset = bmpheadersize + dipheadersize
	m_bmpHeader.pixelArrayOffset = 14 + sizeof(m_dipHeader);

	m_bmpHeader.sizeOfTheBMPFile = static_cast<uint32_t>(m_bmpHeader.pixelArrayOffset + m_gifImage->getSizeOfTheIndexStream() * 4);
	m_bmpHeader.reserved = 0000;

	fwrite(&m_bmpHeader.IDField, sizeof(uint16_t), 1, m_file);
	fwrite(&m_bmpHeader.sizeOfTheBMPFile, sizeof(uint32_t), 1, m_file);
	fwrite(&m_bmpHeader.reserved, sizeof(uint32_t), 1, m_file);
	fwrite(&m_bmpHeader.pixelArrayOffset, sizeof(uint32_t), 1, m_file);
}

void BMPFormat::setPadding()
{
	// four bytes per pixel, no padding
	m_paddingSize = 0;
	m_paddingValue = 0;
}

void BMPFormat::handleDIPHeader()
{
	m_dipHeader.sizeOfThdDIPHeader = sizeof(m_dipHeader);
	m_dipHeader.bitmapWidth = m_gifImage->getImageWidth();
	m_dipHeader.bitmapHeight = m_gifImage->getImageHeight();
	m_dipHeader.numberOfPlanes = 1;
	m_dipHeader.bitsPerPixel = 32;
	m_dipHeader.compression = 3; // BI_BITFIELDS
	m_dipHeader.sizeOfRawBitmapData = static_cast<uint32_t>(m_gifImage->getSizeOfTheIndexStream() * 4);
	m_dipHeader.printHorizontalResolution = 0;
	m_dipHeader.printVerticalResolution = 0;
	m_dipHeader.numberOfColorsInPallete = 0;
	m_dipHeader.importantColors = 0;

	// transparency
	m_dipHeader.redChannelBitMask = 0x00ff0000;
	m_dipHeader.greenChannelBitMask = 0x0000ff00;
	m_dipHeader.blueChannelBitMask = 0x000000ff;
	m_dipHeader.alphaChannelBitMask = 0xff000000;
	m_dipHeader.win = 0x206E6957;

	uint32_t tmp = 0;
	// clear array
	m_dipHeader.empty[BMP_DIPHEADER_EMPTYSIZE] = {0};

	fwrite(&m_dipHeader, sizeof(m_dipHeader), 1, m_file);
}

void BMPFormat::handlePixelArray()
{
	vector<uint32_t> indexStream =  m_gifImage->getIndexStream();

	int counter = 0;

	vector<vector <Color>> lines;

	vector<Color> in;

	// loop until the index stream is not empty
	while (!indexStream.empty()) {
		vector<Color> tmp;
		// load width of image of colors into the temporary vector
		for (size_t i = 0; i < m_dipHeader.bitmapWidth; i++) {
			tmp.push_back(m_gifImage->getColorFromColorTable(indexStream.back()));
			in.push_back(m_gifImage->getColorFromColorTable(indexStream.back()));
			indexStream.pop_back();
		}

		// reverse to handle in the correct order
		reverse(tmp.begin(),tmp.end());

		lines.push_back(tmp);

		tmp.clear();
		counter++;
	}

	// deinterlace lines

	reverse(lines.begin(),lines.end());

	vector<vector <Color>> dLines;


	// first indexes of groups
	vector<uint8_t> groupIndexes = {0,4,2,1};


	// how many times do we repeat the writing? just once, if the image is not interlaced
	uint8_t repeat = 1;

	uint32_t index;

	// we convert lines do deinterlaced order
	for (uint8_t r = 0; r < repeat; r++)
	{
		index = groupIndexes[r]; // current index to be read from the group

		for (size_t i = 0; i < lines.size(); i++)
		{
			if (index == i)
			{
				dLines.push_back(lines[index]);

				if (repeat == 1) // if we dont have interlacing
					index++; // increment the current index in the first group by one (next line)
				else // if we have interlacing
				{
					// we increment the the required line index
					if (r == 0)
						index += 8;
					else if (r == 1)
						index += 8;
					else if (r == 2)
						index += 4;
					else if (r == 3)
						index += 2;
				}
			}
		}
	}


	reverse(lines.begin(),lines.end());
	if (m_gifImage->isImageInterlaced())
	{
		reverse(in.begin(), in.end());
		size_t j = 0;

		uint32_t width = m_gifImage->getImageWidth();
		uint32_t size = m_gifImage->getImageHeight() * width;
		size_t newIndex, oldIndex;
		size_t start = 0;

		vector<Color> out(size, {0, 0, 0, false});

		start = 0;
		for (; start < size; start += 8 * width, j += width) {
			newIndex = start;
			oldIndex = j;

			for (uint32_t m = 0; m < width; m++, newIndex++, oldIndex++)
				out.at(newIndex) = in.at(oldIndex);
		}

		start = 4 * width;
		for (; start < size; start += 8 * width, j += width) {
			newIndex = start;
			oldIndex = j;

			for (uint32_t m = 0; m < width; m++, newIndex++, oldIndex++)
				out.at(newIndex) = in.at(oldIndex);
		}

		start = width*2;
		for (; start < size; start += 4 * width, j += width) {
			newIndex = start;
			oldIndex = j;

			for (uint32_t m = 0; m < width; m++, newIndex++, oldIndex++)
				out.at(newIndex) = in.at(oldIndex);
		}

		start = width*1;
		for (; start < size; start += 2 * width, j += width) {
			newIndex = start;
			oldIndex = j;

			for (uint32_t m = 0; m < width; m++, newIndex++, oldIndex++)
				out.at(newIndex) = in.at(oldIndex);
		}

		reverse(in.begin(), in.end());

		dLines.clear();
		for (size_t i = 0; i < out.size(); i+= width) {
			vector<Color> tmp;
			for (size_t j = i; j <i+width; j++)
				tmp.push_back(out.at(j));

			dLines.push_back(tmp);
		}
	}

	reverse(dLines.begin(),dLines.end());

	/// we have deiterlaced image, we can write it to file
	for (size_t i = 0; i < dLines.size(); i++)
	{
		for (size_t j = 0; j < dLines[i].size(); j++)
		{
			fwrite(&dLines[i][j].blue, sizeof(uint8_t), 1, m_file);
			fwrite(&dLines[i][j].green, sizeof(uint8_t), 1, m_file);
			fwrite(&dLines[i][j].red, sizeof(uint8_t), 1, m_file);

			uint8_t trnTmp = 0xff;
			if (dLines[i][j].transparent)
				trnTmp = 0;

			fwrite(&trnTmp, sizeof(trnTmp), 1, m_file);
		}
	}
}

uint64_t BMPFormat::getBMPSize() {
	return m_bmpHeader.sizeOfTheBMPFile;
}
