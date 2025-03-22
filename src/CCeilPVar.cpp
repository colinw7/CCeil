#include <CCeilPI.h>
#include <COSNaN.h>

#include <cassert>

ClParserVarMgr::
ClParserVarMgr(ClParser *parser) :
 parser_(parser)
{
  varMap_ = new VarMap;

  addStandardVariables();
}

ClParserVarMgr::
~ClParserVarMgr()
{
  delete varMap_;

  for (auto &varMap : varMapStack_)
    delete varMap;
}

ClParserVarPtr
ClParserVarMgr::
createStdVar(const std::string &name, ClParserValuePtr value)
{
  ClParserVarPtr variable = createVar(name, value);

  variable->setFixedValue();
  variable->setNoDelete();

  return variable;
}

ClParserVarPtr
ClParserVarMgr::
createVar(const std::string &name, ClParserValuePtr value)
{
  assert(! isLocalVariable(name));

  return createVarI(name, value);
}

ClParserVarPtr
ClParserVarMgr::
createVarI(const std::string &name, ClParserValuePtr value)
{
  ClParserVar *variable = new ClParserVar(name, value);

  return addVariable(variable);
}

ClParserVarRefPtr
ClParserVarMgr::
createVarRef(ClParserVarPtr variable, const ClParserValueArray &subscripts)
{
  ClParserVarRef *var_ref = new ClParserVarRef(variable, subscripts);

  return ClParserVarRefPtr(var_ref);
}

ClParserStructVarRefPtr
ClParserVarMgr::
createStructVarRef(ClParserVarPtr variable, const std::string &name,
                   const ClParserValueArray &subscripts)
{
  ClParserStructVarRef *svar_ref = new ClParserStructVarRef(variable, name, subscripts);

  return ClParserStructVarRefPtr(svar_ref);
}

void
ClParserVarMgr::
newVariableList()
{
  varMapStack_.push_back(varMap_);

  varMap_ = new VarMap;
}

void
ClParserVarMgr::
oldVariableList()
{
  if (! varMapStack_.empty()) {
    delete varMap_;

    varMap_ = varMapStack_.back();

    varMapStack_.pop_back();
  }
}

void
ClParserVarMgr::
deleteAllVariables()
{
  varMap_->clear();

  if (varMapStack_.empty())
    addStandardVariables();
}

void
ClParserVarMgr::
addStandardVariables()
{
return;
  double         nan;
  ClParserVarPtr variable;

  COSNaN::set_nan(&nan);

  createStdVar("_NAN", ClParserValueMgrInst->createValue(nan));

  /*--------------*/

  createStdVar("_PI", ClParserValueMgrInst->createValue(M_PI));

  /*--------------*/

  createStdVar("_TRUE" , ClParserValueMgrInst->createValue(1L));
  createStdVar("_FALSE", ClParserValueMgrInst->createValue(0L));

  /*--------------*/

  createStdVar("_NULL", ClParserValueMgrInst->createValue(0L));

  /*--------------*/

  createStdVar("__INTEGER"   , ClParserValueMgrInst->createValue
                                (long(CL_PARSER_VALUE_TYPE_INTEGER)));
  createStdVar("__REAL"      , ClParserValueMgrInst->createValue
                                (long(CL_PARSER_VALUE_TYPE_REAL)));
  createStdVar("__STRING"    , ClParserValueMgrInst->createValue
                                (long(CL_PARSER_VALUE_TYPE_STRING)));
  createStdVar("__ARRAY"     , ClParserValueMgrInst->createValue
                                (long(CL_PARSER_VALUE_TYPE_ARRAY)));
  createStdVar("__LIST"      , ClParserValueMgrInst->createValue
                                (long(CL_PARSER_VALUE_TYPE_LIST)));
  createStdVar("__DICTIONARY", ClParserValueMgrInst->createValue
                                (long(CL_PARSER_VALUE_TYPE_DICTIONARY)));
  createStdVar("__STRUCTURE" , ClParserValueMgrInst->createValue
                                (long(CL_PARSER_VALUE_TYPE_STRUCTURE)));
}

