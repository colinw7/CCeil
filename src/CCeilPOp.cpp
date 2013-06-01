#include <CCeilPOpP.h>

using std::ostream;

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
  for (uint i = 0; parser_operators[i] != NULL; ++i)
    operator_map_[parser_operators[i]->type] =
      new ClParserOperator(*parser_operators[i]);

  return true;
}

ClParserOperatorPtr
ClParserOperatorMgr::
getOperator(ClParserOperatorType type) const
{
  OperatorMap::const_iterator p = operator_map_.find(type);

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

  if (last_operator.isValid() &&
      (last_operator->data_.precedence > data_.precedence ||
       (last_operator->data_.precedence == data_.precedence &&
        data_.associativity == CL_PARSER_OP_ASSOCIATE_L_TO_R)))
    unstack = true;

  return unstack;
}

void
ClParserOperator::
print() const
{
  ClParserInst->output(" %s ", data_.token.c_str());
}

void
ClParserOperator::
print(ostream &os) const
{
  CStrUtil::fprintf(os, " %s ", data_.token.c_str());
}

void
ClParserOperator::
debugPrint() const
{
  fprintf(stderr, " %s ", data_.token.c_str());
}
