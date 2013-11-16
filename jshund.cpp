
#include <stdlib.h>
#include <string>
#include <iostream>

#include "DirectoryIterator.h"
#include "FileReader.h"
#include "Tokenizer.h"
#include "Validator.h"

using namespace std;
using namespace jshund;

int main (int argc, char* argv[]) {
  if (argc < 2) {
    cerr << "usage: " << argv[0] << " <file>" << endl;
    return EXIT_FAILURE;
  }

  bool hasError = false;
  
  DirectoryIterator dir;
  vector<string> files;

  for (int i = 1; i < argc; ++i) {
    dir.addFiles(files, argv[i]);
  }

  FileReader f;
  for (size_t i = 0; i < files.size(); ++i) {
    const string filename = files[i];

    char* fileContent = f.read(filename);

    if (fileContent == 0) {
      cerr << "could not read input file '" << filename << "'" << endl;
      hasError = true;
    }
    else {
      Tokenizer t;
      Validator v;
      if (! v.validate(filename, t.tokenize(fileContent))) {
        hasError = true;
      }  
    }

    delete[] fileContent;
  }

  return hasError ? EXIT_FAILURE : EXIT_SUCCESS;
}