ClParserVarPtr
ClParserVarMgr::
addVariable(ClParserVar *var)
{
  VarMap *varMap = varMap_;

  if (var->isGlobal() && ! varMapStack_.empty())
    varMap = varMapStack_[0];

  const std::string &name = var->getName();

  (*varMap)[name] = ClParserVarPtr(var);

  return (*varMap)[name];
}

void
ClParserVarMgr::
removeVariable(const std::string &name)
{
  VarMap *varMap = varMap_;

  VarMap::iterator p = varMap->find(name);

  if (p != varMap->end()) {
    varMap->erase(p);

    return;
  }

  uint num = uint(varMapStack_.size());

  for (int i = int(num - 1); i >= 0; --i) {
    varMap = varMapStack_[uint(i)];

    p = varMap->find(name);

    if (p != varMap->end()) {
      varMap->erase(p);

      return;
    }
  }
}

bool
ClParserVarMgr::
isVariable(const std::string &name) const
{
  ClParserVarPtr var;

  return getVariableI(name, var);
}

bool
ClParserVarMgr::
isLocalVariable(const std::string &name) const
{
  ClParserVarPtr var;

  return getVariableI(name, var, false);
}

ClParserVarPtr
ClParserVarMgr::
getVariable(const std::string &name, bool create) const
{
  ClParserVarPtr var;

  if (! getVariableI(name, var) && create) {
    ClParserVarMgr *th = const_cast<ClParserVarMgr *>(this);

    return th->createVar(name, ClParserValueMgrInst->createValue(0L));
  }

  return var;
}

bool
ClParserVarMgr::
getVariableI(const std::string &name, ClParserVarPtr &var, bool parentScope) const
{
  if (name == "")
    return false;

  if (! ClParserVar::isValidName(name))
    return false;

  // get in current scope
  VarMap::iterator p = varMap_->find(name);

  if (p != varMap_->end()) {
    var = (*p).second;

    return true;
  }

  // check parent scopes if allowed
  bool found = false;

  if (parentScope) {
    uint num = uint(varMapStack_.size());

    for (int i = int(num - 1); i >= 0; i--) {
      VarMap *varMap = varMapStack_[uint(i)];

      VarMap::iterator pv = varMap->find(name);

      if (pv != varMap->end()) {
        var = (*pv).second;

        found = true;

        break;
      }
    }
  }

  // not found in any scope so fail
  if (! found)
    return false;

  // create variable in this scope
  if (var->getValue() && ! var->isGlobal()) {
    ClParserVarMgr *th = const_cast<ClParserVarMgr *>(this);

    var = th->createVarI(var->getName(), var->getValue());
  }

  return true;
}

ClParserValuePtr
ClParserVarMgr::
getVariableValue(const std::string &name) const
{
  ClParserVarPtr var;

  if (! getVariableI(name, var) || ! var->getValue())
    return ClParserValuePtr();

  return var->getValue();
}

void
ClParserVarMgr::
setVariableValue(const std::string &name, ClParserValuePtr value)
{
  ClParserVarPtr var;

  if (! getVariableI(name, var))
    createVar(name, value);
  else
    var->setValue(value);
}

void
ClParserVarMgr::
printAllVariables() const
{
  VarMap::iterator p1 = varMap_->begin();
  VarMap::iterator p2 = varMap_->end  ();

  for ( ; p1 != p2; ++p1)
    (*p1).second->print();

  uint num = uint(varMapStack_.size());

  for (int i = int(num - 1); i >= 0; i--) {
    parser_->output("\n");

    auto pt1 = varMapStack_[uint(i)]->begin();
    auto pt2 = varMapStack_[uint(i)]->end  ();

    for ( ; pt1 != pt2; ++pt1)
      (*pt1).second->print();
  }
}

void
ClParserVarMgr::
addVariableProc(const std::string &name, ClParserVarMgr::VarProc proc, void *data)
{
  if (! proc)
    return;

  VarProcData *proc_data = new VarProcData(name, proc, data);

  varProcDataList_.push_back(proc_data);
}

