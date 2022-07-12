#include <CCeilPI.h>

int ClParserDict::error_code_;

ClParserDictMgr *
ClParserDictMgr::
getInstance()
{
  static ClParserDictMgr *instance;

  if (! instance)
    instance = new ClParserDictMgr();

  return instance;
}

ClParserDictMgr::
ClParserDictMgr()
{
}

ClParserDictMgr::
~ClParserDictMgr()
{
}

const ClParserKey &
ClParserDictMgr::
objToKey(const ClParserObj &obj)
{
  if      (obj.getBaseType() == CL_PARSER_VALUE_TYPE_INTEGER) {
    const ClParserInteger &integer = ClParserInteger::castObj(obj);

    return integerToKey(integer.getValue());
  }
  else if (obj.getBaseType() == CL_PARSER_VALUE_TYPE_STRING) {
    const ClParserString &str = ClParserString::castObj(obj);

    return stringToKey(str.getText());
  }
  else
    return integerToKey(0);
}

const ClParserKey &
ClParserDictMgr::
valueToKey(ClParserValuePtr value)
{
  if      (value->getType() == CL_PARSER_VALUE_TYPE_INTEGER)
    return integerToKey(value->getInteger()->getValue());
  else if (value->getType() == CL_PARSER_VALUE_TYPE_STRING) {
    if (value->getString().isValid())
      return stringToKey(value->getString()->getText());
    else
      return stringToKey("");
  }
  else
    return integerToKey(0);
}

const ClParserKey &
ClParserDictMgr::
integerToKey(long integer)
{
  IntegerKeyMap::const_iterator p = integer_key_map_.find(integer);

  if (p != integer_key_map_.end())
    return (*p).second;

  ClParserKey key(integer);

  integer_key_map_[integer] = key;

  return integer_key_map_[integer];
}

const ClParserKey &
ClParserDictMgr::
stringToKey(const std::string &str)
{
  StringKeyMap::const_iterator p = string_key_map_.find(str);

  if (p != string_key_map_.end())
    return (*p).second;

  ClParserKey key(str);

  string_key_map_[str] = key;

  return string_key_map_[str];
}

//----------------------------------

ClParserDictPtr
ClParserDict::
createDict()
{
  ClParserDict *dict = new ClParserDict;

  return ClParserDictPtr(dict);
}

ClParserDictPtr
ClParserDict::
createDict(const ClParserValuePtr *values, uint num_values)
{
  ClParserDict *dict = new ClParserDict(values, num_values);

  return ClParserDictPtr(dict);
}

ClParserDictPtr
ClParserDict::
createDict(const ClParserValueArray &values)
{
  ClParserDict *dict = new ClParserDict(values);

  return ClParserDictPtr(dict);
}

ClParserDictPtr
ClParserDict::
createDict(const ClParserDict &dict)
{
  ClParserDict *pdict = new ClParserDict(dict);

  return ClParserDictPtr(pdict);
}

//---------

ClParserDict::
ClParserDict() :
 ClParserObj(CL_PARSER_VALUE_TYPE_DICTIONARY)
{
}

ClParserDict::
ClParserDict(const ClParserValuePtr *values, uint num_values) :
 ClParserObj(CL_PARSER_VALUE_TYPE_DICTIONARY)
{
  int num_values1 = num_values/2;

  for (int i = 0, j = 0; i < num_values1; i++, j += 2) {
    const ClParserKey &key = valueToKey(values[j]);

    addValue(key, values[j + 1]);
  }
}

ClParserDict::
ClParserDict(const ClParserValueArray &values) :
 ClParserObj(CL_PARSER_VALUE_TYPE_DICTIONARY)
{
  uint num_values = uint(values.size());

  int num_values1 = num_values/2;

  for (int i = 0, j = 0; i < num_values1; i++, j += 2) {
    const ClParserKey &key = valueToKey(values[uint(j)]);

    addValue(key, values[uint(j + 1)]);
  }
}

