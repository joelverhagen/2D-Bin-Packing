#include "ResourceFile.h"

void swapEndianness(char * bytes)
{
    char swap;
    swap = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = swap;

    swap = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = swap;
}

void convertToLittleEndian(char * bytes)
{
    // if the host is big-endian, swap the bits around
    if(O32_HOST_ORDER == O32_BIG_ENDIAN)
    {
        swapEndianness(bytes);
    }
}

void convertLittleEndianToNative(char * bytes)
{
    // has the same exact function as convertToLittleEndian, but is replicated for clarity
    if(O32_HOST_ORDER == O32_BIG_ENDIAN)
    {
        swapEndianness(bytes);
    }
}

void arrayCopy(char * source, char * destination, int byteCount, int sourceOffset = 0, int destinationOffset = 0)
{
    for(int i = 0; i < byteCount; i++)
    {
        destination[sourceOffset + i] = source[destinationOffset + i];
    }
}

union intComponents
{
    // allows direct access to the bytes of an int
    unsigned int number;
    char bytes[4];
};

bool fileExists(const string &fileName)
{
    ifstream file(fileName.c_str(), ifstream::in);
    bool exists = file.is_open();
    file.close();

    return exists;
}

int getFileSize(const string &fileName)
{
    ifstream file(fileName.c_str(), ifstream::in | ifstream::binary);

    if(!file.is_open())
    {
        return -1;
    }

    file.seekg(0, ios::end);
    int fileSize = (int)file.tellg();
    file.close();

    return fileSize;
}

ResourceFileEntry::ResourceFileEntry(string name, string filePath, unsigned int fileSize)
{
    myName = name;
    myFilePath = filePath;
    myFileSize = fileSize;
    myOffset = -1;
}

string ResourceFileEntry::name()
{
    return myName;
}

string ResourceFileEntry::filePath()
{
    return myFilePath;
}

unsigned int ResourceFileEntry::fileSize()
{
    return myFileSize;
}

unsigned int ResourceFileEntry::offset()
{
    return myOffset;
}

void ResourceFileEntry::setOffset(unsigned int offset)
{
    myOffset = offset;
}

// magic word is 10E1A44A = JOELANNA :)
unsigned int ResourceFileBase::magicWord = 1252319504u;

ResourceFileBase::ResourceFileBase(string filePath)
{
    myFilePath = filePath;
    myReady = false;
}

vector<string> ResourceFileBase::getEntryNames()
{
    vector<string> entryNamesList;
    for(map<string, ResourceFileEntry *>::iterator i = myEntries.begin(); i != myEntries.end(); i++)
    {
        entryNamesList.push_back((*i).first);

    }

    return entryNamesList;
}

ResourceFileWriter::ResourceFileWriter(string filePath) : ResourceFileBase(filePath)
{

}

bool ResourceFileWriter::write()
{
    ofstream outputFile(myFilePath.c_str(), ios::out | ios::binary);
    if(!outputFile.is_open()) // make sure the file is open
    {
        return false;
    }

    int headerSize = getHeaderSize(); // calculate how many bytes the header will be

    // write the header information
    writeNumber(outputFile, magicWord);
    writeNumber(outputFile, headerSize);
    writeNumber(outputFile, myEntries.size());

    // write the file table
    int currentOffset = headerSize;
    for(map<string, ResourceFileEntry *>::iterator i = myEntries.begin(); i != myEntries.end(); i++)
    {
        ResourceFileEntry * entry = (*i).second;
        entry->setOffset(currentOffset);

        outputFile << entry->name() << '\0';
        writeNumber(outputFile, entry->fileSize());
        writeNumber(outputFile, currentOffset);

        currentOffset += entry->fileSize();
    }

    // input all source files
    ifstream inputFile;
    for(map<string, ResourceFileEntry *>::iterator i = myEntries.begin(); i != myEntries.end(); i++)
    {
        ResourceFileEntry * entry = (*i).second;

        inputFile.open(entry->filePath().c_str(), ios::in | ios::binary);

        outputFile << inputFile.rdbuf();

        inputFile.close();
    }

    outputFile.close();

    return true;
}