void
ClParserVarMgr::
deleteVariableProc(const std::string &name, ClParserVarMgr::VarProc proc, void *data)
{
  auto p1 = varProcDataList_.begin();
  auto p2 = varProcDataList_.end  ();

  for ( ; p1 != p2; ++ p1) {
    VarProcData *proc_data = *p1;

    if (proc_data->name == name &&
        proc_data->proc == proc &&
        proc_data->data == data) {
      varProcDataList_.remove(proc_data);

      delete proc_data;

      return;
    }
  }
}

void
ClParserVarMgr::
callVariableProc(ClParserVarPtr variable)
{
  auto p1 = varProcDataList_.begin();
  auto p2 = varProcDataList_.end  ();

  for ( ; p1 != p2; ++ p1) {
    VarProcData *proc_data = *p1;

    if (proc_data->name == variable->getName()) {
      (*proc_data->proc)(variable, proc_data->data);
      return;
    }
  }
}

//--------------------

ClParserValuePtr
ClParserVar::
getValue() const
{
  return value_;
}

void
ClParserVar::
setValue(ClParserValuePtr value)
{
  value_ = value;
}

bool
ClParserVar::
isValidName(const std::string &name)
{
  return CStrUtil::isIdentifier(name);
}

ClParserVar::
ClParserVar(const std::string &name) :
 name_(name), value_(ClParserValueMgrInst->createValue(0L)), flags_(0)
{
  init();
}

ClParserVar::
ClParserVar(const std::string &name, ClParserValuePtr value) :
 name_(name), value_(value), flags_(0)
{
  init();
}

ClParserVar::
ClParserVar(const ClParserVar &var) :
 name_(var.name_), value_(var.value_), flags_(0)
{
  init();
}

ClParserVar::
~ClParserVar()
{
  term();
}

ClParserVar *
ClParserVar::
dup() const
{
  return new ClParserVar(*this);
}

void
ClParserVar::
init()
{
  if (! isValidName(name_) ||
      ! ClParserInst->isValidNewName(ClParserNameType::VARIABLE, name_))
    ClErrThrow(ClErr::INVALID_VARIABLE_NAME);

  if (name_[0] == '_')
    flags_ |= CLP_VAR_GLOBAL;

  if (name_[0] == '_' && name_[1] == '_')
    flags_ |= CLP_VAR_HIDDEN;
}

void
ClParserVar::
term()
{
}

void
ClParserVar::
print() const
{
  if (isHidden())
    return;

  ClParserInst->output("%s = ", name_.c_str());

  value_->print();

  ClParserInst->output("\n");
}

void
ClParserVar::
print(std::ostream &os) const
{
  if (isHidden())
    return;

  os << name_ << " = ";

  value_->print(os);
}

//--------------------

ClParserVarRef::
ClParserVarRef(ClParserVarPtr variable, const ClParserValueArray &subscripts) :
 variable_(variable), subscripts_(subscripts)
{
}

bool
ClParserVarRef::
getValue(ClParserValuePtr &value) const
{
  value = variable_->getValue();

  uint num_subscripts = uint(subscripts_.size());

  if (num_subscripts == 0)
    return true;

  for (uint i = 0; i < num_subscripts; ++i)
    if (! value->subscriptValue(subscripts_[i], value))
      return false;

  return true;
}

ClParserVarRef *
ClParserVarRef::
dup() const
{
  return new ClParserVarRef(variable_, subscripts_);
}

