#ifndef CFile_H
#define CFile_H

#include <string>
#include <cstdio>
#include <sys/stat.h>

namespace CFile {
  inline bool exists(const std::string &filename) {
    struct stat file_stat;

    return (stat(filename.c_str(), &file_stat) == 0);
  }

  inline bool isRegular(const std::string &filename) {
    struct stat file_stat;

    if (stat(filename.c_str(), &file_stat) != 0)
      return false;

    return S_ISREG(file_stat.st_mode);
  }

  inline bool readLine(FILE *fp, std::string &line) {
    if (fp == NULL) return false;

    // get first character from file
    int c = fgetc(fp);
    if (c == EOF) return false;

    //-----

    // read one character at a time and add it to the buffer until
    // we have a newline or we reach the end of the File

    line = "";

    while (c != EOF && c != '\n') {
      line += c;

      c = fgetc(fp);
    }

    //-----

    return true;
  }
};

#endif
