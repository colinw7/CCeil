#include <CCeilPI.h>

int ClParserList::error_code_ = 0;

ClParserListPtr
ClParserList::
createList()
{
  ClParserList *list = new ClParserList;

  return ClParserListPtr(list);
}

ClParserListPtr
ClParserList::
createList(const ClParserValuePtr *values, uint num_values)
{
  ClParserList *list = new ClParserList(values, num_values);

  return ClParserListPtr(list);
}

ClParserListPtr
ClParserList::
createList(const ClParserValueArray &values)
{
  ClParserList *list = new ClParserList(values);

  return ClParserListPtr(list);
}

ClParserListPtr
ClParserList::
createList(const ClParserList &list)
{
  ClParserList *plist = new ClParserList(list);

  return ClParserListPtr(plist);
}

//------

ClParserList::
ClParserList() :
 ClParserObj(CL_PARSER_VALUE_TYPE_LIST)
{
}

ClParserList::
ClParserList(const ClParserValuePtr *values, uint num_values) :
 ClParserObj(CL_PARSER_VALUE_TYPE_LIST)
{
  for (uint i = 0; i < num_values; ++i)
    addValue(values[i]);
}

ClParserList::
ClParserList(const ClParserValueArray &values)  :
 ClParserObj(CL_PARSER_VALUE_TYPE_LIST)
{
  uint num_values = uint(values.size());

  for (uint i = 0; i < num_values; ++i)
    addValue(values[i]);
}

ClParserList::
ClParserList(const ClParserList &list) :
 ClParserObj(CL_PARSER_VALUE_TYPE_LIST)
{
  addListValues(list);
}

ClParserList::
~ClParserList()
{
  deleteAllValues();
}

ClParserList &
ClParserList::
operator=(const ClParserList &list)
{
  deleteAllValues();

  addListValues(list);

  return *this;
}

void
ClParserList::
copy(const ClParserObj &obj)
{
  assert(base_type_ == obj.getBaseType());

  const ClParserList &rhs = castObj(obj);

  *this = rhs;
}

ClParserList *
ClParserList::
dup() const
{
  return new ClParserList(*this);
}

ClParserListPtr
ClParserList::
dupList() const
{
  ClParserList *list = dup();

  return ClParserListPtr(list);
}

uint
ClParserList::
getNumValues() const
{
  return uint(values_.size());
}

void
ClParserList::
addValue(ClParserValuePtr value)
{
  values_.push_back(value);
}

void
ClParserList::
addListValues(ClParserListPtr list)
{
  addListValues(*dynamic_cast<ClParserList *>(list.get()));
}

void
ClParserList::
addListValues(const ClParserList &list)
{
  if (this != &list) {
    auto p1 = list.values_.begin();
    auto p2 = list.values_.end  ();

    for ( ; p1 != p2; ++p1)
      addValue(*p1);
  }
  else {
    ClParserValueList values = list.values_;

    auto p1 = values.begin();
    auto p2 = values.end  ();

    for ( ; p1 != p2; ++p1)
      addValue(*p1);
  }
}

bool
ClParserList::
deleteValue(ClParserValuePtr value)
{
  auto p1 = values_.begin();
  auto p2 = values_.end  ();

  for ( ; p1 != p2; ++p1) {
    if ((*p1)->cmp(value) == 0) {
      values_.erase(p1);

      return true;
    }
  }

  return false;
}

bool
ClParserList::
deleteListValues(ClParserListPtr list)
{
  return deleteListValues(*dynamic_cast<ClParserList *>(list.get()));
}

bool
ClParserList::
deleteListValues(const ClParserList &list)
{
  auto p1 = list.values_.begin();
  auto p2 = list.values_.end  ();

  for ( ; p1 != p2; ++p1)
    deleteValue(*p1);

  return true;
}

void
ClParserList::
deleteAllValues()
{
  values_.clear();
}

ClParserValuePtr
ClParserList::
getValue(int ind) const
{
  uint num_values = getNumValues();

  if (ind < 0)
    ind += num_values + 1;

  assert(ind >= 1 && ind <= int(num_values));

  auto p1 = values_.begin();
  auto p2 = values_.end  ();

  for (int i = 1; p1 != p2; ++p1, ++i)
    if (i == ind)
      return *p1;

  return *p1;
}

int
ClParserList::
getValuePos(ClParserValuePtr value) const
{
  uint pos = 0;

  auto p1 = values_.begin();
  auto p2 = values_.end  ();

  for ( ; p1 != p2; ++p1, pos++)
    if ((*p1)->cmp(value) == 0)
      return int(pos);

  return -1;
}

int
ClParserList::
getRValuePos(ClParserValuePtr value) const
{
  uint pos = 0;

  auto p1 = values_.rbegin();
  auto p2 = values_.rend  ();

  for ( ; p1 != p2; ++p1, pos++)
    if ((*p1)->cmp(value) == 0)
      return int(pos);

  return -1;
}

void
ClParserList::
sortValues(ClParserSortDirection direction)
{
  values_.sort(ClParserValueCmp(direction));
}