// TODO: cleanup
bool
ClParserVarRef::
setValue(ClParserValuePtr new_value)
{
  uint num_subscripts = uint(subscripts_.size());

  // a = b

  if (num_subscripts == 0) {
    ClParserValuePtr value1 = new_value;

    if (! value1->convertToType(new_value))
      return false;

    variable_->setValue(value1);

    return true;
  }

  ClParserValuePtr value = variable_->getValue();

  // a = "12345"
  // a[2] = "abc"

  if      (value->isType(CL_PARSER_VALUE_TYPE_STRING)) {
    if (! new_value->isType(CL_PARSER_VALUE_TYPE_STRING))
      return false;

    ClParserStringPtr str = new_value->getString();

    int offset = 0;

    int i1, i2;

    for (uint i = 0; i < num_subscripts; i++) {
      ClParserArrayPtr array = subscripts_[i]->getArray();

      array->getSubscriptRange(&i1, &i2);

      i1 += offset;
      i2 += offset;

      if (! value->getString()->replaceSubString(i1, i2, str))
        return false;

      offset += i1;
    }
  }

  // a = [1,2,3,4,5]
  // a[2] = [4]

  else if (value->isType(CL_PARSER_VALUE_TYPE_ARRAY)) {
    ClParserArrayPtr array1 = value->getArray();

    if (array1->isType(CL_PARSER_VALUE_TYPE_REAL) ||
        array1->isType(CL_PARSER_VALUE_TYPE_INTEGER)) {
      IntVectorT subscripts;

      for (uint i = 0; i < num_subscripts; i++) {
        ClParserArrayPtr array = subscripts_[i]->getArray();

        IntVectorT subscripts1;

        array->toIntegers(subscripts1);

        copy(subscripts1.begin(), subscripts1.end(), back_inserter(subscripts));
      }

      // set value
      if (subscripts.size() == array1->getNumDims()) {
        ClParserValuePtr value1 = new_value;

        if (array1->isType(CL_PARSER_VALUE_TYPE_REAL)) {
          if (! value1->convertToReal())
            return false;
        }
        else {
          if (! value1->convertToInteger())
            return false;
        }

        array1->setSubscriptValue(subscripts, value1);
      }
      // set sub-array
      else {
        if (! new_value->isType(CL_PARSER_VALUE_TYPE_ARRAY))
          return false;

        ClParserArrayPtr array = new_value->getArray();

#if 0
        if (! ClParserValue::checkBinaryTypes(value2, new_value))
          return false;
#endif

        array->setSubscriptValue(subscripts, new_value);
      }
    }
    else {
      for (uint i = 0; i < num_subscripts; i++) {
        ClParserArrayPtr array = subscripts_[i]->getArray();

        IntVectorT subscripts;

        array->toIntegers(subscripts);

        uint num_ssubscripts = uint(subscripts.size());

        if      (array1->isType(CL_PARSER_VALUE_TYPE_STRING)) {
          if (i < num_ssubscripts - 1) {
            ClParserValuePtr value1 = array1->getSubscriptValue(subscripts);

            uint num_subscripts1 = num_ssubscripts - i - 1;

            ClParserValueArray subscripts1;

            for (uint j = 0; j < num_subscripts1; ++j)
              subscripts1.push_back(subscripts_[j + i + 1]);

            ClParserVarRefPtr var_ref1 = ClParserInst->createVarRef(variable_, subscripts1);

            if (! var_ref1->setValue(new_value))
              return false;

            break;
          }
          else {
            ClParserValuePtr value1 = new_value;

            if (! value1->convertToString())
              return false;

            array1->setSubscriptValue(subscripts, value1);
          }
        }
        else if (array1->isType(CL_PARSER_VALUE_TYPE_LIST)) {
          if (i < num_ssubscripts - 1) {
            ClParserValuePtr value1 = array1->getSubscriptValue(subscripts);

            uint num_subscripts1 = num_ssubscripts - i - 1;

            ClParserValueArray subscripts1;

            for (uint j = 0; j < num_subscripts1; ++j)
              subscripts1.push_back(subscripts_[j + i + 1]);

            ClParserVarRefPtr var_ref1 = ClParserInst->createVarRef(variable_, subscripts1);

            if (! var_ref1->setValue(new_value))
              return false;

            break;
          }
          else {
            if (! new_value->isType(CL_PARSER_VALUE_TYPE_LIST))
              return false;

            array1->setSubscriptValue(subscripts, new_value);
          }
        }
        else if (array1->getType() == CL_PARSER_VALUE_TYPE_DICTIONARY) {
          if (i < num_ssubscripts - 1) {
            ClParserValuePtr value1 = array1->getSubscriptValue(subscripts);

            uint num_subscripts1 = num_ssubscripts - i - 1;

            ClParserValueArray subscripts1;

            for (uint j = 0; j < num_subscripts1; ++j)
              subscripts1.push_back(subscripts_[j + i + 1]);

            ClParserVarRefPtr var_ref1 = ClParserInst->createVarRef(variable_, subscripts1);

            if (! var_ref1->setValue(new_value))
              return false;

            break;
          }
          else {
            if (! new_value->isType(CL_PARSER_VALUE_TYPE_DICTIONARY))
              return false;

            array1->setSubscriptValue(subscripts, new_value);
          }
        }
        else if (array1->getType() == CL_PARSER_VALUE_TYPE_STRUCTURE) {
          if (i < num_ssubscripts - 1) {
            ClParserValuePtr value1 = array1->getSubscriptValue(subscripts);

            uint num_subscripts1 = num_ssubscripts - i - 1;

            ClParserValueArray subscripts1;

            for (uint j = 0; j < num_subscripts1; ++j)
              subscripts1.push_back(subscripts_[j + i + 1]);

            ClParserVarRefPtr var_ref1 = ClParserInst->createVarRef(variable_, subscripts1);

            if (! var_ref1->setValue(new_value))
              return false;

            break;
          }
          else {
            if (! new_value->isType(CL_PARSER_VALUE_TYPE_STRUCTURE))
              return false;

            ClParserValuePtr svalue = array1->getSubscriptValue(subscripts);

            if ( new_value->getStructure()->getType() != svalue->getStructure()->getType())
              return false;

            svalue = new_value;
          }
        }
      }
    }
  }

  // a = {1,2,3}
  // a[1] = 5

  else if (value->isType(CL_PARSER_VALUE_TYPE_LIST)) {
    ClParserListPtr list = value->getList();

    ClParserArrayPtr array = subscripts_[0]->getArray();

    if (array->getNumData() > 1)
      return false;

    UIntVectorT subscripts;

    array->toIntegers(subscripts);

    int ind = int(subscripts[0]);

    if (ind < 0)
      ind += list->getNumValues() + 1;

    if (ind < 1 || ind > int(list->getNumValues()))
      return false;

    ClParserValuePtr value1 = list->getValue(ind);

    if (num_subscripts > 1) {
      uint num_subscripts1 = num_subscripts - 1;

      ClParserValueArray subscripts1;

      for (uint i = 0; i < num_subscripts1; ++i)
        subscripts1.push_back(subscripts_[i + 1]);

      ClParserVarRefPtr var_ref1 = ClParserInst->createVarRef(variable_, subscripts1);

      if (! var_ref1->setValue(new_value))
        return false;
    }
    else
      value1 = new_value;
  }
  else if (value->isType(CL_PARSER_VALUE_TYPE_DICTIONARY)) {
    ClParserDictPtr dictionary = value->getDictionary();

    ClParserArrayPtr array = subscripts_[0]->getArray();

    if (array->getNumData() > 1)
      return false;

    ClParserValuePtr ind = array->getValue(1);

    ClParserValuePtr value1 = dictionary->getValue(ind);

    if (! value1)
      return false;

    if (num_subscripts > 1) {
      uint num_subscripts1 = num_subscripts - 1;

      ClParserValueArray subscripts1;

      for (uint i = 0; i < num_subscripts1; ++i)
        subscripts1.push_back(subscripts_[i + 1]);

      ClParserVarRefPtr var_ref1 = ClParserInst->createVarRef(variable_, subscripts1);

      if (! var_ref1->setValue(new_value))
        return false;
    }
    else {
      ClParserValuePtr value2 = new_value;

      if (! value2->convertToType(value1))
        return false;

      value1 = value2;
    }
  }
  else
    return false;

  return true;
}

