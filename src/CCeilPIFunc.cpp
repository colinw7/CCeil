#include <CCeilPIFuncP.h>

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
isInternFn(const std::string &name) const
{
  return getInternFn(name).isValid();
}

ClParserInternFnPtr
ClParserInternFnMgr::
getInternFn(const std::string &name) const
{
  auto p = internfn_map_.find(name);

  if (p != internfn_map_.end())
    return (*p).second;

  return ClParserInternFnPtr();
}

void
ClParserInternFnMgr::
addInternFns()
{
  for (uint i = 0; parser_internfn[i].type != CLParserInternFnType::NONE; ++i) {
    ClParserInternFn *internfn = new ClParserInternFn(parser_internfn[i]);

    internfn_map_[parser_internfn[i].name] = internfn;
  }
}

//-----------------------------

ClParserInternFn::
ClParserInternFn(CLParserInternFnType type, const std::string &name,
                 uint num_args, ClParserArg *args, bool composite) :
 data_(type, name, num_args, args, composite)
{
}

ClParserInternFn::
ClParserInternFn(const ClParserInternFnData &data) :
 data_(data)
{
}

ClParserInternFn *
ClParserInternFn::
dup() const
{
  return new ClParserInternFn(data_);
}

//------

void
ClParserInternFn::
print() const
{
  ClParserInst->output(" %s ", getName().c_str());
}

void
ClParserInternFn::
print(std::ostream &os) const
{
  os << " " << getName() << " ";
}

void
ClParserInternFn::
debugPrint() const
{
  fprintf(stderr, " %s ", getName().c_str());
}
