#include <CCeilPI.h>

ClParserStructPtr
ClParserStruct::
createStruct()
{
  auto *structure = new ClParserStruct;

  return ClParserStructPtr(structure);
}

ClParserStructPtr
ClParserStruct::
createStruct(ClParserTypePtr type)
{
  auto *structure = new ClParserStruct(type);

  return ClParserStructPtr(structure);
}

ClParserStructPtr
ClParserStruct::
createStruct(ClParserTypePtr type, const ClParserValuePtr *values, uint num_values)
{
  auto *structure = new ClParserStruct(type, values, num_values);

  return ClParserStructPtr(structure);
}

ClParserStructPtr
ClParserStruct::
createStruct(ClParserTypePtr type, const ClParserValueArray &values)
{
  auto *structure = new ClParserStruct(type, values);

  return ClParserStructPtr(structure);
}

ClParserStructPtr
ClParserStruct::
createStruct(const ClParserStruct &structure)
{
  auto *pstructure = new ClParserStruct(structure);

  return ClParserStructPtr(pstructure);
}

//------------

ClParserStruct::
ClParserStruct() :
 ClParserObj(CL_PARSER_VALUE_TYPE_STRUCTURE)
{
}

ClParserStruct::
ClParserStruct(ClParserTypePtr type) :
 ClParserObj(CL_PARSER_VALUE_TYPE_STRUCTURE), type_(type)
{
  if (type_ && type_->isBuiltinType())
    ClErrThrow(ClErr::INVALID_STRUCTURE_TYPE_NAME);

  uint num_sub_types = (type_ ? type_->getNumSubTypes() : 0);

  for (uint i = 0; i < num_sub_types; ++i)
    values_[type_->getSubType(i)->getName()] = type_->getSubType(i)->getValue();
}

ClParserStruct::
ClParserStruct(ClParserTypePtr type, const ClParserValueArray &values) :
 ClParserObj(CL_PARSER_VALUE_TYPE_STRUCTURE), type_(type)
{
  if (type_ && type_->isBuiltinType())
    ClErrThrow(ClErr::INVALID_STRUCTURE_TYPE_NAME);

  if (! type_)
    ClErrThrow(ClErr::INVALID_STRUCTURE_TYPE_NAME);

  uint num_values = uint(values.size());

  uint i = 0;

  for ( ; i < type_->getNumSubTypes() && i < num_values; ++i) {
    auto value1 = ClParserValueMgrInst->createValue(values[i]);

    if (! value1->convertToType(type_->getSubType(i)->getValue()))
      ClErrThrow(ClErr::INVALID_CONVERSION);

    values_[type_->getSubType(i)->getName()] = value1;
  }

  for ( ; i < type_->getNumSubTypes(); ++i)
    values_[type_->getSubType(i)->getName()] = type_->getSubType(i)->getValue();
}

ClParserStruct::
ClParserStruct(ClParserTypePtr type, const ClParserValuePtr *values, uint num_values) :
 ClParserObj(CL_PARSER_VALUE_TYPE_STRUCTURE), type_(type)
{
  if (type_ && type_->isBuiltinType())
    ClErrThrow(ClErr::INVALID_STRUCTURE_TYPE_NAME);

  if (! type_)
    ClErrThrow(ClErr::INVALID_STRUCTURE_TYPE_NAME);

  uint i = 0;

  for ( ; i < type_->getNumSubTypes() && i < num_values; ++i) {
    auto value1 = ClParserValueMgrInst->createValue(values[i]);

    if (! value1->convertToType(type_->getSubType(i)->getValue()))
      ClErrThrow(ClErr::INVALID_CONVERSION);

    values_[type_->getSubType(i)->getName()] = value1;
  }

  for ( ; i < type_->getNumSubTypes(); ++i)
    values_[type_->getSubType(i)->getName()] = type_->getSubType(i)->getValue();
}