void
ClParserVarRef::
callVariableProc()
{
  ClParserInst->callVariableProc(variable_);
}

void
ClParserVarRef::
addSubscript(ClParserValuePtr subscript_value)
{
  assert(subscript_value->isType(CL_PARSER_VALUE_TYPE_ARRAY));

  ClParserArrayPtr array = subscript_value->getArray();

  //assert(array->getNumDims() == 1 && array->getNumData() <= 2);

  subscripts_.push_back(subscript_value);
}

void
ClParserVarRef::
print() const
{
  ClParserValuePtr value = variable_->getValue();

  value->print();
}

void
ClParserVarRef::
print(std::ostream &os) const
{
  os << "&";

  variable_->print(os);
}

void
ClParserVarRef::
debugPrint() const
{
  ClParserValuePtr value = variable_->getValue();

  if (variable_->isFixedType())
    fprintf(stderr, "fix_type ");

  if (variable_->isFixedValue())
    fprintf(stderr, "fix_value ");

  fprintf(stderr, "ref %s", variable_->getName().c_str());

  value->debugPrint();

  uint num_subscripts = uint(subscripts_.size());

  if (num_subscripts > 0) {
    for (uint i = 0; i < num_subscripts; i++) {
      if (i > 0) fprintf(stderr, " ");

      subscripts_[i]->debugPrint();
    }
  }
}

