// Title:       Resource File Manager
// Version:     1.0
// Date:        2011-03-16
// Description: Allows the user to pack multiple files into one file for easy storage. Often used by game developers to pack resources into a single file.
// Author:      Joel Verhagen
// Website:     http://www.joelverhagen.com
// Licensing:   Do whatever the heck you want with it. Golly, you don't even need to credit me if you don't want. Just don't say you originally wrote it. That would just make me sad.

#ifndef RESOURCEFILE_H
#define RESOURCEFILE_H

#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <iostream>

using namespace std;

#include "order32.h"

class ResourceFileEntry
{
private:
    string myName;
    string myFilePath;
    unsigned int myFileSize;
    unsigned int myOffset;
public:
    ResourceFileEntry(string name, string filePath, unsigned int fileSize);

    string name();
    string filePath();
    unsigned int fileSize();
    unsigned int offset();

    void setOffset(unsigned int offset);
};

class ResourceFileBase
{
public:
    static unsigned int magicWord;
    ResourceFileBase(string filePath);

    vector<string> getEntryNames();
protected:
    bool myReady;
    string myFilePath;
    map<string, ResourceFileEntry *> myEntries;
};

class ResourceFileWriter : public ResourceFileBase
{
public:
    ResourceFileWriter(string filePath);

    bool write();
    bool addEntry(string entryName, string entryFilePath);
private:
    int getHeaderSize();
    void writeNumber(ofstream &outputFile, unsigned int number);
};

class ResourceFileReader : public ResourceFileBase
{
public:
    ResourceFileReader(string filePath);

    bool read();
    unsigned int getEntryFileSize(const string &entryName);
    char * getFile(const string &entryName);
};

#endif
