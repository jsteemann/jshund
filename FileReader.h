
#ifndef JSHUND_FILEREADER_H
#define JSHUND_FILEREADER_H 1

#include <string.h>
#include <string>
#include <iostream>
#include <fstream>

namespace jshund {
  using namespace std;

  class FileReader {
    public:
      FileReader () {
      }

      ~FileReader () {
      }

      char* read (const string& fileName) {
        int fileSize;

        ifstream f;
        f.open(fileName.c_str(), ifstream::in | ifstream::binary);
        if (! f.is_open()) {
          return 0;
        }

        // get filesize
        f.seekg(0, ios::end);
        fileSize = f.tellg();
        f.seekg(0, ios::beg);

        // init buffer
        char* buffer = new char[fileSize + 5];
        memset(buffer + fileSize, 0, 5);

        // read contents
        f.read(buffer, fileSize);
        f.close();

        return buffer;
      }
  };
}

#endif

