#include <CCeilL.h>

ClLanguageLabelData *
ClLanguageLabelDataMgr::
createLabelData(const std::string &name)
{
  ClLanguageLabelData *label_data = new ClLanguageLabelData(name);

  label_stack_.push_back(label_data);

  return label_data;
}

void
ClLanguageLabelDataMgr::
clearLabelDataStack()
{
  uint num_labels = label_stack_.size();

  while (num_labels--) {
    ClLanguageLabelData *label_data = label_stack_[num_labels];

    label_stack_.pop_back();

    delete label_data;
  }

  label_stack_.clear();
}

void
ClLanguageLabelDataMgr::
clearCurrentLabelDatas()
{
  LabelDataMap::const_iterator p1 = current_label_list_.begin();
  LabelDataMap::const_iterator p2 = current_label_list_.end  ();

  for ( ; p1 != p2; ++p1)
    delete (*p1).second;

  current_label_list_.clear();
}

ClLanguageLabelData *
ClLanguageLabelDataMgr::
getCurrentLabelData(const std::string &name) const
{
  ClLanguageLabelData *label_data = nullptr;

  LabelDataMap::const_iterator p = current_label_list_.find(name);

  if (p != current_label_list_.end())
    label_data = (*p).second;

  return label_data;
}

void
ClLanguageLabelDataMgr::
addCurrentLabelData(ClLanguageLabelData *label_data)
{
  current_label_list_[label_data->getName()] = label_data;
}

void
ClLanguageLabelDataMgr::
removeCurrentLabelData(ClLanguageLabelData *label_data)
{
  current_label_list_.erase(label_data->getName());
}

//---------------

ClLanguageLabelData::
ClLanguageLabelData(const std::string &name) :
 name_(name)
{
  depth_ = ClLanguageMgrInst->getDepth();
}

ClLanguageLabelData::
~ClLanguageLabelData()
{
}