ClParserDict::
ClParserDict(const ClParserDict &dict) :
 ClParserObj(CL_PARSER_VALUE_TYPE_DICTIONARY)
{
  KeyValueList::const_iterator p1 = dict.key_values_.begin();
  KeyValueList::const_iterator p2 = dict.key_values_.end  ();

  for ( ; p1 != p2; ++p1)
    addValue((*p1).key, (*p1).value);
}

ClParserDict::
~ClParserDict()
{
  clearValues();
}

ClParserDict &
ClParserDict::
operator=(const ClParserDict &dict)
{
  clearValues();

  addValues(dict);

  return *this;
}

void
ClParserDict::
copy(const ClParserObj &obj)
{
  assert(base_type_ == obj.getBaseType());

  const ClParserDict &rhs = castObj(obj);

  *this = rhs;
}

ClParserDict *
ClParserDict::
dup() const
{
  return new ClParserDict(*this);
}

bool
ClParserDict::
toBool() const
{
  return (key_values_.size() != 0);
}

void
ClParserDict::
clearValues()
{
  key_values_.clear();
}

void
ClParserDict::
addValues(ClParserDictPtr dict)
{
  addValues(*dict.cast<ClParserDict>());
}

void
ClParserDict::
addValues(const ClParserDict &dict)
{
  KeyValueList::const_iterator p1 = dict.key_values_.begin();
  KeyValueList::const_iterator p2 = dict.key_values_.end  ();

  for ( ; p1 != p2; ++p1)
    addValue((*p1).key, (*p1).value);
}

void
ClParserDict::
addValue(const ClParserKey &key, ClParserValuePtr value)
{
  ClParserKeyValue keyval(key, value);

  addValue(keyval);
}

void
ClParserDict::
addValue(const ClParserKeyValue &keyval)
{
  key_values_.push_back(keyval);
}

void
ClParserDict::
deleteValues(const ClParserKey &key)
{
  KeyValueList::iterator p1 = key_values_.begin();
  KeyValueList::iterator p2 = key_values_.end  ();

  for ( ; p1 != p2; ++p1) {
    if ((*p1).key == key) {
      key_values_.erase(p1);
      break;
    }
  }
}

uint
ClParserDict::
numValues() const
{
  return uint(key_values_.size());
}

