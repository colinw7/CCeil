#include <CCeilPI.h>

ClParserScopePtr
ClParserScopeMgr::
getScope(const std::string &name)
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

  const std::string &name = names.front();

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
createScope(ClParserScope *parent, const std::string &name)
{
  ClParserScope *scope = new ClParserScope(parent, name);

  return ClParserScopePtr(scope);
}

//--------------

ClParserScope::
ClParserScope(ClParser *parser, const std::string &name) :
 parser_(parser), parent_(nullptr), name_(name)
{
  varMgr_ = new ClParserVarMgr(parser_);
}

ClParserScope::
ClParserScope(ClParserScope *parent, const std::string &name) :
 parser_(parent->parser_), parent_(parent), name_(name)
{
  varMgr_ = new ClParserVarMgr(parser_);
}

ClParserScope::
ClParserScope(const ClParserScope &scope) :
 parser_(scope.parser_), parent_(scope.parent_), name_(scope.name_), funcMgr_(scope.funcMgr_),
 internFnMgr_(scope.internFnMgr_), userFnMgr_(scope.userFnMgr_), scopeMgr_(scope.scopeMgr_)
{
  varMgr_ = new ClParserVarMgr(*varMgr_);
}

ClParserScope::
~ClParserScope()
{
  delete varMgr_;
}

const ClParserScope &
ClParserScope::
operator=(const ClParserScope &scope)
{
  delete varMgr_;

  parser_      = scope.parser_;
  parent_      = scope.parent_;
  name_        = scope.name_;
  varMgr_      = new ClParserVarMgr(*scope.varMgr_);
  funcMgr_     = scope.funcMgr_;
  internFnMgr_ = scope.internFnMgr_;
  userFnMgr_   = scope.userFnMgr_;
  scopeMgr_    = scope.scopeMgr_;

  return *this;
}

ClParserScope *
ClParserScope::
dup() const
{
  return new ClParserScope(*this);
}


ClParserScopePtr
ClParserScope::
getScope(const std::string &name)
{
  return scopeMgr_.getScope(name);
}

ClParserScopePtr
ClParserScope::
getScope(const StringVectorT &names)
{
  return scopeMgr_.getScope(names);
}

ClParserVarPtr
ClParserScope::
getVariable(const std::string &name, bool create)
{
  return varMgr_->getVariable(name, create);
}

void
ClParserScope::
print() const
{
  ClParserInst->output("%s", name_.c_str());
}

void
ClParserScope::
print(std::ostream &os) const
{
  os << name_;
}

void
ClParserScope::
debugPrint() const
{
  fprintf(stderr, "%s ", name_.c_str());
}
