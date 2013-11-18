
#ifndef JSHUND_FILEREADER_H
#define JSHUND_FILEREADER_H 1

#include <string.h>
#include <string>
#include <iostream>
#include <fstream>

namespace jshund {
  class FileReader {
    public:
      FileReader () {
      }

      ~FileReader () {
      }

    public:

      char* read (const std::string& fileName) {
        std::ifstream f;

        f.open(fileName.c_str(), std::ifstream::in | std::ifstream::binary);

        if (! f.is_open()) {
          return 0;
        }

        // get filesize
        f.seekg(0, std::ios::end);
        int fileSize = f.tellg();
        f.seekg(0, std::ios::beg);

        // allocate buffer plus some extra bytes
        char* buffer = new char[fileSize + 5];

        if (buffer != 0) {
          // initialise buffer with 0s
          memset(buffer + fileSize, 0, 5);

          // read contents
          f.read(buffer, fileSize);
          f.close();
        }

        return buffer;
      }
  };
}

#endif