ClParserValuePtr
ClParserDict::
getKeyValue(const ClParserKey &key) const
{
  KeyValueList::const_iterator p1 = key_values_.begin();
  KeyValueList::const_iterator p2 = key_values_.end  ();

  for ( ; p1 != p2; ++p1)
    if ((*p1).key == key)
      return (*p1).value;

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
getIndValue(uint ind) const
{
  if (ind >= key_values_.size())
    return ClParserValuePtr();

  KeyValueList::const_iterator p1 = key_values_.begin();
  KeyValueList::const_iterator p2 = key_values_.end  ();

  for (uint i = 0; p1 != p2; ++p1, ++i)
    if (i == ind)
      return (*p1).value;

  return ClParserValuePtr();
}

bool
ClParserDict::
getIndKeyValue(uint ind, ClParserKey &key, ClParserValuePtr &value) const
{
  if (ind >= key_values_.size())
    return false;

  KeyValueList::const_iterator p1 = key_values_.begin();
  KeyValueList::const_iterator p2 = key_values_.end  ();

  for (uint i = 0; p1 != p2; ++p1, ++i) {
    if (i == ind) {
      key   = (*p1).key;
      value = (*p1).value;

      return true;
    }
  }

  return false;
}

int
ClParserDict::
cmp(const ClParserObj &obj) const
{
  if (base_type_ != obj.getBaseType())
    return CMathGen::sign(long(base_type_ - obj.getBaseType()));

  const ClParserDict &rhs = castObj(obj);

  if (numValues() != rhs.numValues())
    return int(numValues() - rhs.numValues());

  KeyValueList::const_iterator p1b = key_values_.begin();
  KeyValueList::const_iterator p1e = key_values_.end  ();

  for ( ; p1b != p1e; ++p1b) {
    KeyValueList::const_iterator p2b = rhs.key_values_.begin();
    KeyValueList::const_iterator p2e = rhs.key_values_.end  ();

    for ( ; p2b != p2e; ++p2b)
      if ((*p1b).key == (*p2b).key)
        break;

    if (p2b == p2e)
      return 1;

    int cmp = (*p1b).value->cmp((*p2b).value);

    if (cmp != 0)
      return cmp;
  }

  return 0;
}

uint
ClParserDict::
countKeys(const ClParserKey &key) const
{
  uint count = 0;

  KeyValueList::const_iterator p1 = key_values_.begin();
  KeyValueList::const_iterator p2 = key_values_.end  ();

  for ( ; p1 != p2; ++p1)
    if ((*p1).key == key)
      ++count;

  return count;
}

ClParserValuePtr
ClParserDict::
getMaxValue() const
{
  ClParserValuePtr value;

  KeyValueList::const_iterator p1 = key_values_.begin();
  KeyValueList::const_iterator p2 = key_values_.end  ();

  for ( ; p1 != p2; ++p1)
    if (! value.isValid() || (*p1).value->cmp(value) > 0)
      value = (*p1).value;

  return value;
}

ClParserValuePtr
ClParserDict::
getMinValue() const
{
  ClParserValuePtr value;

  KeyValueList::const_iterator p1 = key_values_.begin();
  KeyValueList::const_iterator p2 = key_values_.end  ();

  for ( ; p1 != p2; ++p1)
    if (! value.isValid() || (*p1).value->cmp(value) < 0)
      value = (*p1).value;

  return value;
}

const ClParserKey &
ClParserDict::
objToKey(const ClParserObj &obj) const
{
  return ClParserDictMgrInst->objToKey(obj);
}

const ClParserKey &
ClParserDict::
valueToKey(ClParserValuePtr value) const
{
  return ClParserDictMgrInst->valueToKey(value);
}

const ClParserKey &
ClParserDict::
integerToKey(long integer) const
{
  return ClParserDictMgrInst->integerToKey(integer);
}

const ClParserKey &
ClParserDict::
stringToKey(const std::string &str) const
{
  return ClParserDictMgrInst->stringToKey(str);
}

//------

std::string
ClParserDict::
asString() const
{
  KeyValueList::const_iterator p1 = key_values_.begin();
  KeyValueList::const_iterator p2 = key_values_.end  ();

  std::string str = "{{";

  for (int i = 0; p1 != p2; ++p1, ++i) {
    if (i > 0) str += ",";

    str += (*p1).key.asString();

    str += ",";

    str += (*p1).value->asString();
  }

  str += "}}";

  return str;
}

void
ClParserDict::
print() const
{
  ClParserInst->output("%s", asString().c_str());
}

void
ClParserDict::
print(std::ostream &os) const
{
  os << asString();
}

void
ClParserDict::
debugPrint() const
{
  fprintf(stderr, "%s", asString().c_str());
}

//-----------------

// Inline Ops

const ClParserObj &
ClParserDict::
increment()
{
  ClErrThrow(ClErr::INVALID_OPERATOR);

  return *this;
}

const ClParserObj &
ClParserDict::
decrement()
{
  ClErrThrow(ClErr::INVALID_OPERATOR);

  return *this;
}

//-----------------

// Unary Ops

ClParserValuePtr
ClParserDict::
unaryPlus() const
{
  ClErrThrow(ClErr::INVALID_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
unaryMinus() const
{
  ClErrThrow(ClErr::INVALID_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
bitNot() const
{
  ClErrThrow(ClErr::INVALID_OPERATOR);

  return ClParserValuePtr();
}

//------

// Binary Ops

ClParserValuePtr
ClParserDict::
plus(const ClParserObj &obj) const
{
  if (obj.getBaseType() != CL_PARSER_VALUE_TYPE_DICTIONARY)
    ClErrThrow(ClErr::INVALID_TYPE_MIX);

  const ClParserDict &dict = castObj(obj);

  ClParserDictPtr dict1 = dupDict();

  dict1->addValues(dict);

  return ClParserValueMgrInst->createValue(dict1);
}

ClParserValuePtr
ClParserDict::
minus(const ClParserObj &obj) const
{
  const ClParserKey &key = objToKey(obj);

  ClParserDictPtr dict1 = dupDict();

  dict1->deleteValues(key);

  return ClParserValueMgrInst->createValue(dict1);
}

ClParserValuePtr
ClParserDict::
times(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
divide(const ClParserObj &obj) const
{
  const ClParserKey &key = objToKey(obj);

  return ClParserValueMgrInst->createValue(long(countKeys(key)));
}

ClParserValuePtr
ClParserDict::
modulus(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
power(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
approxEqual(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
bitAnd(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
bitOr(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
bitXor(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
bitLShift(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
bitRShift(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

//------

// Internal Fns

ClParserValuePtr
ClParserDict::
abs() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
ceil() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
floor() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
sign() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
sqr() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
sqrt() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
cos() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
sin() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
tan() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
acos() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
asin() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
atan() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
atan(double) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
exp() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
log() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
log10() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
cosh() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
sinh() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
tanh() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
norm() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
invnorm() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
toChar() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
toInt() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
toReal() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
toString() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
isNan() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
toLower() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
toUpper() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
min() const
{
  return getMinValue();
}

ClParserValuePtr
ClParserDict::
max() const
{
  return getMaxValue();
}

ClParserValuePtr
ClParserDict::
sum() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
dim() const
{
  return ClParserValueMgrInst->createValue(long(numValues()));
}

ClParserValuePtr
ClParserDict::
len() const
{
  return ClParserValueMgrInst->createValue(long(numValues()));
}

ClParserValuePtr
ClParserDict::
index(const ClParserObj &obj) const
{
  const ClParserKey &key = objToKey(obj);

  return getValue(key);
}

ClParserValuePtr
ClParserDict::
rindex(const ClParserObj &obj) const
{
  const ClParserKey &key = objToKey(obj);

  return getValue(key);
}

ClParserValuePtr
ClParserDict::
sort(ClParserSortDirection) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserDict::
doAssert() const
{
  assert(false);

  return ClParserValuePtr();
}

//-----------------

ClParserValuePtr
ClParserDict::
subscriptValue(const ClParserValuePtr *subscripts, uint num_subscripts) const
{
  error_code_ = 0;

  if (num_subscripts != 1) {
    error_code_ = int(ClErr::TOO_MANY_SUBSCRIPTS);
    return ClParserValuePtr();
  }

  if (subscripts[0]->getType() != CL_PARSER_VALUE_TYPE_ARRAY ||
      subscripts[0]->getArray()->getNumDims() != 1 ||
      subscripts[0]->getArray()->getNumData() != 1) {
    error_code_ = int(ClErr::INVALID_TYPE_FOR_SUBSCRIPT);
    return ClParserValuePtr();
  }

  ClParserValuePtr value = subscripts[0]->getArray()->getValue(1);

  const ClParserKey &key = valueToKey(value);

  ClParserValuePtr sub_value = getKeyValue(key);

  if (! sub_value.isValid()) {
    error_code_ = int(ClErr::SUBSCRIPT_OUT_OF_RANGE);
    return ClParserValuePtr();
  }

  return sub_value;
}

ClParserValuePtr
ClParserDict::
getValue(ClParserValuePtr ind) const
{
  const ClParserKey &key = valueToKey(ind);

  return getValue(key);
}

ClParserValuePtr
ClParserDict::
getValue(const ClParserKey &key) const
{
  error_code_ = 0;

  ClParserValuePtr sub_value = getKeyValue(key);

  if (! sub_value.isValid()) {
    error_code_ = int(ClErr::SUBSCRIPT_OUT_OF_RANGE);
    return ClParserValuePtr();
  }

  return sub_value;
}

ClParserValuePtr
ClParserDict::
getKeyValue(int ind) const
{
  if (ind < 0 || ind >= int(numValues()))
    return ClParserValuePtr();

  ClParserKey      key;
  ClParserValuePtr value;

  if (! getIndKeyValue(uint(ind), key, value))
    return ClParserValuePtr();

  ClParserValuePtr key_value = key.getAsValue();

  ClParserValuePtr lvalue = ClParserValueMgrInst->createValue(CL_PARSER_VALUE_TYPE_LIST);

  lvalue->getList()->addValue(key_value);
  lvalue->getList()->addValue(value);

  return lvalue;
}

ClParserDictPtr
ClParserDict::
dupDict() const
{
  ClParserDict *dict = dup();

  return ClParserDictPtr(dict);
}

//-------------------

ClParserKey::
ClParserKey(long integer) :
 type_(CL_PARSER_VALUE_TYPE_INTEGER)
{
  value_ = new ClParserIntegerKeyValue(integer);
}

ClParserKey::
ClParserKey(const std::string &str) :
 type_(CL_PARSER_VALUE_TYPE_STRING)
{
  value_ = new ClParserStringKeyValue(str);
}

ClParserKey::
ClParserKey(const ClParserKey &key) :
 type_(key.type_)
{
  if (type_ == CL_PARSER_VALUE_TYPE_STRING) {
    auto *svalue = reinterpret_cast<ClParserStringKeyValue *>(key.value_);

    value_ = new ClParserStringKeyValue(*svalue);
  }
  else {
    auto *ivalue = reinterpret_cast<ClParserIntegerKeyValue *>(key.value_);

    value_ = new ClParserIntegerKeyValue(*ivalue);
  }
}

ClParserKey::
~ClParserKey()
{
  delete value_;
}

ClParserKey &
ClParserKey::
operator=(const ClParserKey &key)
{
  if (type_ == key.type_) {
    if (type_ == CL_PARSER_VALUE_TYPE_STRING) {
      auto *lvalue = reinterpret_cast<ClParserStringKeyValue *>(value_);
      auto *rvalue = reinterpret_cast<ClParserStringKeyValue *>(key.value_);

      *lvalue = *rvalue;
    }
    else {
      auto *lvalue = reinterpret_cast<ClParserIntegerKeyValue *>(value_);
      auto *rvalue = reinterpret_cast<ClParserIntegerKeyValue *>(key.value_);

      *lvalue = *rvalue;
    }
  }
  else {
    delete value_;

    type_ = key.type_;

    if (type_ == CL_PARSER_VALUE_TYPE_STRING) {
      auto *svalue = reinterpret_cast<ClParserStringKeyValue *>(key.value_);

      value_ = new ClParserStringKeyValue(*svalue);
    }
    else {
      auto *ivalue = reinterpret_cast<ClParserIntegerKeyValue *>(key.value_);

      value_ = new ClParserIntegerKeyValue(*ivalue);
    }
  }

  return *this;
}

bool
ClParserKey::
operator==(const ClParserKey &key) const
{
  if (type_ != key.type_) return false;

  if (type_ == CL_PARSER_VALUE_TYPE_STRING) {
    auto *lvalue = reinterpret_cast<ClParserStringKeyValue *>(value_);
    auto *rvalue = reinterpret_cast<ClParserStringKeyValue *>(key.value_);

    return (*lvalue == *rvalue);
  }
  else {
    auto *lvalue = reinterpret_cast<ClParserIntegerKeyValue *>(value_);
    auto *rvalue = reinterpret_cast<ClParserIntegerKeyValue *>(key.value_);

    return (*lvalue == *rvalue);
  }
}

//------

std::string
ClParserKey::
asString() const
{
  return value_->toString();
}

void
ClParserKey::
print() const
{
  ClParserInst->output("%s", asString().c_str());
}

void
ClParserKey::
print(std::ostream &os) const
{
  os << asString();
}

void
ClParserKey::
debugPrint() const
{
  fprintf(stderr, "%s", asString().c_str());
}

//------

ClParserValuePtr
ClParserKey::
getAsValue() const
{
  return value_->toValue();
}
