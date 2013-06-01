#ifndef CL_L_CONVERT_H
#define CL_L_CONVERT_H

#define CL_MAX_CONVERT_ARGS 20

struct ClLanguageConverter {
  std::string           from_type;
  std::string           to_type;
  ClLanguageConvertProc proc;
};

#define ClLanguageConverterMgrInst ClLanguageConverterMgr::getInstance()

class ClLanguageConverterMgr {
 private:
  typedef std::vector<ClLanguageConverter *> ConverterArray;

  ConverterArray converter_list_;

 public:
  static ClLanguageConverterMgr *getInstance() {
    static ClLanguageConverterMgr *instance = NULL;

    if (! instance)
      instance = new ClLanguageConverterMgr;

    return instance;
  }

  void setTypedStringArrayArg
        (ClLanguageArgs *args, int num, const char *from_type, int num_args, char *arg, ...);
  void getTypedStringArrayArg
        (ClLanguageArgs *args, int num, const char *from_type, int num_args, char *arg, ...);
  bool convertStringMatrixToStringArray
        (char ***, uint **, uint *, char *, int, int);
  bool convertStringArrayToStringMatrix
        (char **, uint *, uint, char **, int *, int);
  bool convertCharMatrixToStringArray
        (char ***, uint **, uint *, char *, int, int);
  bool convertStringArrayToCharMatrix
        (char **, uint *, uint, char **, int *, int);

  void addConverter(const std::string &from_type, const std::string &to_type,
                    ClLanguageConvertProc proc);

  ClLanguageConverter *getConverter(const std::string &from_type,
                                    const std::string &to_type) const;

 private:
  ClLanguageConverterMgr() { }
 ~ClLanguageConverterMgr() { }
};

#endif
