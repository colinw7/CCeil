#include <CCeilPI.h>
#include <COSNaN.h>

#include <cassert>

using std::string;
using std::ostream;

ClParserVarMgr::
ClParserVarMgr()
{
  var_map_ = new VarMap;

  addStandardVariables();
}

ClParserVarMgr::
~ClParserVarMgr()
{
  delete var_map_;

  VarMapStack::const_iterator p1 = var_map_stack_.begin();
  VarMapStack::const_iterator p2 = var_map_stack_.end  ();

  for ( ; p1 != p2; ++p1)
    delete *p1;
}

ClParserVarPtr
ClParserVarMgr::
createStdVar(const string &name, ClParserValuePtr value)
{
  ClParserVarPtr variable = createVar(name, value);

  variable->setFixedValue();
  variable->setNoDelete();

  return variable;
}

ClParserVarPtr
ClParserVarMgr::
createVar(const string &name, ClParserValuePtr value)
{
  assert(! isVariable(name));

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
createStructVarRef(ClParserVarPtr variable, const string &name,
                   const ClParserValueArray &subscripts)
{
  ClParserStructVarRef *svar_ref =
    new ClParserStructVarRef(variable, name, subscripts);

  return ClParserStructVarRefPtr(svar_ref);
}

void
ClParserVarMgr::
newVariableList()
{
  var_map_stack_.push_back(var_map_);

  var_map_ = new VarMap;
}

void
ClParserVarMgr::
oldVariableList()
{
  if (! var_map_stack_.empty()) {
    delete var_map_;

    var_map_ = var_map_stack_.back();

    var_map_stack_.pop_back();
  }
}

void
ClParserVarMgr::
deleteAllVariables()
{
  var_map_->clear();

  if (var_map_stack_.empty())
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

  createStdVar("__INTEGER",
               ClParserValueMgrInst->createValue
                ((long) CL_PARSER_VALUE_TYPE_INTEGER));
  createStdVar("__REAL",
               ClParserValueMgrInst->createValue
                ((long) CL_PARSER_VALUE_TYPE_REAL));
  createStdVar("__STRING",
               ClParserValueMgrInst->createValue
                ((long) CL_PARSER_VALUE_TYPE_STRING));
  createStdVar("__ARRAY",
               ClParserValueMgrInst->createValue
                ((long) CL_PARSER_VALUE_TYPE_ARRAY));
  createStdVar("__LIST",
               ClParserValueMgrInst->createValue
                ((long) CL_PARSER_VALUE_TYPE_LIST));
  createStdVar("__DICTIONARY",
               ClParserValueMgrInst->createValue
                ((long) CL_PARSER_VALUE_TYPE_DICTIONARY));
  createStdVar("__STRUCTURE",
               ClParserValueMgrInst->createValue
                ((long) CL_PARSER_VALUE_TYPE_STRUCTURE));
}

ClParserVarPtr
ClParserVarMgr::
addVariable(ClParserVar *var)
{
  VarMap *var_map = var_map_;

  if (var->isGlobal() && ! var_map_stack_.empty())
    var_map = var_map_stack_[0];

  const string &name = var->getName();

  (*var_map)[name] = var;

  return (*var_map)[name];
}

void
ClParserVarMgr::
removeVariable(const string &name)
{
  VarMap *var_map = var_map_;

  VarMap::iterator p = var_map->find(name);

  if (p != var_map_->end()) {
    var_map->erase(p);

    return;
  }

  int num = var_map_stack_.size();

  for (int i = num - 1; i >= 0; --i) {
    var_map = var_map_stack_[i];

    p = var_map->find(name);

    if (p != var_map->end()) {
      var_map->erase(p);

      return;
    }
  }
}

bool
ClParserVarMgr::
isVariable(const string &name) const
{
  ClParserVarPtr var;

  return getVariableI(name, var);
}

ClParserVarPtr
ClParserVarMgr::
getVariable(const string &name, bool create) const
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
getVariableI(const string &name, ClParserVarPtr &var) const
{
  if (name == "")
    return false;

  if (! ClParserVar::isValidName(name))
    return false;

  VarMap::iterator p = var_map_->find(name);

  if (p != var_map_->end()) {
    var = (*p).second;

    return true;
  }

  bool found = false;

  int num = var_map_stack_.size();

  for (int i = num - 1; i >= 0; i--) {
    VarMap *var_map_ = var_map_stack_[i];

    VarMap::iterator p = var_map_->find(name);

    if (p != var_map_->end()) {
      var = (*p).second;

      found = true;

      break;
    }
  }

  if (! found)
    return false;

  if (var->getValue().isValid() && ! var->isGlobal()) {
    ClParserVarMgr *th = const_cast<ClParserVarMgr *>(this);

    var = th->createVar(var->getName(), var->getValue());
  }

  return true;
}

ClParserValuePtr
ClParserVarMgr::
getVariableValue(const string &name) const
{
  ClParserVarPtr var;

  if (! getVariableI(name, var) || ! var->getValue().isValid())
    return ClParserValuePtr();

  return var->getValue();
}

void
ClParserVarMgr::
setVariableValue(const string &name, ClParserValuePtr value)
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
  VarMap::iterator p1 = var_map_->begin();
  VarMap::iterator p2 = var_map_->end  ();

  for ( ; p1 != p2; ++p1)
    (*p1).second->print();

  int num = var_map_stack_.size();

  for (int i = num - 1; i >= 0; i--) {
    ClParserInst->output("\n");

    VarMap::iterator p1 = var_map_stack_[i]->begin();
    VarMap::iterator p2 = var_map_stack_[i]->end  ();

    for ( ; p1 != p2; ++p1)
      (*p1).second->print();
  }
}

