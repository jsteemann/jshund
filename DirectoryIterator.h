
#ifndef JSHUND_DIRECTORYITERATOR_H
#define JSHUND_DIRECTORYITERATOR_H 1

#include <string>
#include <vector>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h> 
#include <dirent.h>

namespace jshund {
  using namespace std;

  class DirectoryIterator {
    public:
      DirectoryIterator () {
      }

      ~DirectoryIterator () {
      }
        
      static bool isFile (const char* name) {
        struct stat filestatus;
  
        if (stat(name, &filestatus) != 0 || (filestatus.st_mode & S_IFMT) != S_IFREG) {
          return false;
        }

        return true;
      }
      
      static bool isDir (const char* name) {
        struct stat filestatus;
  
        if (stat(name, &filestatus) != 0 || (filestatus.st_mode & S_IFMT) != S_IFDIR) {
          return false;
        }

        return true;
      }

      void addDir (vector<string>& files, const char* argv) {
        struct dirent* d;
        DIR* dh;

        dh = opendir(argv); 
        if (dh == 0) {
          return;
        }

        while ((d = readdir(dh))) {
          if (d->d_type == DT_REG) {
            ostringstream filename;
            filename << argv << '/' << d->d_name;
            files.push_back(filename.str());
          }
          else if (d->d_type == DT_DIR) {
            ostringstream filename;
            string name(d->d_name);

            if (name == ".." || name == ".") {
              continue;
            }
            filename << argv << '/' << name; 
            addDir(files, filename.str().c_str());
          } 
        }

        closedir(dh);
      }

      void addFiles (vector<string>& files, const char* argv) {
        if (isFile(argv)) {
          files.push_back(string(argv));
        }
        else if (isDir(argv)) {
          addDir(files, argv);
        }
      }
  };
}

#endif

