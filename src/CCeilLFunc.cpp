#include <CCeilLI.h>

ClLanguageFunc *
ClLanguageFuncMgr::
createFunc(const std::string &name, const FuncArgList &args,
           const ClLanguageCommandList &command_list)
{
  ClLanguageFunc *function = lookupFunc(name);

  delete function;

  function = new ClLanguageFunc(name, args, command_list);

  function_map_[name] = function;

  return function;
}

void
ClLanguageFuncMgr::
deleteFunc(const std::string &name)
{
  ClLanguageFunc *function = lookupFunc(name);

  if (function)
    return;

  function_map_.erase(name);

  delete function;
}

ClLanguageFunc *
ClLanguageFuncMgr::
lookupFunc(const std::string &name) const
{
  ClLanguageFunc *function = nullptr;

  FuncMap::const_iterator p = function_map_.find(name);

  if (p != function_map_.end())
    function = (*p).second;

  return function;
}

void
ClLanguageFuncMgr::
printFuncs() const
{
  FuncMap::const_iterator p1 = function_map_.begin();
  FuncMap::const_iterator p2 = function_map_.end  ();

  for ( ; p1 != p2; ++p1)
    (*p1).second->print();
}

void
ClLanguageFuncMgr::
deleteFuncs()
{
  FuncMap::const_iterator p1 = function_map_.begin();
  FuncMap::const_iterator p2 = function_map_.end  ();

  for ( ; p1 != p2; ++p1)
    delete (*p1).second;

  function_map_.clear();
}

//-----------------

ClLanguageFunc::
ClLanguageFunc(const std::string &name, const ArgList &args,
               const ClLanguageCommandList &command_list) :
 name_(name), args_(args), command_list_(command_list)
{
}

ClLanguageFunc::
~ClLanguageFunc()
{
  for (auto &command : command_list_)
    delete command;

  command_list_.clear();
}

void
ClLanguageFunc::
print() const
{
  ClLanguageMgrInst->output("%s(", name_.c_str());

  uint num_args = args_.size();

  if (num_args > 0) {
    ClLanguageMgrInst->output("%s", args_[0]->getName().c_str());

    for (uint i = 1; i < num_args; i++)
      ClLanguageMgrInst->output(",%s", args_[i]->getName().c_str());
  }

  ClLanguageMgrInst->output(")\n");
}
