#include <CCeilLI.h>

ClLanguageProc *
ClLanguageProcMgr::
createProc(const std::string &name, const ProcArgList &args,
           const ClLanguageCommandList &command_list)
{
  ClLanguageProc *proc = lookupProc(name);

  delete proc;

  proc = new ClLanguageProc(name, args, command_list);

  proc_map_[name] = proc;

  return proc;
}

void
ClLanguageProcMgr::
deleteProc(const std::string &name)
{
  ClLanguageProc *proc = lookupProc(name);

  if (proc)
    return;

  proc_map_.erase(name);

  delete proc;
}

ClLanguageProc *
ClLanguageProcMgr::
lookupProc(const std::string &name) const
{
  ClLanguageProc *procedure = nullptr;

  auto p = proc_map_.find(name);

  if (p != proc_map_.end())
    procedure = (*p).second;

  return procedure;
}

void
ClLanguageProcMgr::
printProcs() const
{
  auto p1 = proc_map_.begin();
  auto p2 = proc_map_.end  ();

  for ( ; p1 != p2; ++p1)
    (*p1).second->print();
}

void
ClLanguageProcMgr::
deleteProcs()
{
  auto p1 = proc_map_.begin();
  auto p2 = proc_map_.end  ();

  for ( ; p1 != p2; ++p1)
    delete (*p1).second;

  proc_map_.clear();
}

//-----------------

ClLanguageProc::
ClLanguageProc(const std::string &name, const ArgList &args,
               const ClLanguageCommandList &command_list) :
 name_(name), args_(args), command_list_(command_list), ref_count_(0)
{
}

ClLanguageProc::
~ClLanguageProc()
{
  for (auto &command : command_list_)
    delete command;

  command_list_.clear();
}

void
ClLanguageProc::
print() const
{
  ClLanguageMgrInst->output("%s(", name_.c_str());

  uint num_args = args_.size();

  if (num_args > 0) {
    if (args_[0]->isReturned())
      ClLanguageMgrInst->output("&%s", args_[0]->getName().c_str());
    else
      ClLanguageMgrInst->output("%s", args_[0]->getName().c_str());

    for (uint i = 1; i < num_args; ++i) {
      if (args_[i]->isReturned())
        ClLanguageMgrInst->output(",&%s", args_[i]->getName().c_str());
      else
        ClLanguageMgrInst->output(",%s", args_[i]->getName().c_str());
    }
  }

  ClLanguageMgrInst->output(")\n");
}