ClParserStruct::
ClParserStruct(const ClParserStruct &structure) :
 ClParserObj(CL_PARSER_VALUE_TYPE_STRUCTURE), type_(structure.type_)
{
  auto p1 = structure.values_.begin();
  auto p2 = structure.values_.end  ();

  for ( ; p1 != p2; ++p1)
    values_[p1->first] = p1->second;
}

ClParserStruct::
~ClParserStruct()
{
}

ClParserStruct &
ClParserStruct::
operator=(const ClParserStruct &structure)
{
  type_ = structure.type_;

  if (type_ != structure.type_)
    values_.clear();

  auto p1 = structure.values_.begin();
  auto p2 = structure.values_.end  ();

  for ( ; p1 != p2; ++p1)
    values_[p1->first] = p1->second;

  return *this;
}

void
ClParserStruct::
copy(const ClParserObj &obj)
{
  assert(base_type_ == obj.getBaseType());

  const auto &rhs = castObj(obj);

  *this = rhs;
}

ClParserStruct *
ClParserStruct::
dup() const
{
  return new ClParserStruct(*this);
}

ClParserStructPtr
ClParserStruct::
dupStruct() const
{
  auto *structure = dup();

  return ClParserStructPtr(structure);
}

ClParserStruct::ValueMap::const_iterator
ClParserStruct::
getValuesBegin() const
{
  return values_.begin();
}

ClParserStruct::ValueMap::const_iterator
ClParserStruct::
getValuesEnd() const
{
  return values_.begin();
}

void
ClParserStruct::
addName(const std::string &name)
{
  if (! type_)
    type_ = ClParserInst->createType();

  type_->addSubType(name);

  values_[name] = ClParserValueMgrInst->createValue(0L);
}

bool
ClParserStruct::
getValue(const std::string &name, ClParserValuePtr &value) const
{
  auto p1 = values_.find(name);

  if (p1 == values_.end())
    return false;

  value = p1->second;

  return true;
}

bool
ClParserStruct::
setValue(const std::string &name, ClParserValuePtr value)
{
  auto p1 = values_.find(name);

  if (p1 == values_.end())
    return false;

  ClParserValuePtr value1 = p1->second;

  if (value1->getType() != value->getType()) {
    ClParserValuePtr value2 = ClParserValueMgrInst->createValue(value);

    if (value2->convertToType(value1))
      return false;

    p1->second = value2;
  }
  else
    p1->second = value;

  return true;
}

bool
ClParserStruct::
toBool() const
{
  for (const auto &pv : values_)
    if (pv.second->toBool())
      return true;

  return false;
}

int
ClParserStruct::
cmp(const ClParserObj &obj) const
{
  if (base_type_ != obj.getBaseType())
    return CMathGen::sign(long(base_type_ - obj.getBaseType()));

  const ClParserStruct &rhs = castObj(obj);

  if (type_ != rhs.type_) {
    if (type_->getName() < rhs.type_->getName())
      return -1;
    else
      return  1;
  }

  uint num_values1 = uint(    values_.size());
  uint num_values2 = uint(rhs.values_.size());

  if (num_values1 != num_values2)
    return int(num_values1 - num_values2);

  auto pl1 =     values_.begin();
  auto pl2 =     values_.end  ();
  auto pr1 = rhs.values_.begin();
  auto pr2 = rhs.values_.end  ();

  for ( ; pl1 != pl2 && pr1 != pr2; ++pl1, ++pr1) {
    int cmp = pl1->second->cmp(pl2->second);

    if (cmp != 0)
      return cmp;
  }

  return 0;
}

//------

std::string
ClParserStruct::
asString() const
{
  auto p1 = values_.begin();
  auto p2 = values_.end  ();

  std::string str;

  for (uint i = 0; p1 != p2; ++p1, ++i) {
    if (i > 0) str += " ";

    str += p1->second->asString();
  }

  return str;
}

void
ClParserStruct::
print() const
{
  ClParserInst->output("%s", asString().c_str());
}

void
ClParserStruct::
print(std::ostream &os) const
{
  os << asString();
}

