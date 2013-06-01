#ifndef CL_L_LABEL_H
#define CL_L_LABEL_H

#define ClLanguageLabelDataMgrInst ClLanguageLabelDataMgr::getInstance()

class ClLanguageLabelDataMgr {
 private:
  typedef std::vector<ClLanguageLabelData *>           LabelDataList;
  typedef std::map<std::string, ClLanguageLabelData *> LabelDataMap;

  LabelDataList label_stack_;
  LabelDataMap  current_label_list_;

 public:
  static ClLanguageLabelDataMgr *getInstance() {
    static ClLanguageLabelDataMgr *instance;

    if (! instance)
      instance = new ClLanguageLabelDataMgr;

    return instance;
  }

  ClLanguageLabelData *createLabelData(const std::string &name);

  void clearLabelDataStack();

  void clearCurrentLabelDatas();

  ClLanguageLabelData *getCurrentLabelData(const std::string &name) const;

  void addCurrentLabelData(ClLanguageLabelData *label_data);
  void removeCurrentLabelData(ClLanguageLabelData *label_data);
};

class ClLanguageLabelData {
 private:
  std::string name_;
  int         depth_;

 private:
  friend class ClLanguageLabelDataMgr;

  ClLanguageLabelData(const std::string &name);
 ~ClLanguageLabelData();

 public:
  const std::string &getName() const { return name_; }

  int getDepth() const { return depth_; }

  bool isName(const std::string &name) const { return name_ == name; }
};

#endif
