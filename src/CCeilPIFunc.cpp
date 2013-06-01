#include <CCeilPIFuncP.h>

using std::string;
using std::ostream;

ClParserInternFnMgr::
ClParserInternFnMgr()
{
  addInternFns();
}

ClParserInternFnMgr::
~ClParserInternFnMgr()
{
}

bool
ClParserInternFnMgr::
isInternFn(const string &name) const
{
  return getInternFn(name).isValid();
}

ClParserInternFnPtr
ClParserInternFnMgr::
getInternFn(const string &name) const
{
  InternFnMap::const_iterator p = internfn_map_.find(name);

  if (p != internfn_map_.end())
    return (*p).second;

  return ClParserInternFnPtr();
}

void
ClParserInternFnMgr::
addInternFns()
{
  for (uint i = 0; i < PARSER_NO_INTERN_FNS; ++i) {
    ClParserInternFn *internfn = new ClParserInternFn(parser_internfn[i]);

    internfn_map_[parser_internfn[i].name] = internfn;
  }
}

//-----------------------------

ClParserInternFn::
ClParserInternFn(CLParserInternFnType type, const string &name,
                 uint num_args, ClParserArg *args, bool composite) :
 type_(type), name_(name), num_args_(num_args), args_(args), composite_(composite)
{
}

ClParserInternFn::
ClParserInternFn(const ClParserInternFnData &data) :
 type_(data.type), name_(data.name), num_args_(data.num_args),
 args_(data.args), composite_(data.composite)
{
}

ClParserInternFn *
ClParserInternFn::
dup() const
{
  return new ClParserInternFn(type_, name_, num_args_, args_, composite_);
}

void
ClParserInternFn::
print() const
{
  ClParserInst->output(" %s ", name_.c_str());
}

void
ClParserInternFn::
print(ostream &os) const
{
  CStrUtil::fprintf(os, " %s ", name_.c_str());
}

void
ClParserInternFn::
debugPrint() const
{
  fprintf(stderr, " %s ", name_.c_str());
}
