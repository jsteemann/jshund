
#include <stdlib.h>
#include <string>
#include <iostream>

#include "DirectoryIterator.h"
#include "FileReader.h"
#include "Tokenizer.h"
#include "Validator.h"

using namespace jshund;

static std::vector<std::string> AddFiles (int argc, char* argv[]) {
  std::vector<std::string> files;

  DirectoryIterator dir;
  bool separator = false;
  
  // add files/directories specified in argv
  for (int i = 1; i < argc; ++i) {
    const std::string argument(argv[i]);

    if (argument.substr(0, 2) == "--" && ! separator) {
      // option/value separator
      if (argument.size() == 2) {
        separator = true;
      }
      continue;
    }

    dir.addFiles(files, argument);
  }

  return files;
}

static bool ProcessFiles (std::vector<std::string>& files, bool colors, bool globals, bool debug) {
  bool hasError = false;

  FileReader f;
  for (size_t i = 0; i < files.size(); ++i) {
    const std::string filename = files[i];

    char* fileContent = f.read(filename);

    if (fileContent == 0) {
      std::cerr << "could not read input file '" << filename << "'" << std::endl;
      hasError = true;
    }
    else {
      if (debug) {
        std::cout << "parsing input file '" << filename << "'" << std::endl;
        std::cout << "----------------------------------------------------------" << std::endl << std::endl;
      }
      Tokenizer t(debug);
      Validator v(colors, globals);
      if (! v.validate(filename, t.tokenize(fileContent))) {
        hasError = true;
      }
    }

    delete[] fileContent;
  }

  return hasError;
}

static void Bark () {
  std::cout << "   _     _                     _  " << std::endl;
  std::cout << "  (_)___| |__  _   _ _ __   __| | " << std::endl;
  std::cout << "  | / __| '_ \\| | | | '_ \\ / _` | " << std::endl;
  std::cout << "  | \\__ \\ | | | |_| | | | | (_| | " << std::endl;
  std::cout << " _/ |___/_| |_|\\__,_|_| |_|\\__,_| " << std::endl;
  std::cout << "|__/                              " << std::endl;
  std::cout << std::endl;
}

int main (int argc, char* argv[]) {
  if (argc < 2) {
    // not enough arguments
    Bark();
    std::cout << "usage: " << argv[0] << " <file>" << std::endl;
    return EXIT_FAILURE;
  }
 
  // parse options
  bool colors  = true;
  bool globals = true; 
  bool debug   = false; 

  for (int i = 1; i < argc; ++i) {
    const std::string argument(argv[i]);

    if (argument.size() > 2 && argument.substr(0, 2) == "--") { 
      if (argument == "--colors") {
        colors = true;
      }
      else if (argument == "--no-colors") {
        colors = false;
      }
      else if (argument == "--globals") {
        globals = true;
      }
      else if (argument == "--no-globals") {
        globals = false;
      }
      else if (argument == "--debug") {
        debug = true;
      }
      else if (argument == "--help" || argument == "--version" || argument == "--bark") {
        Bark();
        return EXIT_SUCCESS;
      }
    }
  }

  // add files
  std::vector<std::string> files = AddFiles(argc, argv);

  int errors = ProcessFiles(files, colors, globals, debug);

  // return status code based on whether there were any errors
  if (errors == 0) {
    return EXIT_SUCCESS;
  }

  return EXIT_FAILURE;
}