void
ClParserVarMgr::
addVariableProc(const string &name, ClParserVarMgr::VarProc proc, void *data)
{
  if (proc == NULL)
    return;

  VarProcData *proc_data = new VarProcData(name, proc, data);

  var_proc_data_list_.push_back(proc_data);
}

void
ClParserVarMgr::
deleteVariableProc(const string &name, ClParserVarMgr::VarProc proc, void *data)
{
  VarProcDataList::const_iterator p1 = var_proc_data_list_.begin();
  VarProcDataList::const_iterator p2 = var_proc_data_list_.end  ();

  for ( ; p1 != p2; ++ p1) {
    VarProcData *proc_data = *p1;

    if (proc_data->name == name &&
        proc_data->proc == proc &&
        proc_data->data == data) {
      var_proc_data_list_.remove(proc_data);

      delete proc_data;

      return;
    }
  }
}

void
ClParserVarMgr::
callVariableProc(ClParserVarPtr variable)
{
  VarProcDataList::const_iterator p1 = var_proc_data_list_.begin();
  VarProcDataList::const_iterator p2 = var_proc_data_list_.end  ();

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
isValidName(const string &name)
{
  return CStrUtil::isIdentifier(name);
}

ClParserVar::
ClParserVar(const string &name) :
 name_(name), value_(ClParserValueMgrInst->createValue(0L)), flags_(0)
{
  init();
}

ClParserVar::
ClParserVar(const string &name, ClParserValuePtr value) :
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
      ! ClParserInst->isValidNewName(CL_PARSER_NAME_TYPE_VARIABLE, name_))
    CITHROW(CLERR_INVALID_VARIABLE_NAME);

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
print(ostream &os) const
{
  if (isHidden())
    return;

  CStrUtil::fprintf(os, "%s = ", name_.c_str());

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

  uint num_subscripts = subscripts_.size();

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
  uint num_subscripts = subscripts_.size();

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

        uint num_subscripts = subscripts.size();

        if      (array1->isType(CL_PARSER_VALUE_TYPE_STRING)) {
          if (i < num_subscripts - 1) {
            ClParserValuePtr value1 = array1->getSubscriptValue(subscripts);

            uint num_subscripts1 = num_subscripts - i - 1;

            ClParserValueArray subscripts1;

            for (uint j = 0; j < num_subscripts1; ++j)
              subscripts1.push_back(subscripts_[j + i + 1]);

            ClParserVarRefPtr var_ref1 =
              ClParserInst->createVarRef(variable_, subscripts1);

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
          if (i < num_subscripts - 1) {
            ClParserValuePtr value1 = array1->getSubscriptValue(subscripts);

            uint num_subscripts1 = num_subscripts - i - 1;

            ClParserValueArray subscripts1;

            for (uint j = 0; j < num_subscripts1; ++j)
              subscripts1.push_back(subscripts_[j + i + 1]);

            ClParserVarRefPtr var_ref1 =
              ClParserInst->createVarRef(variable_, subscripts1);

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
          if (i < num_subscripts - 1) {
            ClParserValuePtr value1 = array1->getSubscriptValue(subscripts);

            uint num_subscripts1 = num_subscripts - i - 1;

            ClParserValueArray subscripts1;

            for (uint j = 0; j < num_subscripts1; ++j)
              subscripts1.push_back(subscripts_[j + i + 1]);

            ClParserVarRefPtr var_ref1 =
              ClParserInst->createVarRef(variable_, subscripts1);

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
          if (i < num_subscripts - 1) {
            ClParserValuePtr value1 = array1->getSubscriptValue(subscripts);

            uint num_subscripts1 = num_subscripts - i - 1;

            ClParserValueArray subscripts1;

            for (uint j = 0; j < num_subscripts1; ++j)
              subscripts1.push_back(subscripts_[j + i + 1]);

            ClParserVarRefPtr var_ref1 =
              ClParserInst->createVarRef(variable_, subscripts1);

            if (! var_ref1->setValue(new_value))
              return false;

            break;
          }
          else {
            if (! new_value->isType(CL_PARSER_VALUE_TYPE_STRUCTURE))
              return false;

            ClParserValuePtr svalue = array1->getSubscriptValue(subscripts);

            if ( new_value->getStructure()->getType() !=
                svalue->getStructure()->getType())
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

    int ind = subscripts[0];

    if (ind < 0)
      ind += list->getNumValues() + 1;

    if (ind < 1 || ind > (int) list->getNumValues())
      return false;

    ClParserValuePtr value1 = list->getValue(ind);

    if (num_subscripts > 1) {
      uint num_subscripts1 = num_subscripts - 1;

      ClParserValueArray subscripts1;

      for (uint i = 0; i < num_subscripts1; ++i)
        subscripts1.push_back(subscripts_[i + 1]);

      ClParserVarRefPtr var_ref1 =
        ClParserInst->createVarRef(variable_, subscripts1);

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

    if (! value1.isValid())
      return false;

    if (num_subscripts > 1) {
      uint num_subscripts1 = num_subscripts - 1;

      ClParserValueArray subscripts1;

      for (uint i = 0; i < num_subscripts1; ++i)
        subscripts1.push_back(subscripts_[i + 1]);

      ClParserVarRefPtr var_ref1 =
        ClParserInst->createVarRef(variable_, subscripts1);

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
print(ostream &os) const
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

  uint num_subscripts = subscripts_.size();

  if (num_subscripts > 0) {
    for (uint i = 0; i < num_subscripts; i++) {
      if (i > 0) fprintf(stderr, " ");

      subscripts_[i]->debugPrint();
    }
  }
}

//--------------------

ClParserStructVarRef::
ClParserStructVarRef(ClParserVarPtr variable, const string &name,
                     const ClParserValueArray &subscripts) :
 ClParserVarRef(variable, subscripts)
{
  ClParserValuePtr value = variable_->getValue();

  if (! value->isType(CL_PARSER_VALUE_TYPE_STRUCTURE))
    CITHROW(CLERR_INVALID_TYPE_FOR_OPERATOR);

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
addName(const string &name)
{
  names_.push_back(name);
}

bool
ClParserStructVarRef::
setValue(ClParserValuePtr value)
{
  ClParserValuePtr svalue = variable_->getValue();

  ClParserStructPtr structure = svalue->getStructure();

  uint num_names = names_.size();

  for (uint i = 0; i < num_names - 1; ++i) {
    const string name = names_[i];

    if (! structure->getValue(name, svalue))
      return false;

    if (! svalue->isType(CL_PARSER_VALUE_TYPE_STRUCTURE))
      return false;

    structure = svalue->getStructure();
  }

  const string name = names_[num_names - 1];

  return structure->setValue(name, value);
}

bool
ClParserStructVarRef::
getValue(ClParserValuePtr &value) const
{
  ClParserValuePtr svalue = variable_->getValue();

  ClParserStructPtr structure = svalue->getStructure();

  uint num_names = names_.size();

  for (uint i = 0; i < num_names - 1; ++i) {
    const string name = names_[i];

    if (! structure->getValue(name, svalue))
      return false;

    if (! svalue->isType(CL_PARSER_VALUE_TYPE_STRUCTURE))
      return false;

    structure = svalue->getStructure();
  }

  const string name = names_[num_names - 1];

  if (! structure->getValue(name, value))
    return false;

  uint num_subscripts = subscripts_.size();

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
print(ostream &os) const
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

  uint num_subscripts = subscripts_.size();

  if (num_subscripts > 0) {
    for (uint i = 0; i < num_subscripts; i++) {
      if (i > 0) fprintf(stderr, " ");

      subscripts_[i]->debugPrint();
    }
  }
}