uint
ClParserList::
countValues(ClParserValuePtr value) const
{
  uint count = 0;

  auto p1 = values_.begin();
  auto p2 = values_.end  ();

  for ( ; p1 != p2; ++p1)
    if ((*p1)->cmp(value) == 0)
      ++count;

  return count;
}

void
ClParserList::
roll(int num)
{
  if (num > 0) {
    for (int i = 0; i < num; ++i) {
      ClParserValuePtr value = values_.front();

      values_.pop_front();

      values_.push_back(value);
    }
  }
  else {
    for (int i = 0; i < -num; ++i) {
      ClParserValuePtr value = values_.back();

      values_.pop_back();

      values_.push_front(value);
    }
  }
}

ClParserValuePtr
ClParserList::
getMaxValue() const
{
  ClParserValuePtr value;

  auto p1 = values_.begin();
  auto p2 = values_.end  ();

  for ( ; p1 != p2; ++p1)
    if (! value || (*p1)->cmp(value) > 0)
      value = *p1;

  return value;
}

ClParserValuePtr
ClParserList::
getMinValue() const
{
  ClParserValuePtr value;

  auto p1 = values_.begin();
  auto p2 = values_.end  ();

  for ( ; p1 != p2; ++p1)
    if (! value || (*p1)->cmp(value) < 0)
      value = *p1;

  return value;
}

//------

std::string
ClParserList::
asString() const
{
  auto p1 = values_.begin();
  auto p2 = values_.end  ();

  std::string str = "{";

  for (int i = 0 ; p1 != p2; ++p1, ++i) {
    if (i > 0) ClParserInst->output(",");

    str += (*p1)->asString();
  }

  str += "}";

  return str;
}

void
ClParserList::
print() const
{
  ClParserInst->output("%s", asString().c_str());
}

void
ClParserList::
print(std::ostream &os) const
{
  os << asString();
}

void
ClParserList::
debugPrint() const
{
  fprintf(stderr, "%s", asString().c_str());
}

//------

int
ClParserList::
cmp(const ClParserObj &obj) const
{
  if (base_type_ != obj.getBaseType())
    return CMathGen::sign(long(base_type_ - obj.getBaseType()));

  const ClParserList &rhs = castObj(obj);

  uint num_values1 = uint(    values_.size());
  uint num_values2 = uint(rhs.values_.size());

  if (num_values1 != num_values2)
    return int(num_values1 - num_values2);

  auto p1 = values_.begin();
  auto p2 = rhs.values_.begin();
  auto p3 = values_.end  ();

  int cmp;

  for ( ; p1 != p3; ++p1, ++p2) {
    cmp = (*p1)->cmp(*p2);

    if (cmp != 0)
      return cmp;
  }

  return 0;
}

//-----------------

// Inline Ops

const ClParserObj &
ClParserList::
increment()
{
  ClErrThrow(ClErr::INVALID_OPERATOR);

  return *this;
}

const ClParserObj &
ClParserList::
decrement()
{
  ClErrThrow(ClErr::INVALID_OPERATOR);

  return *this;
}

//-----------------

// Unary Ops

ClParserValuePtr
ClParserList::
unaryPlus() const
{
  ClParserListPtr list = createList();

  auto p1 = values_.begin();
  auto p2 = values_.end  ();

  for ( ; p1 != p2; ++p1)
    list->addValue((*p1)->unaryPlus());

  return ClParserValueMgrInst->createValue(list);
}

ClParserValuePtr
ClParserList::
unaryMinus() const
{
  ClParserListPtr list = createList();

  auto p1 = values_.begin();
  auto p2 = values_.end  ();

  for ( ; p1 != p2; ++p1)
    list->addValue((*p1)->unaryMinus());

  return ClParserValueMgrInst->createValue(list);
}

ClParserValuePtr
ClParserList::
bitNot() const
{
  ClParserListPtr list = createList();

  auto p1 = values_.begin();
  auto p2 = values_.end  ();

  for ( ; p1 != p2; ++p1)
    list->addValue((*p1)->bitNot());

  return ClParserValueMgrInst->createValue(list);
}

//-----------------

// Binary Ops

ClParserValuePtr
ClParserList::
plus(const ClParserObj &obj) const
{
  ClParserListPtr list = dupList();

  if (obj.getBaseType() == CL_PARSER_VALUE_TYPE_LIST) {
    const ClParserList &list1 = castObj(obj);

    list->addListValues(list1);
  }
  else {
    ClParserValuePtr value = ClParserValueMgrInst->createValue(obj);

    list->addValue(value);
  }

  return ClParserValueMgrInst->createValue(list);
}

ClParserValuePtr
ClParserList::
minus(const ClParserObj &obj) const
{
  ClParserListPtr list = dupList();

  if (obj.getBaseType() == CL_PARSER_VALUE_TYPE_LIST) {
    const ClParserList &list1 = castObj(obj);

    list->deleteListValues(list1);
  }
  else {
    ClParserValuePtr value = ClParserValueMgrInst->createValue(obj);

    list->deleteValue(value);
  }

  return ClParserValueMgrInst->createValue(list);
}