//--------------------

ClParserStructVarRef::
ClParserStructVarRef(ClParserVarPtr variable, const std::string &name,
                     const ClParserValueArray &subscripts) :
 ClParserVarRef(variable, subscripts)
{
  ClParserValuePtr value = variable_->getValue();

  if (! value->isType(CL_PARSER_VALUE_TYPE_STRUCTURE))
    ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  names_.push_back(name);
}

ClParserStructVarRef *
ClParserStructVarRef::
dup() const
{
  return new ClParserStructVarRef(variable_, names_.front(), subscripts_);
}

void
ClParserStructVarRef::
addName(const std::string &name)
{
  names_.push_back(name);
}

bool
ClParserStructVarRef::
setValue(ClParserValuePtr value)
{
  ClParserValuePtr svalue = variable_->getValue();

  ClParserStructPtr structure = svalue->getStructure();

  uint num_names = uint(names_.size());

  for (uint i = 0; i < num_names - 1; ++i) {
    const std::string name = names_[i];

    if (! structure->getValue(name, svalue))
      return false;

    if (! svalue->isType(CL_PARSER_VALUE_TYPE_STRUCTURE))
      return false;

    structure = svalue->getStructure();
  }

  const std::string name = names_[num_names - 1];

  return structure->setValue(name, value);
}

bool
ClParserStructVarRef::
getValue(ClParserValuePtr &value) const
{
  ClParserValuePtr svalue = variable_->getValue();

  ClParserStructPtr structure = svalue->getStructure();

  uint num_names = uint(names_.size());

  for (uint i = 0; i < num_names - 1; ++i) {
    const std::string name = names_[i];

    if (! structure->getValue(name, svalue))
      return false;

    if (! svalue->isType(CL_PARSER_VALUE_TYPE_STRUCTURE))
      return false;

    structure = svalue->getStructure();
  }

  const std::string name = names_[num_names - 1];

  if (! structure->getValue(name, value))
    return false;

  uint num_subscripts = uint(subscripts_.size());

  if (num_subscripts == 0)
    return true;

  for (uint i = 0; i < num_subscripts; ++i)
    if (! value->subscriptValue(subscripts_[i], value))
      return false;

  return true;
}

void
ClParserStructVarRef::
print() const
{
  ClParserValuePtr value = variable_->getValue();

  value->print();
}

void
ClParserStructVarRef::
print(std::ostream &os) const
{
  os << "&";

  variable_->print(os);
}

void
ClParserStructVarRef::
debugPrint() const
{
  ClParserValuePtr value = variable_->getValue();

  if (variable_->isFixedType())
    fprintf(stderr, "fix_type ");

  if (variable_->isFixedValue())
    fprintf(stderr, "fix_value ");

  fprintf(stderr, "ref %s", variable_->getName().c_str());

  value->debugPrint();

  uint num_subscripts = uint(subscripts_.size());

  if (num_subscripts > 0) {
    for (uint i = 0; i < num_subscripts; i++) {
      if (i > 0) fprintf(stderr, " ");

      subscripts_[i]->debugPrint();
    }
  }
}