bool ResourceFileWriter::addEntry(string entryName, string entryFilePath)
{
    if(!fileExists(entryFilePath)) // make sure the input file exists
    {
        return false;
    }

    if(myEntries.find(entryName) != myEntries.end()) // make sure the name is unique
    {
        return false;
    }

    // create the entry
    ResourceFileEntry * entry = new ResourceFileEntry(entryName, entryFilePath, getFileSize(entryFilePath));

    // add the entry to the map
    myEntries[entryName] = entry;

    return true;
}

int ResourceFileWriter::getHeaderSize()
{
    // the magic word, header size, and the number of files
    int headerSize = 3 * sizeof(int);

    for(map<string, ResourceFileEntry *>::iterator i = myEntries.begin(); i != myEntries.end(); i++)
    {
        ResourceFileEntry * entry = (*i).second;
        // the number of characters in the name plus a null character
        headerSize += entry->name().size() + 1;
        // the file size and the offset
        headerSize += 2 * sizeof(int);
    }

    return headerSize;
}

void ResourceFileWriter::writeNumber(ofstream &outputFile, unsigned int number)
{
    // convert the number to little-endian
    intComponents numberComponents;
    numberComponents.number = number;
    convertToLittleEndian(&numberComponents.bytes[0]);

    // dump the number into the output stream
    outputFile << numberComponents.bytes[0];
	outputFile << numberComponents.bytes[1];
	outputFile << numberComponents.bytes[2];
	outputFile << numberComponents.bytes[3];
}

ResourceFileReader::ResourceFileReader(string filePath) : ResourceFileBase(filePath)
{

}

bool ResourceFileReader::read()
{
    ifstream inputFile(myFilePath.c_str(), ios::in | ios::binary);
    if(!inputFile.is_open()) // make sure the file is open
    {
        return false;
    }

    // read in the first four bytes, where the magic word should be
    intComponents currentMagicWord;
    currentMagicWord.number = 0;
    inputFile.read(currentMagicWord.bytes, 4);

    convertLittleEndianToNative(currentMagicWord.bytes);
    if(currentMagicWord.number != magicWord) // make sure the magic word matches
    {
        return false;
    }

    // read in the header size and convert from little-endian
    intComponents headerSize;
    headerSize.number = 0;
    inputFile.read(headerSize.bytes, 4);
    convertLittleEndianToNative(headerSize.bytes);

    // read in the file count and convert from little-endian
    intComponents fileCount;
    fileCount.number = 0;
    inputFile.read(fileCount.bytes, 4);
    convertLittleEndianToNative(fileCount.bytes);

    // read the entire file table block into memory
    int fileTableSize = headerSize.number - 3 * sizeof(int);
    char * headerData = new char[fileTableSize];
    inputFile.read(headerData, fileTableSize);

    // parse the file table
    int fileTableOffset = 0;
    unsigned int fileIndex = 0;
    while(true)
    {
        string name(headerData + fileTableOffset);
        fileTableOffset += name.size() + 1;

        // read in the entry's file size
        intComponents fileSize;
        fileSize.number = 0;
        arrayCopy(headerData + fileTableOffset, fileSize.bytes, sizeof(int));
        convertLittleEndianToNative(fileSize.bytes);

        // read in the entry's offset
        intComponents offset;
        offset.number = 0;
        arrayCopy(headerData + fileTableOffset + sizeof(int), offset.bytes, sizeof(int));
        convertLittleEndianToNative(offset.bytes);

        fileTableOffset += 2 * sizeof(int);

        // add the entry to the entry list
        ResourceFileEntry * entry = new ResourceFileEntry(name, "", fileSize.number);
        entry->setOffset(offset.number);

        myEntries[name] = entry;

        fileIndex++;
        if(fileIndex == fileCount.number || fileTableOffset >= fileTableSize) // if we've found enough files or we're at the end of the file table
        {
            break;
        }
    }

    delete headerData;

    return true;
}

unsigned int ResourceFileReader::getEntryFileSize(const string &entryName)
{
    return myEntries[entryName]->fileSize();
}

char * ResourceFileReader::getFile(const string &entryName)
{
    ResourceFileEntry * entry = myEntries[entryName];

    // seek to the beginning of the file we want
    ifstream inputFile(myFilePath.c_str(), ifstream::in | ifstream::binary);
    inputFile.seekg(entry->offset(), ios::beg);

    // read the entire file into memory
    char * fileData = new char[entry->fileSize()];

    inputFile.read(fileData, entry->fileSize());

    inputFile.close();

    return fileData;
}
