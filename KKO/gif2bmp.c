/**
 * Author: Bc. Klara Necasova
 * Login: xnecas24
 * Date: 16. 2. 2018
 * Description: Library for GIF to BMP conversion
 */

#include "gif2bmp.h"

using namespace std;

int gif2bmp(tGIF2BMP* gif2bmp, FILE* inputFile, FILE* outputFile)
{
	// creates a GIF file object gifFile
	GIFFormat gifFile(inputFile);

	// gets the file size for the statistics
	gif2bmp->gifSize = gifFile.getFileSize();

	// status variable (state of the conversion)
	int8_t status = CONVERSION_COMPLETED;

	try {
		// loads file header, checks the file type and version
		gifFile.loadHeader();

		// loads and prints logical screen descriptor
		gifFile.loadLogicalScreenDescriptor();

		if (gifFile.getGlobalColorTableFlag()) {
			// creates and prints global color table
			gifFile.createGlobalColorTable();
		}

		//loads blocks
		gifFile.loadBlocks();

		// the GIF file was parsed successfully, generates the BMP file
		BMPFormat bmpFile(&gifFile, outputFile);

		bmpFile.handleBMPHeader();
		bmpFile.handleDIPHeader();
		bmpFile.handlePixelArray();

		gif2bmp->bmpSize = bmpFile.getBMPSize();
	}
	catch (const bad_alloc &ex) {
		cerr << "bad_alloc caught: " << ex.what() << endl;
		status = CONVERSION_ERROR;
	}
	catch (const exception &ex) {
		cerr << ex.what() << endl;
		status = CONVERSION_ERROR;
	}

	return status;
}

GIFFormat::GIFFormat (FILE* file)
{
	m_file = file;
}

void GIFFormat::loadHeader()
{
	// loads format of the file
	char tmpFormat[GIF_FORMAT_SIZE + 1];
	fread(&tmpFormat,  sizeof(uint8_t), GIF_FORMAT_SIZE, m_file);
	tmpFormat[GIF_FORMAT_SIZE] = '\0';

	m_headerFormat.assign(tmpFormat);

	// loads version of the file
	char tmpVersion[GIF_VERSION_SIZE + 1];
	fread(&tmpVersion, sizeof(uint8_t), GIF_VERSION_SIZE, m_file);
	tmpVersion[GIF_VERSION_SIZE] = '\0';

	m_headerVersion.assign(tmpVersion);

	if (m_headerFormat != "GIF")
		  throw runtime_error("incorrect file format");

	if (m_headerVersion != "87a" && m_headerVersion != "89a")
		throw runtime_error("incorrect file version");
}

void GIFFormat::loadLogicalScreenDescriptor()
{
	size_t itemCnt = fread(&m_logicalScreenDescriptor, sizeof(m_logicalScreenDescriptor), 1, m_file);

	// checks if all items were read successfully
	if (itemCnt * sizeof(m_logicalScreenDescriptor) != GIF_LOGICAL_SCREEN_DESCRIPTOR_SIZE)
		throw runtime_error("incorrect size of the logical screen descriptor");
}

