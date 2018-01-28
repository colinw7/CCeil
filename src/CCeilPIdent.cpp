#include <CCeilPI.h>

ClParserIdentifierMgr *
ClParserIdentifierMgr::
getInstance()
{
  static ClParserIdentifierMgr *instance;

  if (! instance)
    instance = new ClParserIdentifierMgr;

  return instance;
}

ClParserIdentifierPtr
ClParserIdentifierMgr::
createIdentifier(const std::string &name)
{
  ClParserIdentifier *identifier = new ClParserIdentifier(name);

  return ClParserIdentifierPtr(identifier);
}

//--------------

ClParserScopePtr
ClParserIdentifier::
getScope() const
{
  ClParserScopePtr scope;

  ScopeList::const_iterator ps1 = scopes_.begin();
  ScopeList::const_iterator ps2 = scopes_.end  ();

  for ( ; ps1 != ps2; ++ps1) {
    if (scope.isValid())
      scope = scope->getScope(*ps1);
    else
      scope = ClParserInst->getScope(*ps1);
  }

  return scope;
}

//------

void
ClParserIdentifier::
print() const
{
  ClParserInst->output("%s", name_.c_str());
}

void
ClParserIdentifier::
print(std::ostream &os) const
{
  os << name_;
}

void
ClParserIdentifier::
debugPrint() const
{
  if      (isFunction())
    fprintf(stderr, " fn");
  else if (isStructPart())
    fprintf(stderr, " str");
  else if (isVariable())
    fprintf(stderr, " var");
  else if (isArgument())
    fprintf(stderr, " arg");
  else
    fprintf(stderr, " ??");

  fprintf(stderr, " %s ", name_.c_str());
}