void
ClParserStruct::
debugPrint() const
{
  fprintf(stderr, "{%s}", asString().c_str());
}

//-----------------

// Inline Ops

const ClParserObj &
ClParserStruct::
increment()
{
  for (auto &pv : values_)
    pv.second->increment();

  return *this;
}

const ClParserObj &
ClParserStruct::
decrement()
{
  for (auto &pv : values_)
    pv.second->decrement();

  return *this;
}

//-----------------

// Unary Ops

ClParserValuePtr
ClParserStruct::
unaryPlus() const
{
  auto structure = dupStruct();

  auto p1 = values_.begin();
  auto p2 = values_.end  ();

  for ( ; p1 != p2; ++p1)
    structure->setValue(p1->first, p1->second->unaryPlus());

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
unaryMinus() const
{
  auto structure = dupStruct();

  auto p1 = values_.begin();
  auto p2 = values_.end  ();

  for ( ; p1 != p2; ++p1)
    structure->setValue(p1->first, p1->second->unaryMinus());

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
bitNot() const
{
  auto structure = dupStruct();

  auto p1 = values_.begin();
  auto p2 = values_.end  ();

  for ( ; p1 != p2; ++p1)
    structure->setValue(p1->first, p1->second->bitNot());

  return ClParserValueMgrInst->createValue(structure);
}

//------

ClParserValuePtr
ClParserStruct::BinaryFunc::
process(const ClParserStruct &lstructure, const ClParserObj &robj) const
{
  const ClParserStruct &rstructure = castObj(robj);

  if (lstructure.getType() != rstructure.getType())
    ClErrThrow(ClErr::INVALID_TYPE_MIX);

  auto lstructure1 = lstructure.dupStruct();

  auto pl1 = lstructure1->values_.begin();
  auto pl2 = lstructure1->values_.end  ();
  auto pr1 = rstructure . values_.begin();

  for ( ; pl1 != pl2; ++pl1, ++pr1)
    exec(pl1->second, pr1->second);

  return ClParserValueMgrInst->createValue(lstructure1);
}

// Binary Ops

struct Plus : public ClParserStruct::BinaryFunc {
  void exec(ClParserValuePtr lhs, ClParserValuePtr rhs) const override {
    lhs->plus(rhs);
  }
};

ClParserValuePtr
ClParserStruct::
plus(const ClParserObj &obj) const
{
  return binaryOp<Plus>(obj);
}

struct Minus : public ClParserStruct::BinaryFunc {
  void exec(ClParserValuePtr lhs, ClParserValuePtr rhs) const override {
    lhs->minus(rhs);
  }
};

ClParserValuePtr
ClParserStruct::
minus(const ClParserObj &obj) const
{
  return binaryOp<Minus>(obj);
}

struct Times : public ClParserStruct::BinaryFunc {
  void exec(ClParserValuePtr lhs, ClParserValuePtr rhs) const override {
    lhs->times(rhs);
  }
};

ClParserValuePtr
ClParserStruct::
times(const ClParserObj &obj) const
{
  return binaryOp<Times>(obj);
}

struct Divide : public ClParserStruct::BinaryFunc {
  void exec(ClParserValuePtr lhs, ClParserValuePtr rhs) const override {
    lhs->divide(rhs);
  }
};

ClParserValuePtr
ClParserStruct::
divide(const ClParserObj &obj) const
{
  return binaryOp<Divide>(obj);
}

struct Modulus : public ClParserStruct::BinaryFunc {
  void exec(ClParserValuePtr lhs, ClParserValuePtr rhs) const override {
    lhs->modulus(rhs);
  }
};

ClParserValuePtr
ClParserStruct::
modulus(const ClParserObj &obj) const
{
  return binaryOp<Modulus>(obj);
}

struct Power : public ClParserStruct::BinaryFunc {
  void exec(ClParserValuePtr lhs, ClParserValuePtr rhs) const override {
    lhs->power(rhs);
  }
};

ClParserValuePtr
ClParserStruct::
power(const ClParserObj &obj) const
{
  return binaryOp<Power>(obj);
}

struct ApproxEqual : public ClParserStruct::BinaryFunc {
  void exec(ClParserValuePtr lhs, ClParserValuePtr rhs) const override {
    lhs->approxEqual(rhs);
  }
};

ClParserValuePtr
ClParserStruct::
approxEqual(const ClParserObj &obj) const
{
  return binaryOp<ApproxEqual>(obj);
}

struct BitAnd : public ClParserStruct::BinaryFunc {
  void exec(ClParserValuePtr lhs, ClParserValuePtr rhs) const override {
    lhs->bitAnd(rhs);
  }
};

ClParserValuePtr
ClParserStruct::
bitAnd(const ClParserObj &obj) const
{
  return binaryOp<BitAnd>(obj);
}

struct BitOr : public ClParserStruct::BinaryFunc {
  void exec(ClParserValuePtr lhs, ClParserValuePtr rhs) const override {
    lhs->bitOr(rhs);
  }
};

ClParserValuePtr
ClParserStruct::
bitOr(const ClParserObj &obj) const
{
  return binaryOp<BitOr>(obj);
}

struct BitXor : public ClParserStruct::BinaryFunc {
  void exec(ClParserValuePtr lhs, ClParserValuePtr rhs) const override {
    lhs->bitXor(rhs);
  }
};

ClParserValuePtr
ClParserStruct::
bitXor(const ClParserObj &obj) const
{
  return binaryOp<BitXor>(obj);
}

struct BitLShift : public ClParserStruct::BinaryFunc {
  void exec(ClParserValuePtr lhs, ClParserValuePtr rhs) const override {
    lhs->bitLShift(rhs);
  }
};

ClParserValuePtr
ClParserStruct::
bitLShift(const ClParserObj &obj) const
{
  return binaryOp<BitLShift>(obj);
}

struct BitRShift : public ClParserStruct::BinaryFunc {
  void exec(ClParserValuePtr lhs, ClParserValuePtr rhs) const override {
    lhs->bitRShift(rhs);
  }
};

ClParserValuePtr
ClParserStruct::
bitRShift(const ClParserObj &obj) const
{
  return binaryOp<BitRShift>(obj);
}

//------

// Internal Fns

ClParserValuePtr
ClParserStruct::
abs() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->abs();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
ceil() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->ceil();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
floor() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->floor();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
sign() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->sign();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
sqr() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->sqr();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
sqrt() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->sqrt();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
cos() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->cos();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
sin() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->sin();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
tan() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->tan();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
acos() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->acos();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
asin() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->asin();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
atan() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->atan();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
atan(double real) const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->atan(real);

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
exp() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->exp();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
log() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->log();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
log10() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->log10();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
cosh() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->cosh();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
sinh() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->sinh();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
tanh() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->tanh();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
norm() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserStruct::
invnorm() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserStruct::
toChar() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->toChar();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
toInt() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserStruct::
toReal() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserStruct::
toString() const
{
  ClErrThrow(ClErr::INVALID_TYPE_FOR_OPERATOR);

  return ClParserValuePtr();
}

ClParserValuePtr
ClParserStruct::
isNan() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->isNan();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
toLower() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->toLower();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
toUpper() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->toUpper();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
min() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->min();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
max() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->max();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
sum() const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->sum();

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
index(const ClParserObj &obj) const
{
  ClParserValuePtr value = ClParserValueMgrInst->createValue(obj);

  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->index(value);

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
rindex(const ClParserObj &obj) const
{
  ClParserValuePtr value = ClParserValueMgrInst->createValue(obj);

  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->rindex(value);

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
sort(ClParserSortDirection direction) const
{
  auto structure = dupStruct();

  for (auto &pv : structure->values_)
    pv.second->sort(direction);

  return ClParserValueMgrInst->createValue(structure);
}

ClParserValuePtr
ClParserStruct::
doAssert() const
{
  assert(false);

  return ClParserValuePtr();
}