void GIFFormat::createGlobalColorTable()
{
	// sets size of the global color table from the descriptor
	m_globalColorTable.setSize(getSizeOfGlobalColorTable());

	for (size_t i = 0; i < m_globalColorTable.size(); i++) {
		// no information about transparency of colors, set to false for all colors
		Color c{0, 0, 0, false};

		size_t itemCnt = fread(&c, 3 * sizeof(uint8_t), 1, m_file);

		if (itemCnt != 1)
			throw runtime_error("global color table - incorrect color storage");

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
	bool noGraphicsExtensionBlock = true;
	size_t count;

	uint8_t tmpByte;

	// reads next byte
	count = fread(&tmpByte, sizeof(uint8_t), 1, m_file);

	if (count != 1)
		throw runtime_error("file structure error");

	// loops until the end of the image is loaded (or until the number of processed images is not equal to 1)
	while (tmpByte != 0x3b) {
		// extension block
		if (tmpByte == 0x21) {
			// selects the extension block, that follows (next byte)
			uint8_t extensionBlockType;
			count = fread(&extensionBlockType, sizeof(uint8_t), 1, m_file);

			if (count != 1)
				throw runtime_error("block structure error");

			switch (extensionBlockType) {
				case 0xF9 :
					LOG cout << "Graphics Extension Block" << endl;
					noGraphicsExtensionBlock = false;
					handleGraphicsControlExtension();
					break;

				case 0x01:
					LOG cout << "Plain Text Extension" << endl;

					// number of bytes that can be skipped
					uint8_t skip;
					count = fread(&skip, sizeof(uint8_t), 1, m_file);

					if (count != 1)
						throw runtime_error("block structure error");

					// skip given number of bytes
					count = fread(nullptr, sizeof(uint8_t), skip, m_file);

					if (count != 1)
						throw runtime_error("block structure error");

					// size of the data block
					uint8_t blockSize;
					count = fread(&blockSize, sizeof(uint8_t), skip, m_file);

					if (count != 1)
						throw runtime_error("block structure error");

					// loops until blockSize is 0
					while (blockSize != 0x00) {
						// reads blockSize of bytes
						count = fread(nullptr, sizeof(uint8_t), blockSize, m_file);

						if (count != blockSize)
							throw runtime_error("block structure error");

						// reads the next byte to determine if algorithm should continue
						count = fread(&blockSize, sizeof(uint8_t), skip, m_file);

						if (count != 1)
							throw runtime_error("block structure error");
					}
					break;

				case 0xFF:
					LOG cout << "Application Extension" << endl;
					// reads until the end of the extension is loaded
					while (tmpByte != 0x00)
						fread(&tmpByte, sizeof(uint8_t), 1, m_file);
					break;

				case 0xFE:
					LOG cout << "Comment Extension" << endl;
					uint8_t numberOfBytes;
					fread(&numberOfBytes, sizeof(uint8_t), 1, m_file);

					while (numberOfBytes != 0x00) {
						for (uint8_t i = 0; i < numberOfBytes; i++)
							fread(&skip, sizeof(uint8_t), 1, m_file);

						fread(&numberOfBytes, sizeof(uint8_t), 1, m_file);
					}
					break;

				default:
					break;
			}
		}
		// image
		else if (tmpByte == 0x2C) {

			if (noGraphicsExtensionBlock)
				handleEmptyGraphicsControlExtension();

			loadImage();
			break;
		}

		// prevents infinite looping
		fread(&tmpByte, sizeof(uint8_t), 1, m_file);
	}
}

void GIFFormat::handleGraphicsControlExtension()
{
	// reads the extension block into memory
	fread(&m_graphicsControlExtension, sizeof(m_graphicsControlExtension), 1, m_file);

	// prints the GraphicsControlExtension (debug)
	LOG printGraphicsControlExtension();

	// animations were detected
	if (m_graphicsControlExtension.delayTime != 0) {
		LOG cout << "GIF file contains animations, convert the first image" << endl;
	}
}

void GIFFormat::printGraphicsControlExtension()
{
	printf("byteSize: %d\n", m_graphicsControlExtension.byteSize);
	printf("transparentColorFlag: %d\n", m_graphicsControlExtension.transparentColorFlag);
	printf("userInputFlag: %d\n", m_graphicsControlExtension.userInputFlag);
	printf("disposalMethod: %d\n", m_graphicsControlExtension.disposalMethod);
	printf("delayTime: %d\n", m_graphicsControlExtension.delayTime);
	printf("transparentColorIndex: %d\n", m_graphicsControlExtension.transparentColorIndex);
	printf("blockTerminator: %d\n", m_graphicsControlExtension.blockTerminator);
}

void GIFFormat::loadImage()
{
	loadImageDescriptor();
	LOG printImageDescriptor();

	// if local color table exists
	if (getLocalColorTableFlag())
	{
		LOG cout << "Local color table detected" << endl;

		// loads data and fills local color table
		createLocalColorTable();
	}
	else {
		// uses global color table instead local color table
		m_localColorTable = m_globalColorTable;
	}

	// if there is a transparency
	if (m_graphicsControlExtension.transparentColorFlag == 1)
		m_localColorTable.setTransparencyToIndex(m_graphicsControlExtension.transparentColorIndex);

	// prints current color table
	LOG m_localColorTable.print();

	// decodes image data
	decodeImageData();
}

int64_t GIFFormat::getFileSize()
{
	int64_t size = 0;

	fseek(m_file, 0, SEEK_END);
	size = ftell(m_file);

	fseek(m_file, 0, SEEK_SET);

	return size;
}

bool GIFFormat::isImageInterlaced()
{
	return 1 == m_imageDescriptor.interlaceFlag;
}

void ColorTable::setSize(uint32_t s)
{
	m_size = static_cast<uint32_t>(pow(2, s + 1));
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
	cout << "size " << m_content.size() << endl;

	for (const auto &item : m_content) {
		cout << dec << unsigned(item.red) << " ";
		cout << dec << unsigned(item.green) << " ";
		cout << dec << unsigned(item.red) << " ";
		cout << dec << item.transparent << endl;
	}
	cout << "end" << endl;
}

Color ColorTable::colorFromColorTable(uint32_t i)
{
	return m_content[i];
}

void ColorTable::setTransparencyToIndex(uint16_t i)
{
	m_content[i].transparent = true;
}

void GIFFormat::loadImageDescriptor()
{
	fread(&m_imageDescriptor, sizeof(m_imageDescriptor), 1, m_file);
}

void GIFFormat::printImageDescriptor()
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

bool GIFFormat::getLocalColorTableFlag()
{
	return m_imageDescriptor.localColorTableFlag;
}

void GIFFormat::decodeImageData()
{
	uint8_t blockSize;
	uint8_t initialCodeSize;
	string bitsAll;

	// reads the initial code size
	fread(&initialCodeSize, sizeof(uint8_t), 1, m_file);

	// used number of bits (initially and currently)
	m_codeTable.setInitialCodeSize(initialCodeSize);

	// reads the size of the following block (first block)
	fread(&blockSize, sizeof(uint8_t), 1, m_file);

	loadDataBits(blockSize,&bitsAll);

	// until all bits are parsed, keeps loading additional codes
	while (!bitsAll.empty())
	{
		string tmp;
		// gets following code from the bit stream
		for (int i = 0; i < m_codeTable.getCurrentCodeSize(); i++) {
			tmp.push_back(bitsAll.back());
			bitsAll.pop_back();

			// last bit was removed, break from the loop
			if (bitsAll.empty()) {
				LOG cout << "Last bit removed, breaking" << endl;
				break;
			}
		}

		// due to reading from MSB, reverses the loaded sequence of bits
		reverse(tmp.begin(), tmp.end());

		// string to unsigned long integer conversion
		m_codeTable.setCurrentCode(static_cast<uint16_t>(strtoul(tmp.c_str(), nullptr, 2)));

		LOG cout << "currentBits: " << tmp << endl;
		LOG cout << "currentCode: " << m_codeTable.currentCode() << endl;
		LOG cout << "previousCode: " << m_codeTable.previousCode() << endl;
		LOG cout << "clearCode: " << m_codeTable.clearCode() << endl;
		LOG cout << "emptyCode: " << m_codeTable.emptyCode() << endl;
		LOG cout << "numberOfBits: " << m_codeTable.getCurrentCodeSize() << endl;
		LOG cout << m_localColorTable.size() << endl;

		// the current code value is equal to the clear code
		if (m_codeTable.currentCode() == pow(2, initialCodeSize)) {
			LOG cout << "Clear code" << endl;

			// creates code table
			m_codeTable.initializeCodeTable(&m_localColorTable);
			m_codeTable.resetCurrentCodeSize();
		}
		// the current code value is equal to the end of information code
		else if (m_codeTable.currentCode() == m_codeTable.endOfInformationCode()) {
			LOG cout << "End of the image" << endl;
			break;
		}
		else {
			// if the previous code from the byte stream was clear code
			if (m_codeTable.previousCode() == m_codeTable.clearCode()) {
				LOG cout << "Just pushing back the currentCode" << endl;
				LOG cout << "previousCode: " << m_codeTable.previousCode() << endl;
				LOG cout << "clearCode: " << m_codeTable.clearCode() << endl;
				LOG cout << "codeValue: " << m_codeTable.currentCode() << endl;

				// places the color index into the index stream
				vector<uint32_t> tmpInsert;
				tmpInsert = m_codeTable.code(m_codeTable.currentCode());
				m_indexStream.insert(m_indexStream.end(), tmpInsert.begin(), tmpInsert.end());
			}
			// if the current code is not in the code table
			else if (m_codeTable.currentCode() >= m_codeTable.emptyCode()) {
				LOG cout << "Code is not in the codeTable" << endl;

				// gets the first index of the previous code
				uint32_t k = m_codeTable.firstIndexOfCode(m_codeTable.previousCode());

				// modifies the code stream
				vector<uint32_t> tmpPreviousCode = m_codeTable.code(m_codeTable.previousCode());
				vector<uint32_t> tmpInsert;
				tmpInsert.insert(tmpInsert.end(), tmpPreviousCode.begin(), tmpPreviousCode.end());

				// adds k to the end
				tmpInsert.push_back(k);

				// adds previous code
				m_indexStream.insert(m_indexStream.end(), tmpInsert.begin(), tmpInsert.end());

				LOG cout << "Value inserted into the index stream: ";
				for (const auto &item : tmpInsert)
					LOG cout << hex << item << " ";
				LOG cout << endl;

				// adds new row to the code table (variable tmpInsert)
				m_codeTable.addRowToCodeTable(tmpInsert);

				LOG cout << "Value inserted into the code table: " ;
				for (const auto &item : tmpInsert)
					LOG cout << hex << item << " ";
				LOG cout << endl;
			}
			else {
				LOG cout << "Code is in code table" << endl;

				// current code is inserted into the index stream
				vector<uint32_t> tmpCode = m_codeTable.code(m_codeTable.currentCode());
				m_indexStream.insert(m_indexStream.end(), tmpCode.begin(), tmpCode.end());

				LOG cout << "Value inserted into the index stream: " ;
				for (const auto &item : tmpCode)
					LOG cout << hex << item << " ";
				LOG cout << endl;

				// k is the first index of the current code
				uint32_t k = tmpCode[0];

				vector<uint32_t> tmpInsert;
				vector<uint32_t> tmpPreviousCode = m_codeTable.code(m_codeTable.previousCode());

				tmpInsert.insert(tmpInsert.end(), tmpPreviousCode.begin(), tmpPreviousCode.end());
				tmpInsert.push_back(k);

				// adds new row to the code table (variable tmpInsert)
				m_codeTable.addRowToCodeTable(tmpInsert);
			}
		}

		// increments number of bits
		if (m_codeTable.emptyCode() == pow(2, m_codeTable.getCurrentCodeSize()))
			m_codeTable.incrementCurrentCodeSize();

		// if the m_currentCodeSize exceeds the maximum codeSize, sets it to the maximumSize (12)
		if (m_codeTable.getCurrentCodeSize() >= 13)
			m_codeTable.setCurrentCodeSize(12);

		m_codeTable.setPreviousCode(m_codeTable.currentCode());
	}
}

size_t GIFFormat::getSizeOfTheIndexStream()
{
	return m_indexStream.size();
}

uint16_t GIFFormat::getImageHeight()
{
	return m_imageDescriptor.imageHeight;
}

uint16_t GIFFormat::getImageWidth()
{
	return m_imageDescriptor.imageWidth;
}

vector<uint32_t> GIFFormat::getIndexStream()
{
	return m_indexStream;
}

Color GIFFormat::getColorFromColorTable(uint32_t index)
{
	return m_localColorTable.colorFromColorTable(index);
}

void GIFFormat::createLocalColorTable()
{
	m_localColorTable.setSize(m_imageDescriptor.sizeOfLocalColorTable);

	for (size_t i = 0; i < m_localColorTable.size(); i++) {
		Color c;
		fread(&c, 3 * sizeof(uint8_t), 1, m_file);

		m_localColorTable.insert(c);
	}
}

void GIFFormat::handleEmptyGraphicsControlExtension()
{
	m_graphicsControlExtension = {0, 0, 0, 0, 0, 0, 0, 0};
	LOG printGraphicsControlExtension();
}

void GIFFormat::loadDataBits(uint8_t blockSize, string* dataBits)
{
	// until data is available
	while (blockSize != 0x00) {
		string bits;

		// for block saving
		vector<uint8_t> data;
		uint8_t value;

		for (uint8_t i = 0; i < blockSize; i++) {
			fread(&value, sizeof(uint8_t), 1, m_file);
			data.push_back(value);
		}

		// uint8_t to binary string conversion and creating of one string
		for (int i = blockSize - 1; i >= 0; i--) {
			bitset<8> set(data[i]);
			bits.append(set.to_string());
		}

		// creates final string from created strings
		dataBits->insert(dataBits->begin(), bits.begin(), bits.end());

		// reads the size of the following block
		fread(&blockSize, sizeof(uint8_t), 1, m_file);
	}
}

void CodeTable::addRowToCodeTable(vector<uint32_t> row)
{
	LOG cout << "Adding row to code table: ";
	for (uint32_t i : row)
		LOG cout << hex << unsigned(i) << " ";
	LOG cout << endl;

	m_rows.push_back(row);
	m_emptyCode = static_cast<uint32_t>(m_rows.size());
}

uint32_t CodeTable::endOfInformationCode()
{
	return m_endOfInformationCode;
}

uint32_t CodeTable::clearCode()
{
	return m_clearCode;
}

void CodeTable::initializeCodeTable(ColorTable *table)
{
	// clears the code table if it contains something
	clearCodeTable();

	vector<uint32_t> tmp;
	for (size_t i = 0; i < table->size(); i++) {
		tmp.push_back(static_cast<unsigned char>(i));
		addRowToCodeTable(tmp);
		tmp.clear();
	}

	// fixes for the smaller color table (number of colors is smaller than 4)
	if (table->size() < 3)
	{
		size_t toFill = 4 - table->size();

		for (size_t i = 0; i < toFill; i++) {
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

	tmp.push_back(m_endOfInformationCode);
	addRowToCodeTable(tmp);
	tmp.clear();

	m_emptyCode = static_cast<uint32_t>(m_rows.size());

	// sets the previous code to clear code
	m_previousCode = m_clearCode;
}

uint32_t CodeTable::emptyCode()
{
	return m_emptyCode;
}

vector<uint32_t> CodeTable::code(uint32_t c)
{
	return m_rows[c];
}

uint32_t CodeTable::firstIndexOfCode(uint32_t pc)
{
	return m_rows[pc][0];
}

void CodeTable::setInitialCodeSize(uint8_t cs)
{
	m_initialCodeSize = static_cast<uint8_t>(cs + 1);
	m_currentCodeSize = m_initialCodeSize;
}

uint8_t CodeTable::getCurrentCodeSize()
{
	return m_currentCodeSize;
}

void CodeTable::incrementCurrentCodeSize()
{
	m_currentCodeSize++;
}

void CodeTable::resetCurrentCodeSize()
{
	m_currentCodeSize = m_initialCodeSize;
}

void CodeTable::setCurrentCode(uint32_t cc)
{
	m_currentCode = cc;
}

uint32_t CodeTable::currentCode()
{
	return m_currentCode;
}

void CodeTable::setPreviousCode(uint32_t pc)
{
	m_previousCode = pc;
}

uint32_t CodeTable::previousCode()
{
	return m_previousCode;
}

void CodeTable::clearCodeTable()
{
	LOG cout << "Clearing code table" << endl;
	m_rows.clear();
	LOG cout << "Size of the code table " << m_rows.size() << endl;
}

void CodeTable::setCurrentCodeSize(uint8_t size)
{
	m_currentCodeSize = size;
}

BMPFormat::BMPFormat(GIFFormat* format, FILE* file)
{
	// gets parsed data and inserts it to BMP file
	m_gifFormat = format;

	m_file = file;

	// sets the padding for the BMP image being created
	setPadding();
}

void BMPFormat::handleBMPHeader()
{
	m_bmpHeader.IDField = 0x4d42;
	m_bmpHeader.pixelArrayOffset = 14 + sizeof(m_dipHeader);

	m_bmpHeader.sizeOfTheBMPFile = static_cast<uint32_t>(m_bmpHeader.pixelArrayOffset + m_gifFormat->getSizeOfTheIndexStream() * 4);
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
	m_dipHeader.sizeOfTheDIPHeader = sizeof(m_dipHeader);
	m_dipHeader.bitmapWidth = m_gifFormat->getImageWidth();
	m_dipHeader.bitmapHeight = m_gifFormat->getImageHeight();
	m_dipHeader.numberOfPlanes = 1;
	m_dipHeader.bitsPerPixel = 32;
	m_dipHeader.compression = 3;
	m_dipHeader.sizeOfRawBitmapData = static_cast<uint32_t>(m_gifFormat->getSizeOfTheIndexStream() * 4);
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

	// clears array
	m_dipHeader.empty[BMP_DIP_HEADER_EMPTYSIZE] = {0};

	fwrite(&m_dipHeader, sizeof(m_dipHeader), 1, m_file);
}

void BMPFormat::handlePixelArray()
{
	uint32_t width = m_gifFormat->getImageWidth();
	uint32_t size = m_gifFormat->getImageHeight() * width;

	vector<uint32_t> indexStream =  m_gifFormat->getIndexStream();
	vector<Color> in;

	// loops until the index stream is empty
	while (!indexStream.empty()) {
		for (size_t i = 0; i < m_dipHeader.bitmapWidth; i++) {
			in.push_back(m_gifFormat->getColorFromColorTable(indexStream.back()));
			indexStream.pop_back();
		}
	}

	reverse(in.begin(), in.end());

	if (m_gifFormat->isImageInterlaced()) {
		vector<Color> output(size, {0, 0, 0, false});
		size_t newPosition, oldPosition;
		size_t i = 0;
		size_t startPosition = 0;

		for (; startPosition < size; startPosition += 8 * width, i += width) {
			newPosition = startPosition;
			oldPosition = i;

			for (uint32_t j = 0; j < width; j++, newPosition++, oldPosition++)
				output.at(newPosition) = in.at(oldPosition);
		}

		startPosition = 4 * width;
		for (; startPosition < size; startPosition += 8 * width, i += width) {
			newPosition = startPosition;
			oldPosition = i;

			for (uint32_t m = 0; m < width; m++, newPosition++, oldPosition++)
				output.at(newPosition) = in.at(oldPosition);
		}

		startPosition = width * 2;
		for (; startPosition < size; startPosition += 4 * width, i += width) {
			newPosition = startPosition;
			oldPosition = i;

			for (uint32_t m = 0; m < width; m++, newPosition++, oldPosition++)
				output.at(newPosition) = in.at(oldPosition);
		}

		startPosition = width * 1;
		for (; startPosition < size; startPosition += 2 * width, i += width) {
			newPosition = startPosition;
			oldPosition = i;

			for (uint32_t m = 0; m < width; m++, newPosition++, oldPosition++)
				output.at(newPosition) = in.at(oldPosition);
		}

		in = output;
	}

	vector<vector <Color>> dLines;
	for (size_t i = 0; i < in.size(); i+= width) {
		vector<Color> tmp;

		for (size_t j = i; j <i+width; j++)
			tmp.push_back(in.at(j));

		dLines.push_back(tmp);
	}

	reverse(dLines.begin(),dLines.end());

	// interspersing removed, writes image to file
	for (const auto &dLine : dLines) {
		for (const auto &item : dLine) {
			fwrite(&item.blue, sizeof(uint8_t), 1, m_file);
			fwrite(&item.green, sizeof(uint8_t), 1, m_file);
			fwrite(&item.red, sizeof(uint8_t), 1, m_file);

			uint8_t trnTmp = 0xff;
			if (item.transparent)
				trnTmp = 0;

			fwrite(&trnTmp, sizeof(trnTmp), 1, m_file);
		}
	}
}

uint64_t BMPFormat::getBMPSize()
{
	return m_bmpHeader.sizeOfTheBMPFile;
}
