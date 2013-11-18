
#ifndef JSHUND_DIRECTORYITERATOR_H
#define JSHUND_DIRECTORYITERATOR_H 1

#include <string>
#include <vector>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h> 
#include <dirent.h>

namespace jshund {
  class DirectoryIterator {
    public:
      DirectoryIterator () {
      }

      ~DirectoryIterator () {
      }

    public:
      void addFiles (std::vector<std::string>& files, std::string const& argument) {
        if (isFile(argument)) {
          files.push_back(std::string(argument));
        }
        else if (isDir(argument)) {
          addDir(files, argument);
        }
      }

    private:
        
      static bool isFile (std::string const& name) {
        struct stat filestatus;
  
        if (::stat(name.c_str(), &filestatus) != 0 || (filestatus.st_mode & S_IFMT) != S_IFREG) {
          return false;
        }

        return true;
      }
      
      static bool isDir (std::string const& name) {
        struct stat filestatus;
  
        if (::stat(name.c_str(), &filestatus) != 0 || (filestatus.st_mode & S_IFMT) != S_IFDIR) {
          return false;
        }

        return true;
      }

      void addDir (std::vector<std::string>& files, std::string path) {
        static const char separator = '/';

        if (! path.empty() && path[path.size() - 1] == separator) {
          // strip end separator
          path = path.substr(0, path.size() - 1);
        }

        DIR* dh = ::opendir(path.c_str()); 

        if (dh == 0) {
          return;
        }

        struct dirent* d;
        while ((d = ::readdir(dh))) {
          if (d->d_type == DT_REG) {
            std::ostringstream filename;
            filename << path << separator << d->d_name;
            files.push_back(filename.str());
          }
          else if (d->d_type == DT_DIR) {
            std::string name(d->d_name);

            if (name == ".." || name == ".") {
              continue;
            }

            std::ostringstream filename;
            filename << path << separator << name; 
            addDir(files, filename.str().c_str());
          } 
        }

        ::closedir(dh);
      }

  };
}

#endif