ClParserValuePtr
ClParserList::
times(const ClParserObj &obj) const
{
  if (obj.getBaseType() != CL_PARSER_VALUE_TYPE_INTEGER)
    ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  const ClParserInteger &integer = ClParserInteger::castObj(obj);

  long num = integer.getValue();

  if (num < 0)
    ClErrThrow(ClErr::VALUE_OUT_OF_RANGE);

  ClParserListPtr list = dupList();

  if (num == 0)
    list->deleteAllValues();
  else {
    for (long i = 1; i < num; ++i)
      list->addListValues(*this);
  }

  return ClParserValueMgrInst->createValue(list);
}

ClParserValuePtr
ClParserList::
divide(const ClParserObj &obj) const
{
  ClParserValuePtr value = ClParserValueMgrInst->createValue(obj);

  long result = countValues(value);

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserList::
modulus(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
power(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
approxEqual(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
bitAnd(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
bitOr(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
bitXor(const ClParserObj &) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
bitLShift(const ClParserObj &obj) const
{
  if (obj.getBaseType() != CL_PARSER_VALUE_TYPE_INTEGER)
    ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  const ClParserInteger &integer = ClParserInteger::castObj(obj);

  long num = integer.getValue();

  ClParserListPtr list = dupList();

  list->roll(int(num));

  return ClParserValueMgrInst->createValue(list);
}

ClParserValuePtr
ClParserList::
bitRShift(const ClParserObj &obj) const
{
  if (obj.getBaseType() != CL_PARSER_VALUE_TYPE_INTEGER)
    ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  const ClParserInteger &integer = ClParserInteger::castObj(obj);

  long num = -integer.getValue();

  ClParserListPtr list = dupList();

  list->roll(int(num));

  return ClParserValueMgrInst->createValue(list);
}

//-----------------

// Intern Fn

ClParserValuePtr
ClParserList::
abs() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
ceil() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
floor() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
sign() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
sqr() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
sqrt() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
cos() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
sin() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
tan() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
acos() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
asin() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
atan() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
atan(double) const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
exp() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
log() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
log10() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
cosh() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
sinh() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
tanh() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
norm() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
invnorm() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
toChar() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
toInt() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
toReal() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
toString() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
isNan() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
toLower() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
toUpper() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserList::
dim() const
{
  long result = getNumValues();

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserList::
len() const
{
  long result = getNumValues();

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserList::
min() const
{
  return getMinValue();
}

ClParserValuePtr
ClParserList::
max() const
{
  return getMaxValue();
}

ClParserValuePtr
ClParserList::
sum() const
{
  ClParserValuePtr value;

  auto p1 = values_.begin();
  auto p2 = values_.end  ();

  for ( ; p1 != p2; ++p1) {
    if (! value)
      value = *p1;
    else
      value = value->plus(*p1);
  }

  return value;
}

ClParserValuePtr
ClParserList::
index(const ClParserObj &obj) const
{
  ClParserValuePtr value = ClParserValueMgrInst->createValue(obj);

  long result = getValuePos(value) + 1;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserList::
rindex(const ClParserObj &obj) const
{
  ClParserValuePtr value = ClParserValueMgrInst->createValue(obj);

  long result = getRValuePos(value) + 1;

  return ClParserValueMgrInst->createValue(result);
}

ClParserValuePtr
ClParserList::
sort(ClParserSortDirection direction) const
{
  ClParserListPtr list = dupList();

  list->sortValues(direction);

  return ClParserValueMgrInst->createValue(list);
}

ClParserValuePtr
ClParserList::
doAssert() const
{
  assert(false);

  return ClParserValuePtr();
}

//-----------------

ClParserValuePtr
ClParserList::
subscriptValue(const uint *subscripts, uint num_subscripts) const
{
  error_code_ = 0;

  if (num_subscripts > 2) {
    error_code_ = int(ClErr::TOO_MANY_SUBSCRIPTS);
    return ClParserValuePtr();
  }

  if (num_subscripts == 1)
    return getValue(int(subscripts[0]));

  ClParserListPtr list = createList();

  for (uint i = subscripts[0]; i <= subscripts[1]; i++)
    list->addValue(getValue(int(i)));

  return ClParserValueMgrInst->createValue(list);
}

ClParserValuePtr
ClParserList::
subscriptValue(const UIntVectorT &subscripts) const
{
  error_code_ = 0;

  uint num_subscripts = uint(subscripts.size());

  if (num_subscripts > 2) {
    error_code_ = int(ClErr::TOO_MANY_SUBSCRIPTS);
    return ClParserValuePtr();
  }

  if (num_subscripts == 1)
    return getValue(int(subscripts[0]));

  ClParserListPtr list = createList();

  for (uint i = subscripts[0]; i <= subscripts[1]; i++)
    list->addValue(getValue(int(i)));

  return ClParserValueMgrInst->createValue(list);
}
