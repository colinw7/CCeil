#include <CCeilPI.h>

using std::string;
using std::ostream;

ClParserScopePtr
ClParserScopeMgr::
getScope(const string &name)
{
  ScopeMap::const_iterator p = scope_map_.find(name);

  if (p != scope_map_.end())
    return (*p).second;

  ClParserScopePtr scope = createScope(parent_, name);

  scope_map_[name] = scope;

  return scope;
}

ClParserScopePtr
ClParserScopeMgr::
getScope(const StringVectorT &names)
{
  uint num_names = names.size();

  assert(num_names > 0);

  const string &name = names.front();

  ClParserScopePtr scope = getScope(name);

  if (! scope.isValid())
    return scope;

  if (num_names == 1)
    return scope;

  StringVectorT names1;

  for (uint i = 1; i < num_names; ++i)
    names1.push_back(names[i]);

  return scope->getScope(names1);
}

ClParserScopePtr
ClParserScopeMgr::
createScope(ClParserScope *parent, const string &name)
{
  ClParserScope *scope = new ClParserScope(parent, name);

  return ClParserScopePtr(scope);
}

//--------------

void
ClParserScope::
print() const
{
  ClParserInst->output("%s", name_.c_str());
}

void
ClParserScope::
print(ostream &os) const
{
  CStrUtil::fprintf(os, "%s", name_.c_str());
}

void
ClParserScope::
debugPrint() const
{
  fprintf(stderr, "%s ", name_.c_str());
}
