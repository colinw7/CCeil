#include <CCeilPOpP.h>

ClParserOperatorMgr *
ClParserOperatorMgr::
getInstance()
{
  static ClParserOperatorMgr *instance;

  if (! instance)
    instance = new ClParserOperatorMgr;

  return instance;
}

ClParserOperatorMgr::
ClParserOperatorMgr()
{
  initOperatorMap();
}

ClParserOperatorMgr::
~ClParserOperatorMgr()
{
}

bool
ClParserOperatorMgr::
initOperatorMap()
{
  for (uint i = 0; parser_operators[i]; ++i) {
    auto *op = new ClParserOperator(*parser_operators[i]);

    operator_map_[parser_operators[i]->type] = ClParserOperatorPtr(op);
  }

  return true;
}

ClParserOperatorPtr
ClParserOperatorMgr::
getOperator(ClParserOperatorType type) const
{
  auto p = operator_map_.find(type);

  if (p != operator_map_.end())
    return (*p).second;

  return ClParserOperatorPtr();
}

//--------------------------

ClParserOperator::
ClParserOperator(const ClParserOperatorData &data)
{
  data_.type          = data.type;
  data_.token         = data.token;
  data_.name          = data.name;
  data_.precedence    = data.precedence;
  data_.associativity = data.associativity;
  data_.types         = data.types;
}

ClParserOperator::
ClParserOperator(const ClParserOpData &data)
{
  data_.type          = data.type;
  data_.token         = data.token;
  data_.name          = data.name;
  data_.precedence    = data.precedence;
  data_.associativity = data.associativity;
  data_.types         = data.types;
}

ClParserOperator *
ClParserOperator::
dup() const
{
  return new ClParserOperator(data_);
}

bool
ClParserOperator::
checkUnstack(ClParserOperatorPtr last_operator) const
{
  bool unstack = false;

  if (last_operator &&
      (last_operator->data_.precedence > data_.precedence ||
       (last_operator->data_.precedence == data_.precedence &&
        data_.associativity == CLParserOperatorAssociate::L_TO_R)))
    unstack = true;

  return unstack;
}

//------

void
ClParserOperator::
print() const
{
  ClParserInst->output(" %s ", data_.token.c_str());
}

void
ClParserOperator::
print(std::ostream &os) const
{
  os << " " << data_.token << " ";
}

void
ClParserOperator::
debugPrint() const
{
  fprintf(stderr, " %s ", data_.token.c_str());
}
